#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>

class VideoWidget:public QOpenGLWidget
{
public:
	VideoWidget(QWidget *parent = NULL);

	void paintEvent(QPaintEvent *event);//���ڵ����»���
	void timerEvent(QTimerEvent *event);//��ʱ��
	virtual ~VideoWidget();
};

