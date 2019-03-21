#pragma once
#include "qobject.h"
#include <QSlider>
class XSlider :
	public QSlider
{
	Q_OBJECT

public:
	XSlider(QWidget *parent);
	~XSlider();
	void mousePressEvent(QMouseEvent *ev);//鼠标按下事件
};

