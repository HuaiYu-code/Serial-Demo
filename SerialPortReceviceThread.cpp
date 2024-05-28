#include "SerialPortReceviceThread.h"



SerialPortReceviceThread::SerialPortReceviceThread(QObject* par)
{
     qDebug()<< "Thread01 construct fun" << QThread::currentThread();
}

void SerialPortReceviceThread::run()
{
    qDebug()<< "Thread01::run" << QThread::currentThread();
    while(1)
    {
        //qDebug() << index++;

        emit message("str");
        QThread::msleep(100);
    }

}
