#include "VideoWidget.h"
#include <QPainter>
#include "XFFmpeg.h"
#include "XVideoThread.h"
VideoWidget::VideoWidget(QWidget *parent) :QOpenGLWidget(parent)
{
	//XFFmpeg::Get()->Open("1080.mp4");//打开视频
	startTimer(20);//设置定时器
	XVideoThread::Get()->start();
}


VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent *e)
{//绘制
	static QImage *image = NULL;
	static int w = 0;
	static int h = 0;
	if (w != width() || h != height())//当缩小窗口或者方法窗口时，删除image,重新绘制
	{
		if (image)
		{
			delete image->bits();//删除内容
		    delete image;
			image = NULL;
		}
		

	}
	if (image == NULL)
	{
		uchar *buf = new uchar[width()*height() * 4];//存放解码后的视频空间
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
	}

	//将解码后的视频帧转化为RGB
	XFFmpeg::Get()->ToRGB((char *)image->bits(),width(),height());


	QPainter painter;
	painter.begin(this);//清理屏幕
	painter.drawImage(QPoint(0, 0), *image);//绘制FFMpeg解码后的视频
	painter.end();//


}

void VideoWidget::timerEvent(QTimerEvent *event)
{
	this->update();//定时器更新
}
