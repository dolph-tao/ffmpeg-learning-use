#include "XVideoThread.h"
#include "XFFmpeg.h"
#include "XAudioPlay.h"
#include <list>
using namespace std;

static list<AVPacket> videos;//������Ž���ǰ����Ƶ֡
bool isexit = false;//�߳�δ�˳�
static int apts = -1;//��Ƶ��pts
XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
}

void XVideoThread::run()
{
	char out[10000] = {0};
	while (!isexit)//�߳�δ�˳�
	{
		if (!XFFmpeg::Get()->isPlay)//���Ϊ��ͣ״̬��������
		{
			msleep(10);
			continue;
		}
		while (videos.size()>0)//ȷ��list���Ƿ���AVpacket��
		{
			AVPacket pack = videos.front();//ÿ��ȡ��list�еĵ�һ��AVPack��
			int pts = XFFmpeg::Get()->GetPts(&pack);//��øð���pts
			if (pts > apts)//��������������Ƶ����pts������
			{
				break;
			}
	 		XFFmpeg::Get()->Decode(&pack);//������Ƶ֡
			av_packet_unref(&pack);//�����AVPacket��
			videos.pop_front();//��list������ɾ��
		}

		int free = XAudioPlay::Get()->GetFree();//��ʱ�������Ŀռ��С
		if (free < 10000)
		{
			msleep(1);
			continue;
		}
		AVPacket pkt = XFFmpeg::Get()->Read();
		if (pkt.size <= 0)//δ����Ƶ
		{
			msleep(10);
			continue;
		}
		if (pkt.stream_index == XFFmpeg::Get()->audioStream)
		{
			apts = XFFmpeg::Get()->Decode(&pkt);//������Ƶ
			av_packet_unref(&pkt);//�ͷ�pkt��S
			int len = XFFmpeg::Get()->ToPCM(out);//�ز�����Ƶ
			XAudioPlay::Get()->Write(out, len);//д����Ƶ
			continue;
		}
// 		XFFmpeg::Get()->Decode(&pkt);//������Ƶ֡
// 		av_packet_unref(&pkt); 
		videos.push_back(pkt);
	}


}
