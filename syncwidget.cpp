#include "syncwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QIcon>
#include <QDebug>
#include <QHBoxLayout>
SyncWidget::SyncWidget(QWidget* parent,QPixmap *pixmap) : QWidget(parent)
{

    qDebug()<< __FUNCTION__<<__LINE__;
    m_pixmap = pixmap;
}


void SyncWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    // 绘制图标
    QPixmap iconPixmap(":/icon.png");  // 替换为你的图标
    painter.drawPixmap(0, 0, m_pixmap->width(), m_pixmap->height(), *m_pixmap);

    // 绘制文本
    painter.drawText(m_pixmap->width() + 10, painter.fontMetrics().height(), "Custom Widget");
}
