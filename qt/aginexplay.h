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
	void timerEvent(QTimerEvent *event);//定时器滑动条进度和播放进度
	void resizeEvent(QResizeEvent *event);//改变窗口大小


public slots:
	void open();//槽函数用来响应打开文件的按钮
	void sliderPressed();//按下进度条时		
	void sliderReleased();//松开进度条时
	void play();//控制视频得播放和暂停

private:
	Ui::agineXplayClass ui;
};

#endif // AGINEXPLAY_H
