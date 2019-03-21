#include "XSlider.h"
#include <QMouseEvent>

XSlider::XSlider(QWidget *p /*= NULL*/) :QSlider(p)
{

}


XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent *ev)
{
	double pos = (double)ev->pos().x() / (double)width();//当前鼠标位置比率
	setValue(pos*this->maximum());//设置位置
	QSlider::mousePressEvent(ev);
}
