 #ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QGroupBox>
#include <QFile>
#include <QPainter>
#include <QPaintEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QUrl>
#include <QTimer>
#include <QBuffer>
#include <QSettings>
#include <QScrollBar>
#include <QFileIconProvider>
#include <QProcess>
#include <QSettings>
#include <QPaintEvent>
#include <QSystemTrayIcon>
#include <QByteArray>
#ifdef  Q_OS_WIN32
#include <QtWin>
#include <cstring>
//using namespace std;

#include <SetupAPI.h>
#include <InitGuid.h>
#include "TlHelp32.h"
#include "windows.h"


#pragma comment(lib, "Advapi32")
DEFINE_GUID(UsbClassGuid, 0xa5dcbf10L, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
#endif //Q_OS_WIN32

#include "nlistwidget.h"
#include "Helper/iconhelper.h"
#include "Gui/frmmessagebox.h"
#include "Gui/aboutdialog.h"
#include "Gui/hintdialog.h"
#include "Gui/uploadwidget.h"
#include "Gui/montagedialog.h"

#include "hhtheader.h"
#include "SerialPortReceviceThread.h"

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo> //20220706


#include <QListWidget>
#include <QListWidgetItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPixmap>

#define  SUPORT_X5X7_DEVICE  0
# if HHT_CHINA
#define  APPLICATION_NAME  "Newline assistant"
#else
#define  APPLICATION_NAME  "Newline assistant"
#endif
#define  MONTAGE  "Montage Receiver" //定义特殊第三方软件Montage
#define SHOWNORNAL   (WM_USER+0x0004)//自定义SendMessageA 的Msg参数
class NListWidget;
class HiteVisionAssistantService;

namespace Ui {
	class MainDialog;
}
QT_END_NAMESPACE

class MyThread : public QThread
{
    Q_OBJECT

public:
    virtual void run() override;
    MyThread(QObject *parent);
signals:
    void writeData_signal();
};


class MainDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MainDialog(QWidget *parent = 0);
	~MainDialog();
	QVector<HHTAPPINFO>g_appInfoVector;
    QVector<QString> syncSuccessAPP;
    enum DataType{
        File_List_Verification = 1,   //校验对应位置的文件是否与OPS端保持一致
        Operation_Result_Return = 2,  //返回发送的命令是否操作成功
        File_listClean = 3,           //清除列表
        File_listAdd = 4,             //增加一个应用
        File_listDelete = 5,          //删除一个应用
        File_listCheck = 6,           //校验应用编号
        File_listCheckSuccess = 7,    //校验成功
        File_listCheckFail = 8,       //校验失败
        open_OPS_APP = 9,             //打开opsAPP
        close_OPS_APP = 10,           //关闭opsApp
        UnknownBaud = -1
    };
	void init();
	void initWind();
	//用于记录程序关闭前设置的APP信息
	void WriteRecords();
	void ReadSettings();
	void ReadRecords();

    void reLoadAppInfo();
    void sendFileDate(QString filename,QIcon icon,int number,QString type);
    int  Packed_Data(QByteArray dataInArray,QByteArray *ArrayData);
    int  Analysis_of_Serial_Port_Protocol(QByteArray buffer, int Size,QByteArray *data_result);
    void ReceviceData();
    void onDataReceived(QByteArray buffer,int Size);
    void copyQByteArray(QByteArray *src,int srcPos,QByteArray *dest,int destPos,int len);
    void Parsedata(QString data);
    void onSuccess(QString type,int num);
    void setItemIcon(QListWidgetItem *item,QPixmap pixmap,bool isSync);

	//    //QIcon转数据
	QByteArray QIcon2QByteArray(QIcon icon);

    void WriteNewlineCastRegisterInfo(bool isRegister);

    void sendOPSInfo();

	
/*******************************************************************************/

#ifdef  Q_OS_WIN32
	bool nativeEvent(const QByteArray & eventType, void *message, long * lResult);
#endif //Q_OS_WIN32

	QPixmap getMaxPixmap(const QString sourceFile);
protected:
	void mousePressEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

	void dragEnterEvent(QDragEnterEvent *event);

	void dropEvent(QDropEvent *event);

public slots:

	void slot_openWindowsApp(QString fileName);

	void slot_TrashOpenCOMFailed();

	//TrashWIdget删除APP响应信号
	void slot_deleteAppFromVector(QString appName);

	void slot_deleteAppFromVectorFailed();


	void  trayiconActivated(QSystemTrayIcon::ActivationReason reason);

	void  slot_quitAction();
	void  slot_syncAction();

	void slot_showMainDialog();//连接至MontageDialog类

    void onFailed();


private slots:

    void on_checkButton_clicked();

	void on_trayButton_clicked();

	void on_aboutButton_clicked();

	void on_minButton_clicked();

	void on_syncButton_clicked();

	void on_closeButton_clicked();

    void on_listWidget_clicked(const QModelIndex &index);

    void on_listWidget_itemSelectionChanged();

private:
	Ui::MainDialog *ui;

    QByteArray DataBuffer;
    QByteArray Buffer_data;
    int WriteDatapost1=0;
    int ReadDatapost1=0;
    int data_head_detection=0;
    int data_head_num=0;
    int data_end_num=0;
    unsigned short data_len=0;
    int data_end=0;
    int Buffer_data_recevice_NUM = 0;
    QTimer *querryTimer;
    QPixmap m_pixmap;
    QString BaudRate= NULL;

	QWidget *m_widget;
	bool groupBoxFliter;
	QPoint windowPos;
	QPoint mousePos;
	QPoint dPos;
	AboutDialog *aboutDialog;
	MontageDialog *montageDialog;
	//Menu
	QString shortcutPath;
	UploadWidget *upload;
	QMenu *trayIconMenu;
	QSystemTrayIcon *trayIcon;
	QAction *quitAction, *syncAction, *clearAction;
	bool  recvivedFailedOpenCom;
	bool  isFailedOpenCOM;
	HiteVisionAssistantService *assistant;
	QTimer* updateTimer;
    bool isSuccess = false;
    bool isTimeOut = false;
    // add by chenpeinan 20220706
    QSerialPort *m_pSerialPort;
    QSerialPortInfo info_forOpen;

private slots:
    void on_Trash_signal_mousepressevent();
    void on_Trash_signal_mousereleaseevent();
};

#endif // MAINDIALOG_H
