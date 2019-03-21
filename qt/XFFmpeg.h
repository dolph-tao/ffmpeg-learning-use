#pragma once
#include <iostream>
#include <QMutex>
extern "C"
{
	//����FFMpeg��ͷ�ļ�
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class XFFmpeg
{
public:
	static XFFmpeg *Get()//����ģʽ
	{
		static XFFmpeg ff;
		return &ff;
	}

   /*���ļ�������Ƶ�ļ�����ʱ��
     ��ʱ��С��0��ͬ��δ�򿪳ɹ�
   */
	int Open(const char *path);
	void Close();//�ر��ļ�

	//��ȡ��Ƶ��ÿһ֡������ÿ֡����Ҫ����ռ�
	AVPacket Read();

	//��ȡ��ÿ֡���ݺ���Ҫ������н���,��������pts
	int Decode(const AVPacket *pkt);

	//��������YUV��Ƶ֡ת��ΪRGB��ʽ
	bool ToRGB(char *out,int outwidth,int outheight);

	int ToPCM(char *out);//��Ƶ���ز���

	std::string GetError();//��ȡ������Ϣ
	virtual ~XFFmpeg();
	int totalMs = 0;//��ʱ��
	int videoStream = 0;//��Ƶ��
	int audioStream = 1;//��Ƶ��
	int fps = 0;//ÿ�����Ƶ֡��

	bool Seek(float pos);//���ý��������϶�λ�ú󲥷�

	bool isPlay = false;//������ͣ

	int pts;//��õ�ǰ����֡��ʱ��
	int GetPts(const AVPacket *pkt);//�����Ƶ����ǰ��Ƶ֡��pts

	int sampleRate = 48000;//������
	int sampleSize = 16;//������С
	int channel = 2;///ͨ����
protected:
	char errorbuff[1024];//��ʱ�����Ĵ�����Ϣ
	XFFmpeg();
	QMutex mutex;//������������߳�ʱ����ͬʱ��Ķ�д
	AVFormatContext *ic = NULL;//���װ������
	AVFrame *yuv = NULL;//��������Ƶ֡����
	AVFrame *pcm = NULL;//��������Ƶ����
	SwsContext  *cCtx = NULL;//��Ƶת��������
	SwrContext *aCtx = NULL;//��Ƶ�ز���������
};

