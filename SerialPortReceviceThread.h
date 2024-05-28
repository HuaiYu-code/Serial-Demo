#ifndef SERIALPORTRECEVIETHREAD_H
#define SERIALPORTRECEVIETHREAD_H



#include <QThread>
#include <iostream>
#include <QDebug>

class SerialPortReceviceThread : public QThread
{
    Q_OBJECT;
public:
    SerialPortReceviceThread(QObject* par);

    void run() override;

signals:
    void message(const QString &result);

};



#endif // SERIALPORTRECEVIETHREAD_H
