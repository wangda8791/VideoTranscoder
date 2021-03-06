#include "CVideoDecoder.h"

CVideoDecoder::CVideoDecoder()
{
	img_convert_ctx = NULL;
	m_video_streamID = -1;
	m_audio_streamID = -1;

	m_videostream = NULL;
	m_audiostream = NULL;
	m_videoFrame = NULL;
	m_audioStream = NULL;
	m_cur_audio = NULL;
	img_convert_ctx = NULL;

	m_AVFC = NULL;
	m_picRGB = NULL;
	
	Init();
}

bool CVideoDecoder::Open(char* filename)
{
    if(avformat_open_input(&m_AVFC,filename,NULL,NULL)<0)
    {
        return false;
    }

    if(avformat_find_stream_info(m_AVFC,NULL)<0)
    {
        return false;
    }

    for(int i=0; i<(int)m_AVFC->nb_streams; i++)
    {
        AVCodecContext *avcc = m_AVFC->streams[i]->codec;

		if(AVMEDIA_TYPE_VIDEO == avcc->codec_type)
        {
            AVCodec *codec = avcodec_find_decoder(avcc->codec_id);
            if(!codec)
            {
                return false;
            }
			if (!avcodec_is_open(avcc))
				avcodec_open2(avcc,codec,NULL);

			m_video_streamID = i;
            m_videostream = m_AVFC->streams[i];
		}

		if(AVMEDIA_TYPE_AUDIO == avcc->codec_type)
		{
			AVCodec *codec = avcodec_find_decoder(avcc->codec_id);
			if(!codec)
			{
				return false;
			}
			if (!avcodec_is_open(avcc))
				avcodec_open2(avcc,codec,NULL);

			m_audio_streamID = i;
			m_audiostream = m_AVFC->streams[i];
            //m_audioStream = ICBS_Stream::Create();
            //m_cur_audio = ICBS_Stream::Create();
		}
    }
    
	m_duration = m_videostream->duration - m_videostream->start_time;

	return true;
}

void CVideoDecoder::Init()
{
	m_AVFC = avformat_alloc_context();
	
	m_picRGB = avcodec_alloc_frame();
	
	memset(&packet,0,sizeof(packet));
}

CVideoDecoder::~CVideoDecoder()
{
	Close();
}

void CVideoDecoder::Close()
{
    if(m_audiostream){
		if( m_audiostream->codec->extradata )
			av_free( m_audiostream->codec->extradata );
		//if (avcodec_is_open(m_audiostream->codec))
		//	avcodec_close( m_audiostream->codec );
		av_free( m_audiostream->codec );
		av_free( m_audiostream );

		m_audiostream = NULL;
	}

	if(m_videostream){
		if( m_videostream->codec->extradata )
			av_free( m_videostream->codec->extradata );
		//if (avcodec_is_open(m_videostream->codec))
		//	avcodec_close( m_videostream->codec );
		av_free( m_videostream->codec );
		av_free( m_videostream );

		m_videostream = NULL;
	}

	if (m_AVFC != NULL)
	{
		av_free( m_AVFC );
		m_AVFC = NULL;
	}

	if(img_convert_ctx){
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = NULL;
	}
	
	if(m_videoFrame){
		av_free(m_videoFrame);
		m_videoFrame = NULL;
	}

    if(m_audioStream){
        //m_audioStream->Release();
        m_audioStream = NULL;
	}

	if(m_picRGB && m_picRGB->data[0]){
		free(m_picRGB->data[0]);
		m_picRGB->data[0] = NULL;
		av_free(m_picRGB);
		m_picRGB = NULL;
	}

	m_video_streamID = -1;
	m_audio_streamID = -1;

	if(packet.data){
		av_free_packet(&packet);
		packet.data = NULL;
	}
}

int CVideoDecoder::GetFrameCount()
{
	int count = m_AVFC->streams[m_video_streamID]->nb_frames;
	return count;
}

double CVideoDecoder::GetFPS()
{
	if(m_videostream)
	{
		return (double)m_AVFC->streams[m_video_streamID]->r_frame_rate.num 
			/ m_AVFC->streams[m_video_streamID]->r_frame_rate.den;
	}

	return 0;
}

int64_t CVideoDecoder::GetNextFrame()
{
	int got_picture = 0;

	if(m_AVFC == NULL || m_videostream == NULL)
		return -1;
	
	av_free_packet(&packet);

	while (true)
	{
		if (av_read_frame(m_AVFC, &packet) < 0)
			return -1;

		if(packet.stream_index == m_audio_streamID)
		{
            //AVFrame* m_audioFrame = avcodec_alloc_frame();
            //avcodec_decode_audio4(m_audiostream->codec, m_audioFrame, &got_picture, &packet);
            //if (got_picture)
            //{
            //	int len = m_audioFrame->nb_samples * m_audioFrame->channels * av_get_bytes_per_sample(m_audiostream->codec->sample_fmt);
            //	m_audioStream->push32(len);
            //	m_audioStream->push(m_audioFrame->data[0], len);
            //}
            //av_free(m_audioFrame);
		}
		else if(packet.stream_index == m_video_streamID)
		{
			m_videoFrame = avcodec_alloc_frame();
			avcodec_decode_video2(m_videostream->codec, m_videoFrame, &got_picture, &packet);
			
			AVRational m_time;
			m_time.num = 1;
			m_time.den = 1000;

			return av_rescale_q(packet.pts - m_videostream->start_time, m_videostream->time_base, m_time);
		}
	}

	return -2;
}

void CVideoDecoder::Seek(int pos)
{
	if (m_AVFC == NULL)
		return;

	double time_base = (double)m_AVFC->streams[m_video_streamID]->time_base.num / 
		m_AVFC->streams[m_video_streamID]->time_base.den;

	int time_stamp = m_AVFC->streams[m_video_streamID]->start_time;
	time_stamp += (int)(pos / 1000 / time_base+0.5);
	
	av_seek_frame(m_AVFC,m_video_streamID,time_stamp,AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(m_videostream->codec);
	for (int i=0; i<m_videostream->codec->gop_size; i++)
		GetNextFrame();
    while (m_videoFrame && GetCurFrame() == NULL)
	{
        if (GetNextFrame() == -1)
			break;
	}
}

uint8_t* CVideoDecoder::GetCurFrame()
{
	if (!m_videostream || !(m_videoFrame->data) || !(m_videoFrame->data[0]))
		return NULL;

	if (m_picRGB->data[0] != NULL)
	{
		free (m_picRGB->data[0]);
		m_picRGB->data[0] = NULL;
	}

	m_picRGB->data[0] = (uint8_t*)malloc(avpicture_get_size(AV_PIX_FMT_RGB24, m_videostream->codec->width, m_videostream->codec->height));
	avpicture_fill((AVPicture*)m_picRGB, m_picRGB->data[0], AV_PIX_FMT_RGB24, m_videostream->codec->width, m_videostream->codec->height);

	if (img_convert_ctx)
		sws_freeContext(img_convert_ctx);

	img_convert_ctx = sws_getCachedContext(NULL,
		m_videostream->codec->width, m_videostream->codec->height, m_videostream->codec->pix_fmt, 
		m_videostream->codec->width, m_videostream->codec->height, AV_PIX_FMT_RGB24, 
		SWS_BICUBIC,NULL,NULL,NULL);

	if (img_convert_ctx == NULL)
		return NULL;

	sws_scale(img_convert_ctx, m_videoFrame->data, m_videoFrame->linesize,0 , 
		m_videostream->codec->height, m_picRGB->data, m_picRGB->linesize);

	return m_picRGB->data[0];
}

ICBS_Stream* CVideoDecoder::GetCurSamples()
{
    if (m_audioStream->size() == 0)
		return NULL;

	m_cur_audio->reset();

	int len = m_audioStream->pop32();
	m_cur_audio->push(m_audioStream->ptr(), len);
	m_audioStream->pop(len);

	return m_cur_audio;
}

int	CVideoDecoder::GetWidth()
{
	return m_videostream->codec->width;
}

int	CVideoDecoder::GetHeight()
{
	return m_videostream->codec->height;
}

int64_t CVideoDecoder::GetDuration()
{
	if (m_videostream == NULL)
		return 0;

	int64_t cur_ts = m_videostream->cur_dts - m_videostream->start_time;
	if (m_duration < cur_ts)
		m_duration = cur_ts;

	return m_duration * av_q2d(m_videostream->time_base) * 1000;
}

bool CVideoDecoder::IsVideo()
{
	return m_videostream != NULL;
}

bool CVideoDecoder::IsAudio()
{
	return m_audiostream != NULL;
}

int CVideoDecoder::GetSampleRate()
{
	return m_audiostream->codec->sample_rate;
}

int CVideoDecoder::GetChannels()
{
	return m_audiostream->codec->channels;
}

int CVideoDecoder::GetSampleDepth()
{
	return av_get_bytes_per_sample(m_audiostream->codec->sample_fmt) * 8;
}
    KIʿ�$_92��S�*Q%$��}C�Ψb�>��)D����o�i��=	�J�5X�9�9a�\g'g9y���=���\�PL��M��}�C~�]��3��}�y���ek����{������ZQEvO\dn�Q��:��1���bs�}�Cۻ�����N�N[Q&?��_m�O�Y���E� [��-ɘ