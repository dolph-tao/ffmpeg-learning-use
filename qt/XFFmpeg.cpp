#include "XFFmpeg.h"
#include <QDebug>
//����FFMpeg��lib��
#pragma comment(lib,"avformat.lib")
#pragma  comment(lib,"avutil.lib")
#pragma  comment(lib,"avcodec.lib")
#pragma  comment(lib,"swscale.lib")
#pragma  comment(lib,"swresample.lib")

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}
XFFmpeg::XFFmpeg()
{
	errorbuff[0] = '\0';
	av_register_all();//ע��FFMpeg�Ŀ�
}


XFFmpeg::~XFFmpeg()
{
}

int  XFFmpeg::Open(const char *path)
{

	Close();//��ǰ�ȹر�����
	mutex.lock();//��
	ic = avformat_alloc_context();
	const char *path11 = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
	int re = avformat_open_input(&ic, path11, nullptr, nullptr);//�򿪽��װ��
	if (re != 0)//�򿪴���ʱ
	{
		mutex.unlock();//����
		av_strerror(re, errorbuff, sizeof(errorbuff));//������Ϣ
		printf("open %s failed :%s\n", path, errorbuff);
		return 0;
	}
	totalMs = ic->duration / (AV_TIME_BASE);//��ȡ��Ƶ����ʱ��
	totalMs = 11110;

	//��ӡ��Ƶ��ϸ��Ϣ
	av_dump_format(ic, 0, path11, 0);
	//������
	for (int i = 0; i < ic->nb_streams; i++)
	{
		AVCodecContext *enc = ic->streams[i]->codec;//����������

		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)//�ж��Ƿ�Ϊ��Ƶ
		{
			videoStream = i;
			fps = r2d(ic->streams[i]->avg_frame_rate);//�����Ƶ��fps
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);//���ҽ�����
			if (!codec)//δ�ҵ�������
			{
				mutex.unlock();
				printf("video code not find\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);//�򿪽�����
			if (err != 0)//δ�򿪽�����
			{
				mutex.unlock();
				char buf[1024] = { 0 };
				av_strerror(err, buf, sizeof(buf));
				printf("not open!  %s",buf);
				return 0;
			}
			printf("open codec success!\n");
		}

		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)//��δ��Ƶ��
		{
			audioStream = i;//��Ƶ��
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);//���ҽ�����
			if (avcodec_open2(enc, codec, NULL) < 0)
			{
				mutex.unlock();
				return 0;
			}
			this->sampleRate = enc->sample_rate;//������
			this->channel = enc->channels;//ͨ����
			switch (enc->sample_fmt)//������С
			{
			case AV_SAMPLE_FMT_S16://signed 16 bits
				this->sampleSize = 16;
				break;
			case  AV_SAMPLE_FMT_S32://signed 32 bits
				this->sampleSize = 32;
			default:
				break;
			}
			printf("audio sample rate:%d sample size:%d chanle:%d\n",this->sampleRate,this->sampleSize,this->channel);

		}

	}//����Ϊ�򿪽���������

	printf("file totalSec is %d-%d\n", totalMs / 60, totalMs % 60);//�Է����ʱ
	mutex.unlock();
	return totalMs;


}

void XFFmpeg::Close()
{
	mutex.lock();//��Ҫ�������Է����߳�����������close����һ���߳����ڶ�ȡ��
	//qDebug() <<r2d(ic->streams[videoStream]->avg_frame_rate) ;
	if (ic) avformat_close_input(&ic);//�ر�ic�����Ŀռ�
	if (yuv) av_frame_free(&yuv);//�ر�ʱ�ͷŽ�������Ƶ֡�ռ�
	if (pcm) av_frame_free(&pcm);//�ر�ʱ�ͷŽ�������Ƶ�ռ�
	if (cCtx)
	{
		sws_freeContext(cCtx);//�ͷ�ת���������Ŀռ�
		cCtx = NULL;
		printf("shifang zhuangmaqi");
	}
	if (aCtx)
	{
		swr_free(&aCtx);//�ͷ���Ƶ�����Ŀռ�

	}
	//if ()
	//printf("");
	mutex.unlock();

}

std::string XFFmpeg::GetError()
{
	mutex.lock();
	std::string re = this->errorbuff;
	mutex.unlock();
	return re;
}

AVPacket XFFmpeg::Read()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	mutex.lock();
	if (!ic)
	{
		mutex.unlock();
		return pkt;
	}
	int err = av_read_frame(ic, &pkt);//��ȡ��Ƶ֡
	if (err != 0)//��ȡʧ��
	{
		av_strerror(err, errorbuff, sizeof(errorbuff));
	}
	mutex.unlock();
	return pkt;

}

int XFFmpeg::Decode(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic)//��δ����Ƶ
	{
		mutex.unlock();
		return NULL;

	}
	if (yuv == NULL)//�������Ķ���ռ�
	{
		yuv = av_frame_alloc();
	}
	if (pcm == NULL)
	{
		pcm = av_frame_alloc();
	}
	AVFrame *frame = yuv;//��ʱ��frame�ǽ�������Ƶ��
	if (pkt->stream_index == audioStream)//��δ��Ƶ
	{
		frame = pcm;//��ʱframe�ǽ�������Ƶ��
	}
	int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);//����֮ǰ��ȡ��pkt
	if (re != 0)
	{
		mutex.unlock();
		return NULL;
	}
	re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);//����pkt�����yuv��
	if (re != 0)
	{
		mutex.unlock();
		return NULL;
	}
	qDebug() << "pts=" << frame->pts;
	
	mutex.unlock();
    int p = frame->pts*r2d(ic->streams[pkt->stream_index]->time_base);//��ǰ�������ʾʱ��
	if (pkt->stream_index == audioStream)//Ϊ��Ƶ��ʱ����pts
		this->pts = p;

	return p;
}

bool XFFmpeg::ToRGB(char *out, int outwidth, int outheight)
{

	mutex.lock();
	if (!ic||!yuv)//δ����Ƶ�ļ�����δ����
	{
		mutex.unlock();
		return false;
	}
	AVCodecContext *videoCtx = ic->streams[this->videoStream]->codec;
	cCtx = sws_getCachedContext(cCtx, videoCtx->width,//��ʼ��һ��SwsContext
		videoCtx->height,
		videoCtx->pix_fmt, //�������ظ�ʽ
		outwidth, outheight,
		AV_PIX_FMT_BGRA,//������ظ�ʽ
		SWS_BICUBIC,//ת����㷨
		NULL, NULL, NULL);

	if (!cCtx)
	{
		mutex.unlock();
		printf("sws_getCachedContext  failed!\n");
		return false;
	}
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)out;//��һλ���RGB
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };

	linesize[0] = outwidth * 4;//һ�еĿ�ȣ�32λ4���ֽ�
	int h = sws_scale(cCtx, yuv->data, //��ǰ���������ÿ��ͨ������ָ��
		yuv->linesize,//ÿ��ͨ�����ֽ���
		0, videoCtx->height,//ԭ��Ƶ֡�ĸ߶�
		data,//�����ÿ��ͨ������ָ��	
		linesize//ÿ��ͨ�����ֽ���

		);//��ʼת��

	if (h > 0)
	{
		printf("(%d)", h);
	}
	mutex.unlock();
	return true;

}

bool XFFmpeg::Seek(float pos)
{
	mutex.lock();
	if (!ic)//δ����Ƶ
	{
		mutex.unlock();
		return false;
	}
	int64_t stamp = 0;
	stamp = pos * ic->streams[videoStream]->duration;//��ǰ��ʵ�ʵ�λ��
	pts = stamp * r2d(ic->streams[videoStream]->time_base);//��û������������ʱ���

	int re = av_seek_frame(ic, videoStream, stamp,
		AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);//����Ƶ��������ǰλ��
	avcodec_flush_buffers(ic->streams[videoStream]->codec);//ˢ�»���,�����
     mutex.unlock();
	if (re > 0)
		return true;
	return false;
}

int XFFmpeg::ToPCM(char *out)
{
	mutex.lock();
	if (!ic || !pcm || !out)//�ļ�δ�򿪣�������δ�򿪣�������
	{
		mutex.unlock();
		return 0;
	}
	AVCodecContext *ctx = ic->streams[audioStream]->codec;//��Ƶ������������
	if (aCtx == NULL)
	{
		aCtx = swr_alloc();//��ʼ��
		swr_alloc_set_opts(aCtx,ctx->channel_layout,
			AV_SAMPLE_FMT_S16,
			  ctx->sample_rate,
			  ctx->channels,
			  ctx->sample_fmt,
			  ctx->sample_rate,
			  0,0
			  );
		swr_init(aCtx);
	}
	uint8_t  *data[1];
	data[0] = (uint8_t *)out;
	int len = swr_convert(aCtx, data, 10000,
		(const uint8_t **)pcm->data,
		pcm->nb_samples
		);
	if (len <= 0)
	{
		mutex.unlock();
		return 0;
	}
	int outsize = av_samples_get_buffer_size(NULL, ctx->channels,
		pcm->nb_samples,
		AV_SAMPLE_FMT_S16,
		0);

	mutex.unlock();
	return outsize;
}

int XFFmpeg::GetPts(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic)
	{
		mutex.unlock();
		return -1;
	}
	int pts = pkt->pts*r2d(ic->streams[pkt->stream_index]->time_base);
	mutex.unlock();
	return pts;
}
