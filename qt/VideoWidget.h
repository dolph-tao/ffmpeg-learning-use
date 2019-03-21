#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget:public QOpenGLWidget
{
public:
	VideoWidget(QWidget *parent = NULL);

	void paintEvent(QPaintEvent *event);//窗口的重新绘制
	void timerEvent(QTimerEvent *event);//定时器
	virtual ~VideoWidget();
};

