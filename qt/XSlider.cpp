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
	double pos = (double)ev->pos().x() / (double)width();//��ǰ���λ�ñ���
	setValue(pos*this->maximum());//����λ��
	QSlider::mousePressEvent(ev);
}
