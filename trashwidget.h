#ifndef TRASHWIDGET_H
#define TRASHWIDGET_H

#include <QWidget>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTimer>
#include "nlistwidget.h"
class TrashWIdget : public QWidget
{
    Q_OBJECT
public:
    explicit TrashWIdget(QWidget *parent = 0);
    void   deleteSingleAppCommandsToAndroid(QString appName);//删除操作，向Android写删除指令
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent* event);  //添加鼠标点击事件
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
signals:
    void signal_deleteAppFromVector(QString appName);//删除APP信号-->连接至MainDialog
	void signal_deleteAppFromVector811(QString appName);
    void signal_deleteAppFromVectorFailed();
    void signal_TrashOpenCOMFailed();
	void signal_mousepressevent();  //自定义点击信号，在mousePressEvent事件发生时触发
	void signal_mousereleaseevent();
public slots:
    void slot_timeout();
private:
     bool isTrydelete;
	 bool isdropin;
     QTimer *timer;
};

#endif // TRASHWIDGET_H
