#include "mywidgetitem.h"
#include <QPainter>
#include <QSize>
#include <QListWidget>
MyWidgetItem::MyWidgetItem(const QString &text, QListWidget *parent) : QListWidgetItem(text, parent) {

}


void MyWidgetItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // 绘制Item的背景色及边框
    painter->save();
    painter->setBrush(QBrush(QColor(255, 255, 255)));
    painter->setPen(QPen(QColor(200, 200, 200), 1));
    painter->drawRect(option.rect);
    painter->restore();

    // 绘制Item的文本
    painter->save();
    QRect textRect = option.rect.adjusted(5, 5, -5, -5);
    QFont font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignCenter, text());
    painter->restore();
}
