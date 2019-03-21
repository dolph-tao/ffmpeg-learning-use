#include "aginexplay.h"
#include "XFFmpeg.h"
#include <QtWidgets/QApplication>

#include <QAudioOutput>
#include <QIODevice>
int main(int argc, char *argv[])
{
	QAudioOutput *out;//������Ƶ
	QAudioFormat fmt;//������Ƶ�����ʽ
	fmt.setSampleRate(48000);//1�����Ƶ������
	fmt.setSampleSize(16);//���������Ĵ�С
	fmt.setChannelCount(2);//����
	fmt.setCodec("audio/pcm");//�����ʽ
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);//������Ƶ����
	out = new QAudioOutput(fmt);
	QIODevice *io = out->start();
	//io->write();
	QApplication a(argc, argv);
	agineXplay w;
	w.show();
	return a.exec();
	


}
