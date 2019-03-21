#include "aginexplay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "XFFmpeg.h"
#include "XAudioPlay.h"
static bool isPressSlider;//是否按下进度条
static bool isPlay = true;//播放
#define  PAUSE "QPushButton\
{border-image: url\
(:/agineXplay/Resources/stop.jpg);}"//css语法暂停按钮
#define  PLAY "QPushButton\
{border-image: url\
(:/agineXplay/Resources/play.jpg);}"//播放按钮

agineXplay::agineXplay(QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	startTimer(40);

}

agineXplay::~agineXplay()
{

}

void agineXplay::open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));//打开视频文件
	if (name.isEmpty())
		return;
	
	this->setWindowTitle(name);//设置窗口的标题
	int totalMs = XFFmpeg::Get()->Open(name.toLocal8Bit());//获取视频总时间
	if (totalMs <= 0)//未打开成功
	{
		QMessageBox::information(this, "err", "file open failed!");//弹出错误窗口
		return;
	}
	XAudioPlay::Get()->sampleRate = XFFmpeg::Get()->sampleRate;
	XAudioPlay::Get()->channel = XFFmpeg::Get()->channel;
	XAudioPlay::Get()->sampleSize = 16;
	XAudioPlay::Get()->Start();
	char buf[1024] = { 0 };//用来存放总时间
	int min = (totalMs) / 60;
	int sec = (totalMs) % 60;
	sprintf(buf, "%03d:%02d", min, sec);//存入buf中
	ui.totaltime->setText(buf);//显示在界面中
	isPlay = false;
	play();
}

void agineXplay::timerEvent(QTimerEvent *event)
{
	int min = (XFFmpeg::Get()->pts) / 60;//视频播放当前的分钟	
	int sec = (XFFmpeg::Get()->pts) % 60;//视频播放当前的秒
	char buf[1024] = { 0 };
	sprintf(buf, "%03d:%02d /  ", min, sec);//存入buf中
	ui.playtime->setText(buf);//显示在界面中

	if (XFFmpeg::Get()->totalMs > 0)//判断视频得总时间
	{
		float rate = (float)XFFmpeg::Get()->pts / (float)XFFmpeg::Get()->totalMs;//当前播放的时间与视频总时间的比值
		if (!isPressSlider) //当松开时继续刷新进度条位置
			ui.playslider->setValue(rate * 1000);//设置当前进度条位置
	}

}

void agineXplay::sliderPressed()
{
	isPressSlider = true;
}

void agineXplay::sliderReleased()
{
	isPressSlider = false;
	float pos = 0;

	//松开时此时滑动条的位置与滑动条的总长度
	pos = (float)ui.playslider->value() / (float)(ui.playslider->maximum() + 1);
	XFFmpeg::Get()->Seek(pos);

}

void agineXplay::play()
{
	isPlay = !isPlay;//播放取反
	XFFmpeg::Get()->isPlay = isPlay;//将播放状态传递于XFFMpeg中的isPlay
	if (isPlay)//如果播放了
	{
		ui.playButton->setStyleSheet(PLAY);//显示播放按钮状态
	}
	else
	{
		ui.playButton->setStyleSheet(PAUSE);//显示暂停播放按钮状态
	}

}

void agineXplay::resizeEvent(QResizeEvent *event)
{
	ui.openGLWidget->resize(size());
	ui.playButton->move(this->width() / 2 + 50, this->height() - 80);
	ui.openButton->move(this->width() / 2 - 50, this->height() - 80);
	ui.playslider->move(25,this->height()-120);
	ui.playslider->resize(this->width()-50,ui.playslider->height());
	ui.playtime->move(25, ui.playButton->y());
	ui.totaltime->move(130,ui.playButton->y());

}
