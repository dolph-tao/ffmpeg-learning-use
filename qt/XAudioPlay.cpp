#include "XAudioPlay.h"
#include <QAudioOutput>
#include<QMutex>
class CXAudioPlay :public XAudioPlay
{
public:
	
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;
	QMutex mutex;
	void Stop()
	{
		mutex.lock();
		if (output)//Ϊ��AudioOutput
		{
			output->stop();
			delete output;
			output = NULL;
			io = NULL;
		}
		mutex.unlock();
	}

	//�������ò��ŵĸ�ʽ�Լ�����
	bool Start()
	{
		Stop();
		mutex.lock();
		QAudioOutput *out;//������Ƶ
		QAudioFormat fmt;//������Ƶ�����ʽ
		fmt.setSampleRate(48000);//1�����Ƶ������
		fmt.setSampleSize(16);//���������Ĵ�С
		fmt.setChannelCount(2);//����
		fmt.setCodec("audio/pcm");//�����ʽ
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);//������Ƶ����
		output = new QAudioOutput(fmt);
		io = output->start();//���ſ�ʼ
		mutex.unlock();
		return true;
	}
	
	void  Play(bool isplay)
	{
		mutex.lock();
		if (!output)
		{
			mutex.unlock();
			return;
		}

		if (isplay)
		{
			output->resume();//�ָ�����
		}
		else
		{
			output->suspend();//��ͣ����
		}
		mutex.unlock();
	}


	int GetFree()
	{
		mutex.lock();
		if (!output)
		{
			mutex.unlock();
			return 0;
		}
		int free = output->bytesFree();//ʣ��Ŀռ�

		mutex.unlock();

		return free;
	}

	bool Write(const char *data, int datasize)
	{
		mutex.lock();
		if (io)
		   io->write(data, datasize);//����ȡ����Ƶд�뵽��������
		mutex.unlock();
		return true;
	}
};

XAudioPlay::XAudioPlay()
{
}


XAudioPlay::~XAudioPlay()
{
}

XAudioPlay * XAudioPlay::Get()
{
	static CXAudioPlay ap;
	return &ap;

}
