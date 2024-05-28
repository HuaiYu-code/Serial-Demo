#ifndef SYNCWIDGET_H
#define SYNCWIDGET_H
#include <QWidget>
#include <QLabel>

class SyncWidget : public QWidget
{
	Q_OBJECT
public:
    explicit SyncWidget(QWidget* parent = 0,QPixmap *pixmap = nullptr);

    void paintEvent(QPaintEvent* event) override;
public:
    QString *fileName;
    // 默认显示
    QPixmap *m_pixmap;
    QLabel *m_label1;
    QLabel *label2;
    QLabel *label3;


};
#endif
