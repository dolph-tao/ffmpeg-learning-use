#pragma once
#include <iostream>
#include <QMutex>
extern "C"
{
	//调用FFMpeg的头文件
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class XFFmpeg
{
public:
	static XFFmpeg *Get()//单件模式
	{
		static XFFmpeg ff;
		return &ff;
	}

   /*打开文件返回视频文件的总时间
     若时间小于0，同样未打开成功
   */
	int Open(const char *path);
	void Close();//关闭文件

	//读取视频的每一帧，返回每帧后需要清理空间
	AVPacket Read();

	//读取到每帧数据后需要对其进行解码,返回它的pts
	int Decode(const AVPacket *pkt);

	//将解码后的YUV视频帧转化为RGB格式
	bool ToRGB(char *out,int outwidth,int outheight);

	int ToPCM(char *out);//音频的重采样

	std::string GetError();//获取错误信息
	virtual ~XFFmpeg();
	int totalMs = 0;//总时长
	int videoStream = 0;//视频流
	int audioStream = 1;//音频流
	int fps = 0;//每秒的视频帧数

	bool Seek(float pos);//设置进度条的拖动位置后播放

	bool isPlay = false;//播放暂停

	int pts;//获得当前解码帧的时间
	int GetPts(const AVPacket *pkt);//获得视频解码前视频帧的pts

	int sampleRate = 48000;//样本率
	int sampleSize = 16;//样本大小
	int channel = 2;///通道数
protected:
	char errorbuff[1024];//打开时发生的错误信息
	XFFmpeg();
	QMutex mutex;//互斥变量，多线程时避免同时间的读写
	AVFormatContext *ic = NULL;//解封装上下文
	AVFrame *yuv = NULL;//解码后的视频帧数据
	AVFrame *pcm = NULL;//解码后的音频数据
	SwsContext  *cCtx = NULL;//视频转码上下文
	SwrContext *aCtx = NULL;//音频重采样上下文
};

