#ifndef ROLLINGLABLE_H
#define ROLLINGLABLE_H

#include <QEvent>
#include <QLabel>
#include <QPaintEvent>

class RollingLable : public QLabel
{
    Q_OBJECT
public:
    explicit RollingLable(QWidget *parent = nullptr);

signals:

public slots:

public:
    void upateLabelRollingState();
    void timerEvent(QTimerEvent *e);
    void paintEvent(QPaintEvent *e);
    void setText(const QString & txt);
    void resizeEvent(QResizeEvent *e);

    int fontSize;
    int left;
    int timerId;
    QString strSpace;
};

#endif // ROLLINGLABLE_H
