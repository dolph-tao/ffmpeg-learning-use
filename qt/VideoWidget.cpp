#include "VideoWidget.h"
#include <QPainter>
#include "XFFmpeg.h"
#include "XVideoThread.h"
VideoWidget::VideoWidget(QWidget *parent) :QOpenGLWidget(parent)
{
	//XFFmpeg::Get()->Open("1080.mp4");//����Ƶ
	startTimer(20);//���ö�ʱ��
	XVideoThread::Get()->start();
}


VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent *e)
{//����
	static QImage *image = NULL;
	static int w = 0;
	static int h = 0;
	if (w != width() || h != height())//����С���ڻ��߷�������ʱ��ɾ��image,���»���
	{
		if (image)
		{
			delete image->bits();//ɾ������
		    delete image;
			image = NULL;
		}
		

	}
	if (image == NULL)
	{
		uchar *buf = new uchar[width()*height() * 4];//��Ž�������Ƶ�ռ�
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
	}

	//����������Ƶ֡ת��ΪRGB
	XFFmpeg::Get()->ToRGB((char *)image->bits(),width(),height());


	QPainter painter;
	painter.begin(this);//������Ļ
	painter.drawImage(QPoint(0, 0), *image);//����FFMpeg��������Ƶ
	painter.end();//


}

void VideoWidget::timerEvent(QTimerEvent *event)
{
	this->update();//��ʱ������
}
