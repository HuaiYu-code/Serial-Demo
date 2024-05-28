#include "trashwidget.h"
#include "Helper/iconhelper.h"
#include "Pub/ss_pub.h"
#include "hhtheader.h"
#include "hhtheader.h"
#include "nlistwidget.h"
#include "hintdialog.h"
#include "Pub\global.h"
extern NListWidget      *g_listWidget;//全局的ListWidget
extern QWidget            *g_TrashWidget;
extern QLabel         *g_trashpicture;
extern QLabel        *g_trashtext;
extern int            g_nCameraCommandsFlag;//摄像头状态标识
extern int            g_nDeleteSingleAppFlag;//删除一个APP 发送指令标识
TrashWIdget::TrashWIdget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
	
}

void TrashWIdget::dragEnterEvent(QDragEnterEvent *event)
{
	isdropin = false;
    qDebug()<<"dialog drag enter";
	HHT_LOG(EN_INFO, "drag enter");
    event->acceptProposedAction();
    if(g_listWidget->currentItem())
    {
//        qDebug()<<"将要删除";
        isTrydelete = true;
		g_trashtext->setStyleSheet("background-color:transparent;font-size:22px;color:#ffffff;font - style:normal;font - stretch:normal;font - family:NotoSans;");
       	g_TrashWidget->setStyleSheet("background-color:#0097a7;border-radius:10px;");
		g_trashpicture->setStyleSheet("border-image:url(:/Resource/image811/delete-open_1.png);");
        timer->start(1000);
		qDebug() << "taimer start:";
		HHT_LOG(EN_INFO, "taimer start");

    }
}

void TrashWIdget::deleteSingleAppCommandsToAndroid(QString appName)
{
   //删除指令 +APP 名字
}

void TrashWIdget::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event);
    int  appCount = g_listWidget->count();
    qDebug()<<"APP_COUNT:"<<appCount;
	HHT_LOG(EN_INFO, "APP COUNT (%d):",appCount);
	isdropin = true;
    if(g_listWidget->currentItem())
    {
		if (g_listWidget->currentItem()->isSelected())
		{
            emit signal_deleteAppFromVector(g_listWidget->currentItem()->text());
		}
        //        //删除列表
        if(isTrydelete&&appCount > g_listWidget->count())
        {
			g_TrashWidget->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
			g_trashtext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style:normal;font - stretch:normal;font - family:NotoSans;");
			g_trashpicture->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
        }
    }
}

void TrashWIdget::slot_timeout()
{
	if (!isdropin)
	{
		isdropin = true;
		qDebug() << "solt_timeout:";
	//	g_TrashWidget->setStyleSheet("border-image: url(:/Resource/image811/delete.png);background-color:transparent ");
	}
}
void TrashWIdget::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);
	emit signal_mousepressevent();
}
void TrashWIdget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
	emit signal_mousereleaseevent();
}
void TrashWIdget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);
}
