#ifndef CVIDEODECODER_H
#define CVIDEODECODER_H

#ifndef UINT64_C
#define UINT64_C(x) x ## ULL
#endif

#include <QtCore>
#include "libcbs.h"

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libswscale/swscale.h>
}

class CVideoDecoder
{
public:
    CVideoDecoder();
	~CVideoDecoder();

	void Init();
    bool Open(char*);
    void Close();

    int		GetFrameCount();
    double	GetFPS();

    int64_t	GetNextFrame();
    void	Seek(int pos);

	int		GetWidth();
	int		GetHeight();

	uint8_t*	GetCurFrame();
	ICBS_Stream*GetCurSamples();
	int64_t		GetDuration();
	bool		IsVideo();
	bool		IsAudio();
	int			GetSampleRate();
	int			GetChannels();
	int			GetSampleDepth();

protected:
    AVFormatContext *m_AVFC;

    int m_video_streamID;
	int	m_audio_streamID;
    AVStream* m_videostream;
	AVStream* m_audiostream;

    AVFrame* m_picRGB;
    AVFrame* m_videoFrame;
	ICBS_Stream* m_audioStream;
	ICBS_Stream* m_cur_audio;
	int		m_cur_size;

	int64_t		m_duration;

	struct SwsContext* img_convert_ctx;
	AVPacket packet;
};

#endif // CVIDEODECODER_H
    ;�Έ�.�r4�n��y� %Kj����A�0�Ą��hEMgXF˵j*�� 4y��p�L�����ANVU��qB�!�|(���70��n��N�]�8�c;���1dQ{������W�y?�0S��ƨ�4�DUW���z��PDXY4����P��C(�B�eq?W{�46J�:*��_r~sd܃��ѭ6<4��}