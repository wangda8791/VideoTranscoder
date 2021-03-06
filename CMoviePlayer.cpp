#include <QVBoxLayout>
#include <QHBoxLayout>

#include "CMoviePlayer.h"
#include "utils.h"

CMoviePlayer::CMoviePlayer(QWidget *parent) :
    QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;
	setLayout(vLayout);
	vLayout->setMargin(0);

    m_lblBack = new QLabel(tr("No movie loaded"));
	m_lblBack->setBackgroundRole(QPalette::Dark);
	m_lblBack->setAlignment(Qt::AlignCenter);
	m_lblBack->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	m_lblBack->setAutoFillBackground(true);
    vLayout->addWidget(m_lblBack, 10);

    QHBoxLayout* hLayout = new QHBoxLayout;
    m_seekSlider = new QSlider(Qt::Horizontal);
    m_seekSlider->setFixedHeight(20);
	m_seekSlider->setTickInterval(10);
	m_seekSlider->setEnabled(false);

    m_playButton = new QPushButton("►");
    m_playButton->setMinimumSize(30, 30);
    m_playButton->setMaximumSize(30, 30);
    m_playButton->setStyleSheet("border:none");
    m_pauseButton = new QPushButton("||");
    m_pauseButton->setMinimumSize(30, 30);
    m_pauseButton->setMaximumSize(30, 30);
    m_pauseButton->setStyleSheet("border:none");
    m_pauseButton->hide();
    hLayout->addWidget(m_playButton, 1);
    hLayout->addWidget(m_pauseButton, 1);
    hLayout->addWidget(m_seekSlider, 10);
	hLayout->addWidget(&m_timebar, 1);

	vLayout->addLayout(hLayout, 1);

	m_pDecoder = new CVideoDecoder;

	m_current_time = 0;
	m_timer = NULL;
	m_state = READY;
	m_slider_onchange = false;
	
    //m_audio_device = NULL;
    //m_audio_output = NULL;
    //m_audio_io = NULL;

	connect(m_seekSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnSeek(int)));
	connect(m_seekSlider, SIGNAL(sliderPressed()), this, SLOT(OnSliderPressed()));
	connect(m_seekSlider, SIGNAL(sliderReleased()), this, SLOT(OnSliderReleased()));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(OnPlay()));
    connect(m_pauseButton, SIGNAL(clicked()), this, SLOT(OnPause()));
    connect(this, SIGNAL(seek(int64_t)), this, SLOT(OnSeek(int)));

    UpdateTimeBar();
}

bool CMoviePlayer::Open(QString filename)
{
	m_pDecoder->Close();
	m_pDecoder->Init();

    QByteArray buf = filename.toLatin1();
	char* fn = buf.data();

	m_state = LOADING;
	m_lblBack->setBackgroundRole(QPalette::ButtonText);
	m_lblBack->setText("<span style='color:white;'>Loading...</span>");

	if (!m_pDecoder->Open(fn))
	{
		m_state = READY;
		m_lblBack->setBackgroundRole(QPalette::Dark);
		m_lblBack->setText("No movie loaded");

		return false;
	}

    m_lblBack->setBackgroundRole(QPalette::Dark);

    //if (m_pDecoder->IsAudio())
    //{
    //	m_audio_format.setSampleRate(m_pDecoder->GetSampleRate());
    //	m_audio_format.setChannelCount(m_pDecoder->GetChannels());
    //	m_audio_format.setSampleSize(m_pDecoder->GetSampleDepth());
    //	m_audio_format.setCodec("audio/pcm");
    //	m_audio_format.setByteOrder(QAudioFormat::LittleEndian);
    //	m_audio_format.setSampleType(QAudioFormat::UnSignedInt);

    //	m_audio_device = new QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());

    //	QStringList lst = m_audio_device->supportedCodecs();
    //	if (!m_audio_device->isFormatSupported(m_audio_format)) {
    //		printf("raw audio format not supported by backend, cannot play audio.");
    //		return false;
    //	}
    //	m_audio_output = new QAudioOutput(*m_audio_device, m_audio_format, this);
    //	m_audio_io = m_audio_output->start();
    //}

	m_seekSlider->setEnabled(true);
	m_state = PAUSED;

    m_cropTop = 0;
    m_cropRight = 0;
    m_cropBottom = 0;
    m_cropLeft = 0;

    m_segmentStart = 0;
    m_segmentFinish = GetDuration();

	return true;
}

void CMoviePlayer::Play()
{
	Pause();

	m_timer = new QTimer(this);
	connect(m_timer,SIGNAL(timeout()),this,SLOT(OnTimer()));
	m_timer->start(1000 / m_pDecoder->GetFPS());

    this->m_playButton->hide();
    this->m_pauseButton->show();

	m_state = PLAYING;
}

void CMoviePlayer::Pause()
{
	if (m_timer)
	{
		m_timer->stop();
		delete m_timer;
		m_timer = NULL;
	}

	if (m_state == LOADING)
	{
        m_lblBack->setBackgroundRole(QPalette::Dark);
		m_lblBack->setText("No movie loaded");
	}

    this->m_playButton->show();
    this->m_pauseButton->hide();

	m_state = PAUSED;
}

void CMoviePlayer::Stop()
{
    this->Pause();
	m_state = READY;
	if (m_pDecoder)
		m_pDecoder->Close();
	m_seekSlider->setValue(0);
	m_seekSlider->setEnabled(false);
	m_lblBack->setBackgroundRole(QPalette::Dark);
	m_lblBack->setText("No movie loaded");
}

int64_t CMoviePlayer::CurrentTime()
{
	return m_current_time;
}

int64_t	CMoviePlayer::GetDuration()
{
	return m_pDecoder->GetDuration();
}

int CMoviePlayer::GetState()
{
	return m_state;
}

void CMoviePlayer::OnTimer()
{
	int64_t current;
	if ((current = m_pDecoder->GetNextFrame()) == -1)
		return;

    if (current != -2) { //if not audio
        if (current - m_current_time >= 0 || current - m_current_time < -1000) {
            m_current_time = current;
        }
    }

	RenderFrame();
    //OutSamples();
	if (!m_slider_onchange)
	{
		UpdateSeekSlider();
    }
    UpdateTimeBar();

	emit tick(m_current_time);
}

void CMoviePlayer::RenderFrame()
{
	uint8_t* pic = m_pDecoder->GetCurFrame();
	if (pic == NULL)
		return;

	QImage img((const unsigned char*)pic, m_pDecoder->GetWidth(), m_pDecoder->GetHeight(), QImage::Format_RGB888);
	QPixmap pix(QPixmap::fromImage(img));

    int width = m_pDecoder->GetWidth() - m_cropLeft - m_cropRight;
    int height = m_pDecoder->GetHeight() - m_cropTop - m_cropBottom;
    QRect rect(m_cropLeft, m_cropTop, width, height);
    QPixmap cropped = pix.copy(rect);

    if (CurrentTime() < m_segmentStart || CurrentTime() > m_segmentFinish ) {
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QPainter p(&cropped);
        p.setPen(pen);
        p.drawLine(0, 0, width, height);
        p.drawLine(width, 0, 0, height);
    }

    pix = cropped.scaled(m_lblBack->width(), m_lblBack->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	m_lblBack->setPixmap(pix);
}

//void CMoviePlayer::OutSamples()
//{
//	if (m_pDecoder->IsAudio())
//	{
//		while (true)
//		{
//			ICBS_Stream *cur_stream = m_pDecoder->GetCurSamples();
//			if (cur_stream == NULL)
//				break;
//			m_audio_io->write((const char*)cur_stream->ptr(), cur_stream->size());
//		}
//	}
//}

void CMoviePlayer::UpdateTimeBar()
{
    m_timebar.setText(Utils::GetTimeString(m_current_time) + "/" + Utils::GetTimeString(GetDuration()));
}

void CMoviePlayer::UpdateSeekSlider()
{
	m_seekSlider->setMaximum(GetDuration());
	m_seekSlider->setValue(m_current_time);
}

void CMoviePlayer::OnSeek(int pos)
{
    m_pDecoder->Seek(pos);
    OnTimer();
}

void CMoviePlayer::OnSliderPressed()
{
	m_slider_onchange = true;
}

void CMoviePlayer::OnSliderReleased()
{
	m_slider_onchange = false;
	m_pDecoder->Seek(m_seekSlider->value());
	OnTimer();
	emit seek(m_seekSlider->value());
}

void CMoviePlayer::OnPlay()
{
    this->Play();
}

void CMoviePlayer::OnPause()
{
    this->Pause();
}

void CMoviePlayer::Crop(int top, int right, int bottom, int left)
{
    this->m_cropTop = top;
    this->m_cropRight = right;
    this->m_cropBottom = bottom;
    this->m_cropLeft = left;

    RenderFrame();
}

void CMoviePlayer::SetSegment(int start, int finish)
{
    this->m_segmentStart = start;
    this->m_segmentFinish = finish;

    RenderFrame();
}

void CMoviePlayer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == 32) {
        if (m_state == PLAYING)
            Pause();
        OnTimer();
    }
}
    b`�Z�G��͗�e�n���H�+?:pw�#}/����u�\G8��t�TGŝ"b�r����K��{��^ �� ��4�zxעU��>:�L�EB�B�e9�5!���GT��O+��H��0�Lq�P�A��X8�qs�h5R�y�����-�ta�d�`������]� ��'!t��T;���ZP�:�