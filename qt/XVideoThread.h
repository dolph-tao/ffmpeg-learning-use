#pragma once
#include <QThread>
class XVideoThread:public QThread
{
public:
	static XVideoThread *Get()//创建单例模式
	{
		static XVideoThread vt;
		return &vt;
	}
	void run();
	XVideoThread();
	virtual ~XVideoThread();
};

