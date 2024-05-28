#ifndef MYWIDGETITEM_H
#define MYWIDGETITEM_H
#include <QListWidgetItem>
#include <QSize>
#include <QPainter>
#include <QListWidget>

class MyWidgetItem : public QListWidgetItem
{
public:
    explicit MyWidgetItem(const QString &text, QListWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // MYWIDGETITEM_H
