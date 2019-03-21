#include "XFFmpeg.h"
#include <QDebug>
//调用FFMpeg的lib库
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
	av_register_all();//注册FFMpeg的库
}


XFFmpeg::~XFFmpeg()
{
}

int  XFFmpeg::Open(const char *path)
{

	Close();//打开前先关闭清理
	mutex.lock();//锁
	ic = avformat_alloc_context();
	const char *path11 = "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8";
	int re = avformat_open_input(&ic, path11, nullptr, nullptr);//打开解封装器
	if (re != 0)//打开错误时
	{
		mutex.unlock();//解锁
		av_strerror(re, errorbuff, sizeof(errorbuff));//错误信息
		printf("open %s failed :%s\n", path, errorbuff);
		return 0;
	}
	totalMs = ic->duration / (AV_TIME_BASE);//获取视频的总时间
	totalMs = 11110;

	//打印视频详细信息
	av_dump_format(ic, 0, path11, 0);
	//解码器
	for (int i = 0; i < ic->nb_streams; i++)
	{
		AVCodecContext *enc = ic->streams[i]->codec;//解码上下文

		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)//判断是否为视频
		{
			videoStream = i;
			fps = r2d(ic->streams[i]->avg_frame_rate);//获得视频得fps
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);//查找解码器
			if (!codec)//未找到解码器
			{
				mutex.unlock();
				printf("video code not find\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);//打开解码器
			if (err != 0)//未打开解码器
			{
				mutex.unlock();
				char buf[1024] = { 0 };
				av_strerror(err, buf, sizeof(buf));
				printf("not open!  %s",buf);
				return 0;
			}
			printf("open codec success!\n");
		}

		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)//若未音频流
		{
			audioStream = i;//音频流
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);//查找解码器
			if (avcodec_open2(enc, codec, NULL) < 0)
			{
				mutex.unlock();
				return 0;
			}
			this->sampleRate = enc->sample_rate;//样本率
			this->channel = enc->channels;//通道数
			switch (enc->sample_fmt)//样本大小
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

	}//至此为打开解码器过程

	printf("file totalSec is %d-%d\n", totalMs / 60, totalMs % 60);//以分秒计时
	mutex.unlock();
	return totalMs;


}

void XFFmpeg::Close()
{
	mutex.lock();//需要上锁，以防多线程中你这里在close，另一个线程中在读取，
	//qDebug() <<r2d(ic->streams[videoStream]->avg_frame_rate) ;
	if (ic) avformat_close_input(&ic);//关闭ic上下文空间
	if (yuv) av_frame_free(&yuv);//关闭时释放解码后的视频帧空间
	if (pcm) av_frame_free(&pcm);//关闭时释放解码后的音频空间
	if (cCtx)
	{
		sws_freeContext(cCtx);//释放转码器上下文空间
		cCtx = NULL;
		printf("shifang zhuangmaqi");
	}
	if (aCtx)
	{
		swr_free(&aCtx);//释放音频上下文空间

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
	int err = av_read_frame(ic, &pkt);//读取视频帧
	if (err != 0)//读取失败
	{
		av_strerror(err, errorbuff, sizeof(errorbuff));
	}
	mutex.unlock();
	return pkt;

}

int XFFmpeg::Decode(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic)//若未打开视频
	{
		mutex.unlock();
		return NULL;

	}
	if (yuv == NULL)//申请解码的对象空间
	{
		yuv = av_frame_alloc();
	}
	if (pcm == NULL)
	{
		pcm = av_frame_alloc();
	}
	AVFrame *frame = yuv;//此时的frame是解码后的视频流
	if (pkt->stream_index == audioStream)//若未音频
	{
		frame = pcm;//此时frame是解码后的音频流
	}
	int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);//发送之前读取的pkt
	if (re != 0)
	{
		mutex.unlock();
		return NULL;
	}
	re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);//解码pkt后存入yuv中
	if (re != 0)
	{
		mutex.unlock();
		return NULL;
	}
	qDebug() << "pts=" << frame->pts;
	
	mutex.unlock();
    int p = frame->pts*r2d(ic->streams[pkt->stream_index]->time_base);//当前解码的显示时间
	if (pkt->stream_index == audioStream)//为音频流时设置pts
		this->pts = p;

	return p;
}

bool XFFmpeg::ToRGB(char *out, int outwidth, int outheight)
{

	mutex.lock();
	if (!ic||!yuv)//未打开视频文件或者未解码
	{
		mutex.unlock();
		return false;
	}
	AVCodecContext *videoCtx = ic->streams[this->videoStream]->codec;
	cCtx = sws_getCachedContext(cCtx, videoCtx->width,//初始化一个SwsContext
		videoCtx->height,
		videoCtx->pix_fmt, //输入像素格式
		outwidth, outheight,
		AV_PIX_FMT_BGRA,//输出像素格式
		SWS_BICUBIC,//转码的算法
		NULL, NULL, NULL);

	if (!cCtx)
	{
		mutex.unlock();
		printf("sws_getCachedContext  failed!\n");
		return false;
	}
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)out;//第一位输出RGB
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };

	linesize[0] = outwidth * 4;//一行的宽度，32位4个字节
	int h = sws_scale(cCtx, yuv->data, //当前处理区域的每个通道数据指针
		yuv->linesize,//每个通道行字节数
		0, videoCtx->height,//原视频帧的高度
		data,//输出的每个通道数据指针	
		linesize//每个通道行字节数

		);//开始转码

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
	if (!ic)//未打开视频
	{
		mutex.unlock();
		return false;
	}
	int64_t stamp = 0;
	stamp = pos * ic->streams[videoStream]->duration;//当前它实际的位置
	pts = stamp * r2d(ic->streams[videoStream]->time_base);//获得滑动条滑动后的时间戳

	int re = av_seek_frame(ic, videoStream, stamp,
		AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);//将视频移至到当前位置
	avcodec_flush_buffers(ic->streams[videoStream]->codec);//刷新缓冲,清理掉
     mutex.unlock();
	if (re > 0)
		return true;
	return false;
}

int XFFmpeg::ToPCM(char *out)
{
	mutex.lock();
	if (!ic || !pcm || !out)//文件未打开，解码器未打开，无数据
	{
		mutex.unlock();
		return 0;
	}
	AVCodecContext *ctx = ic->streams[audioStream]->codec;//音频解码器上下文
	if (aCtx == NULL)
	{
		aCtx = swr_alloc();//初始化
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
