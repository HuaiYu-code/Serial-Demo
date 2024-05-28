#include "nlistwidget.h"
#include "Helper/hhthelper.h"
#include <QPalette>
#define DISABLE_APP_EXCUTE  //禁止程序运行

NListWidget::NListWidget(QWidget *parent)
    :QListWidget(parent)
{
    this->setAcceptDrops(false);
    this->setResizeMode(QListView::Adjust);   // 设置大小模式-可调节
    this->setViewMode(QListView::IconMode);   // 设置显示模式
    this->setMovement(QListView::Static);     // 设置单元项不可被拖动
    this->setSpacing(25);
    this->setDragEnabled(true);
    this->setDragDropMode(QAbstractItemView::DragOnly);
    this->setDropIndicatorShown(true);
    this->startDrag(Qt::MoveAction);
    this->setAutoScroll(true);
//    this->setSelectionMode(SelectionMode::ContiguousSelection);//鼠标拖拉多选
    this->setFocusPolicy(Qt::NoFocus);
    connect(this,&NListWidget::itemDoubleClicked,
            this,&NListWidget::itemDoubleClickedSlot);
}

void NListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
    {
        int distance = (event->pos()-starPont).manhattanLength();
        if(distance>=QApplication::startDragDistance())
        {
            ;
        }
    }
    QListWidget::mouseMoveEvent(event);
}

void NListWidget::mousePressEvent(QMouseEvent *event)
{
//    qDebug()<<"mousePressEvent";
    if(event->button()==Qt::LeftButton&&
            this->currentItem())
    {
        starPont = event->pos();
    }
    QListWidget::mousePressEvent(event);

}

void NListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    Q_UNUSED(event);
//    qDebug()<<"dragEnterEvent";
//    event->acceptProposedAction();
}

void NListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    Q_UNUSED(event);
//    qDebug()<<"dragMoveEvent";
}

void NListWidget::dropEvent(QDropEvent *event)
{
    qDebug()<<__FUNCTION__<<__LINE__;
        Q_UNUSED(event);
}

void NListWidget::itemDoubleClickedSlot(QListWidgetItem *item)
{
    return;
    qDebug()<<__FUNCTION__<<__LINE__;
#ifdef  DISABLE_APP_EXCUTE//禁止程序运行
    if(!item)
    {
        ;
    }
    else
    {
        //执行
        LPCWSTR program = (LPCWSTR)item->toolTip().utf16();
        //            const  QString Access = "C:/Users/jhonconal/Desktop/office/Access 2013.lnk";
        //             LPCWSTR program = (LPCWSTR)Access.utf16();
        HINSTANCE hInstance;
        //使用ShellExecute
        //直接调用Windows api执行程序可以避免路径空格不识别从而执行程序失败
        //hInstance = ShellExecute(NULL,NULL,program,NULL,NULL,SW_MAXIMIZE);
        hInstance = ShellExecute(NULL,NULL,program,NULL,NULL,SW_NORMAL);
        //if(QDesktopServices::openUrl(item->toolTip())){//
        if(hInstance)
        {
            QStringList section = item->toolTip().split('/');
            QString Name = section.at(section.size()-1);
            qDebug()<<Name.split('.').at(0)<<"====>Execution successed.";
        }
        else
        {
            QMessageBox::information(this,"执行程序",tr("程序运行失败"));
        }
    }
#else
    Q_UNUSED(item);
#endif
}

//void NListWidget::paintEvent(QPaintEvent* event) {
//       qDebug() << __FUNCTION__<<__LINE__;
//        Q_UNUSED(event);
//        QPainter painter(viewport());
//            for (int i = 0; i < count(); ++i) {
//                QListWidgetItem *item = this->item(i);
//                if (currentRow() == i) {
//                    painter.setPen(Qt::NoPen);
//                    QPixmap pixmap(":/Resource/image811/itemselect.png");
//                    painter.drawPixmap(visualItemRect(item), pixmap);
//                } else {
//                      painter.setPen(Qt::NoPen);
//                    QPixmap pixmap(":/Resource/image811/itemdeslect.png");
//                    painter.drawPixmap(visualItemRect(item), pixmap);
//                }

//            }
//}


void NListWidget::setItemIcon(QListWidgetItem *item,QIcon icon){
    qDebug() << __FUNCTION__<<__LINE__;

    QWidget *widget1 = new QWidget();

    widget1->setStyleSheet("background-image: url(:/Resource/image811/itemselect.png);");

    // 设置QWidget的透明度为0
    widget1->setWindowOpacity(0);


//    widget1->setGeometry(0, 0, 100, 100); //定义widget1的位置和大小
    this->setItemWidget(item,widget1);
//    this->update();
}


