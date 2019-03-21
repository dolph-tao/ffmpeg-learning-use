#pragma once
class XAudioPlay
{
public:
	static XAudioPlay *Get();//����ģʽ
	virtual bool Start()=0;//��ʼ
	virtual void  Play(bool isplay)=0;//�Ƿ񲥷�
	virtual bool Write(const char *data,int datasize) = 0;//����Ƶд��
	virtual void Stop()=0;//ֹͣ
	virtual int GetFree() = 0;//��ȡʣ��ռ�
	virtual ~XAudioPlay();
	int sampleRate = 48000;//������
	int sampleSize = 16;//������С
	int channel = 2;///ͨ����
protected:
	XAudioPlay();
};

