#include "XVideoThread.h"
#include "XFFmpeg.h"
#include "XAudioPlay.h"
#include <list>
using namespace std;

static list<AVPacket> videos;//用来存放解码前的视频帧
bool isexit = false;//线程未退出
static int apts = -1;//音频的pts
XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
}

void XVideoThread::run()
{
	char out[10000] = {0};
	while (!isexit)//线程未退出
	{
		if (!XFFmpeg::Get()->isPlay)//如果为暂停状态，不处理
		{
			msleep(10);
			continue;
		}
		while (videos.size()>0)//确定list中是否有AVpacket包
		{
			AVPacket pack = videos.front();//每次取出list中的第一个AVPack包
			int pts = XFFmpeg::Get()->GetPts(&pack);//获得该包的pts
			if (pts > apts)//若视屏包大于音频包的pts，结束
			{
				break;
			}
	 		XFFmpeg::Get()->Decode(&pack);//解码视频帧
			av_packet_unref(&pack);//清理该AVPacket包
			videos.pop_front();//从list链表中删除
		}

		int free = XAudioPlay::Get()->GetFree();//此时缓冲区的空间大小
		if (free < 10000)
		{
			msleep(1);
			continue;
		}
		AVPacket pkt = XFFmpeg::Get()->Read();
		if (pkt.size <= 0)//未打开视频
		{
			msleep(10);
			continue;
		}
		if (pkt.stream_index == XFFmpeg::Get()->audioStream)
		{
			apts = XFFmpeg::Get()->Decode(&pkt);//解码音频
			av_packet_unref(&pkt);//释放pkt包S
			int len = XFFmpeg::Get()->ToPCM(out);//重采样音频
			XAudioPlay::Get()->Write(out, len);//写入音频
			continue;
		}
// 		XFFmpeg::Get()->Decode(&pkt);//解码视频帧
// 		av_packet_unref(&pkt); 
		videos.push_back(pkt);
	}


}
