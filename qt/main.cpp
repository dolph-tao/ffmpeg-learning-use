#include "aginexplay.h"
#include "XFFmpeg.h"
#include <QtWidgets/QApplication>

#include <QAudioOutput>
#include <QIODevice>
int main(int argc, char *argv[])
{
	QAudioOutput *out;//播放音频
	QAudioFormat fmt;//设置音频输出格式
	fmt.setSampleRate(48000);//1秒的音频采样率
	fmt.setSampleSize(16);//声音样本的大小
	fmt.setChannelCount(2);//声道
	fmt.setCodec("audio/pcm");//解码格式
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);//设置音频类型
	out = new QAudioOutput(fmt);
	QIODevice *io = out->start();
	//io->write();
	QApplication a(argc, argv);
	agineXplay w;
	w.show();
	return a.exec();
	


}
