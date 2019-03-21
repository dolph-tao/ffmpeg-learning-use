#ifndef AGINEXPLAY_H
#define AGINEXPLAY_H

#include <QtWidgets/QWidget>
#include "ui_aginexplay.h"

class agineXplay : public QWidget
{
	Q_OBJECT

public:
	agineXplay(QWidget *parent = 0);
	~agineXplay();
	void timerEvent(QTimerEvent *event);//��ʱ�����������ȺͲ��Ž���
	void resizeEvent(QResizeEvent *event);//�ı䴰�ڴ�С


public slots:
	void open();//�ۺ���������Ӧ���ļ��İ�ť
	void sliderPressed();//���½�����ʱ		
	void sliderReleased();//�ɿ�������ʱ
	void play();//������Ƶ�ò��ź���ͣ

private:
	Ui::agineXplayClass ui;
};

#endif // AGINEXPLAY_H
