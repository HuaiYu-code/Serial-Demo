#ifndef SS_PUB_H
#define SS_PUB_H

#include <QtCore>
#include <qobject.h>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QTextCodec>
#include <QFont>
#include <QFontMetrics>
#include <QFontMetricsF>
#include <ShlObj.h>
#include <io.h>
#include <winerror.h>
#include <synchapi.h>

#include "Helper/hhthelper.h"

class PUBComClass :public QObject
{
	Q_OBJECT
public:
	PUBComClass();
	~PUBComClass();
	QString  Pub_GetUserDataPath();
	int Pub_CheckAppRunning(void);

signals:
	void signal_openWindowsApp(QString);
	void signal_SendallapptoAndroid811();
	void signal_TcpconnectSendallapp();
	void signal_TimerCheckdeleteapp();
	void signal_Timerstop();
	void signal_openProcFromFileName(QString);//打开APP信号-->连接至MainDialog
	void signal_openProcFromFileNameX5X7(QString);
	void SignalOpenComFailed();//连接COM失败信号---->连接至Maindialog
	void SignalOpenComSuccess();//打开串口成功信号
    void SignalConnectTcpSuccess();//连接TCP成功信号
	void SignalTotalPkgNum(int);//总包数信号---->连接值UploadWidget
	void SignalCurrentPkgNum(int);//当前发送包数--->连接至UploadWidget
	void SignalSendFileToAndroidFailed();//向Android发送文件失败--->连接至UploadWidget
	void SignalCameraStatusCheck();//摄像头状态检测信号------>连接至MainDialog
	void SignalPubUsbStatusCheck();
	void SignalClearAllApp();
	void SignalX9FirmwareCheck();//X9固件确认信号
	void SignalFileNameTooLong();
	void signalcloseUpload();
public:
	void openWindowsApp(QString fileName);
	void SendallapptoAndroid811();
	void TcpconnectSendallapp();
	void TimerCheckdeleteapp();
	void Timerstop();
	void PostOpenApp(QString strAppName);
	void PostOpenAppX5X7(QString strAppName);
	void PostOpenComFailed();
	void PostOpenComSuccess();
    void PostConnectTcpSuccess();
	void PostTotalPkgNum(int nTotalPkgNum);
	void PostCurrentPkgNum(int nCurrentPkgNum);
	void PostSendFileToAndroidFailed();
	void PostCameraStatusCheck();
	void PostPubUsbStatusCheck();
	void PostClearAllApp();
	void PostX9FirmwareCheck();
	void PostFileNameTooLong();
	void closeUpload();
};

extern PUBComClass  PUB_SS;

#endif // SS_PUB_H
