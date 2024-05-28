#include "assistant/src/hitevisionAssistantService.hpp"
#include "maindialog.h"
#include "ui_maindialog.h"
#include "nlistwidget.h"
#include "trashwidget.h"
#include <QMenuBar>
#include <QMenu>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkInterface>
#include <QIcon>
#include <QListView>
#include <QLabel>
#include "Helper/iconhelper.h"
#include "Pub/ss_pub.h"
#include "Helper/hhthelper.h"
#include "Helper/fontscaleratio.h"

#include "maskwidget.h"
#include "syncwidget.h"
#include "synchapi.h"
//#include <QTime>


#define  HHT_WIDTH  735
#define  HHT_HEIGHT 840
//#define  HHT_DEBUG
#define  HHT_SUPPORT_DIR //支持文件夹拖拽

NListWidget     *g_listWidget;//全局的ListWidgetextern
MainDialog      *g_mainDialog;
QWidget           *g_TrashWidget;
QLabel    *g_trashpicture;
QLabel    *g_trashtext;
QString               g_tryDeleteAppName;//尝试从列表删除的APP
char                    g_Version[100];
int                       g_PubUsbOpertationStatus = -1;//操作状态
extern double     g_fontPixelRatio;

int SUPORT_811_DEVICE =0;
extern INT32  get_curlang;
StartMenuItemPtr uwpitemadd;

/*******************************************************************************************/


void MainDialog::onFailed(){

    qDebug() << "====>失敗";
    HintDialog* hint = new HintDialog();
    HHT_LOG(EN_INFO, "operate failed");
#if  HHT_CHINA
//       hint->setMassage(tr("Network connecting,Please wait a minute."),-2);//鸿合助手向智能设备发送数据失败
    hint->setMassage(tr("Delete app and sync with smart system failed ."),-2);//鸿合助手向智能设备发送数据失败

#else
//       hint->setMassage(tr("Network connecting,Please wait a minute."),-2);
       hint->setMassage(tr("Delete app and sync with smart system failed ."),-2);//鸿合助手向智能设备发送数据失败
#endif
       hint->resize(hint->width() - 40, hint->height());
       hint->showParentCenter(this);
       hint->show();
}


//打开win端app
void MainDialog::slot_openWindowsApp(QString fileName)
{
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));//中文乱码
	HHT_LOG(EN_INFO, "---Android commands to open a app name: (%s)", fileName.toLocal8Bit().data());
	qDebug() << "fileName=" << fileName;
	if (fileName != NULL)
	{
		if (fileName == "Newline assistant" || fileName == QString::fromUtf8("Newline 助手"))
		{//Android want to open the "Newline assistant"
			HHT_LOG(EN_INFO, "Android try to open: (%s)", fileName.toLocal8Bit().data());

			this->trayiconActivated(QSystemTrayIcon::Trigger);//不打开自己，直接show出自己
			//       this->show();//不打开自己，直接show出自己
		}
		else
		{
			for (int i = 0; i < g_appInfoVector.count(); i++)
			{
				QFileInfo fileInfo(g_appInfoVector.at(i)._lnkPath);
				AppInfoPtr Link(std::make_shared<AppInfo>(fileInfo));
				StartMenuItemPtr item(std::make_shared<StartMenuItem>(Link));
				qDebug() << "fliename " << g_appInfoVector.at(i)._fileName;
				qDebug() << "displayName" << item->displayName();			
				if (item->displayName() == fileName || fileName == g_appInfoVector.at(i)._fileName)
				{	
					HHT_LOG(EN_INFO, "fliename: (%s)", g_appInfoVector.at(i)._fileName.toLocal8Bit().data());
				    HHT_LOG(EN_INFO, "displayName: (%s)", item->displayName().toLocal8Bit().data());
					LPCWSTR program = (LPCWSTR)g_appInfoVector.at(i)._lnkPath.utf16();
					HINSTANCE hInstance;
					if(_waccess(program, 0) == -1)
					{
					  HHT_LOG(EN_INFO, "lnkInfo.exists() =(%s)", g_appInfoVector.at(i)._filePath.toLocal8Bit().data());
					  qDebug() << "lnkInfo.exists() == false.";
					  program = (LPCWSTR)g_appInfoVector.at(i)._filePath.utf16();
					}				
						hInstance = ShellExecute(NULL, NULL, program, NULL, NULL, SW_NORMAL);
						if (hInstance != NULL)
						{
							HHT_LOG(EN_INFO, "Android try to open: (%s) success .", fileName.toLocal8Bit().data());

						}
						else
						{
							HintDialog* hint = new HintDialog();
#if HHT_CHINA
							hint->setMassage(tr("Excute %1 failed").arg(fileName), -2);
#else
						hint->setMassage(tr("Excute %1 failed . ").arg(fileName), -2);
#endif
                        QString filename = g_appInfoVector.at(i)._lnkPath;
                        sendFileDate( filename, g_appInfoVector.at(i)._appIcon,i,"response");
//                        querryTimer->start();
						hint->showParentCenter(this);
						this->showNormal();
						hint->show();
					}
				}
			}
		}
	}
	else
	{
		HintDialog* hint = new HintDialog();
#if HHT_CHINA
		hint->setMassage(tr("Receive empty commands from smart system . "), -2);
#else
		hint->setMassage(tr("Receive empty commands from smart system . "), -2);
#endif
		hint->showParentCenter(this);
		this->showNormal();
		hint->show();
	}
}

/**************************************************************************************************/

MainDialog::MainDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainDialog)
{
    ReadSettings();
    HHT_LOG(EN_INFO, "[System font scales ratios (%f)]", g_fontPixelRatio);
	ui->setupUi(this);

    m_pSerialPort = new QSerialPort(this);
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, infos)
    {
        qDebug() << info.portName();
        if (info.portName() == "COM1")
        {
            info_forOpen=info;
            qDebug() << info.portName() << "-------------";
            m_pSerialPort->setPort(info);
            if(BaudRate != NULL){
                qDebug()<< "set BaudRate "<<BaudRate;
                switch (BaudRate.toInt()) {
                    case 1200:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud1200);//设置串口波特率（9600)
                        break;
                    case 2400:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud2400);//设置串口波特率（9600)
                        break;
                    case 4800:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud4800);//设置串口波特率（9600)
                        break;
                    case 9600:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud9600);//设置串口波特率（9600)
                        break;
                    case 19200:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud19200);//设置串口波特率（9600)
                        break;
                    case 38400:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud38400);//设置串口波特率（9600)
                        break;
                    case 57600:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud57600);//设置串口波特率（9600)
                        break;
                    case 115200:
                          m_pSerialPort->setBaudRate(QSerialPort::Baud115200);//设置串口波特率（9600)
                        break;
                }
            }else{
                 m_pSerialPort->setBaudRate(QSerialPort::Baud115200);//设置串口波特率(115200)
            }
            m_pSerialPort->setDataBits(QSerialPort::Data8);   //设置数据位（8）
            m_pSerialPort->setParity(QSerialPort::NoParity);  //设置奇偶校检（无）
            m_pSerialPort->setStopBits(QSerialPort::OneStop); //设置停止位(一位)
            m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);//设置流控制（无）



            if (m_pSerialPort->open(QIODevice::ReadWrite))
            {
                qDebug() << "(311D2) ==>open com success";
            }
            else
            {
                qDebug() << "(311D2) ==>open com failed";
            }
        }
    }
    connect(m_pSerialPort, &QSerialPort::readyRead, this, &MainDialog::ReceviceData);

    querryTimer = new QTimer(this);
    querryTimer->setInterval(5000);
    querryTimer->setSingleShot(true);
    connect(querryTimer,SIGNAL(timeout()),this,SLOT(onFailed()));


//    //摄像头启用指令信号与槽
//    //PusbUSB切换
//    //清除APP列表
//    //X9固件确认信号与曹
	//垃圾桶类删除APP信号与槽
	connect(ui->Trash, SIGNAL(signal_deleteAppFromVector(QString)), this, SLOT(slot_deleteAppFromVector(QString)));

	connect(ui->Trash, SIGNAL(signal_deleteAppFromVectorFailed()), this, SLOT(slot_deleteAppFromVectorFailed()));
	connect(ui->Trash, SIGNAL(signal_TrashOpenCOMFailed()), this, SLOT(slot_TrashOpenCOMFailed()));
	//Montage弹窗
	montageDialog = new MontageDialog();
	connect(montageDialog, SIGNAL(signal_showMainDialog()), this, SLOT(slot_showMainDialog()));


    initWind();//包含 ReadRecords 读取已有APP列表

}

MainDialog::~MainDialog()
{
    if (m_pSerialPort)
    {
        delete m_pSerialPort;
        m_pSerialPort = NULL;
    }



	if (!g_appInfoVector.isEmpty())
	{
		WriteRecords();
		g_appInfoVector.clear();
	}
	delete ui;
}


void MainDialog::init()
{
	ui->Trash->setAcceptDrops(true);
	ui->listWidget->setAcceptDrops(true);
	ui->widget->setAcceptDrops(false);
	this->setAcceptDrops(true);
	this->recvivedFailedOpenCom = false;
	this->isFailedOpenCOM = false;
	g_listWidget = ui->listWidget;
	m_widget = ui->widget;
	g_TrashWidget = ui->Trash;
	g_trashpicture = ui->deleteicon;
	g_trashtext = ui->deletetext;
	upload = 0;
	ui->BottomWidget->setAttribute(Qt::WA_TranslucentBackground);
	ui->cetnterWidget->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint /*|Qt::WindowStaysOnTopHint*/);//窗口无边框，置顶
	setAttribute(Qt::WA_TranslucentBackground);
	ui->maxButton->setVisible(false);
	ui->minButton->setVisible(false);

    if(g_fontPixelRatio>=3){
        ui->checkButton->setStyleSheet("font-size:20px;font-color:black;background-color:#0097a7;border-radius:10px;background-image:url(:/Resource/image811/sync.png);");
    }
	FontScaleRatio::Instance()->setGuiFont("Helvetica", 12, ui->label);
	
	m_widget->installEventFilter(this);//groupBox过滤事件
	//ui->label->setText("HiteVision assistant");
	ui->widget->setStyleSheet("border-style:dashed;\
                              border-color:#373737;\
                              border: solid 2px #373737;\
                              background-color: #2a2929;\
                              border-top-left-radius:10px;\
		                      border-top-right-radius:10px;\
                              ");
#if HHT_CHINA
	ui->label->setText(tr("  Newline  Assistant"));
	ui->label->setStyleSheet("color:#ffffff; font - style:normal;font - stretch:normal;font - family:NotoSans;");
#else
	ui->label->setText("Newline assistant");
#endif
//    ui->syncicon->setStyleSheet("border-image:url(:/Resource/image811/sync_1.png);");
//    ui->Sync->setStyleSheet("background-color:#0097a7;border-radius:10px;");
//    ui->synctext->setText(tr("Sync"));
//    ui->synctext->setStyleSheet("font-size:22px;color:#ffffff; font - style:normal; font - stretch:normal; font - family:NotoSans;");
	

	ui->label3->setText(tr("  Drag the shortcut here to add favorite."));
	ui->label3->setStyleSheet("font-size:22px;color:#ffffff; font - style:normal; font - stretch:normal; font - family:NotoSans;");
   

	ui->Trash->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
	ui->deletetext->setText(tr("Delete"));
	ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style: normal;font - stretch: normal;font - family: NotoSans;");
	ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
	switch (get_curlang)
	{
	case 1:
//		ui->Sync->setFixedWidth(126);
		ui->Trash->setFixedWidth(126);
		break;
	case 2:
//		ui->Sync->setFixedWidth(160);
		ui->Trash->setFixedWidth(160);
		break;
	case 3:
//		ui->Sync->setFixedWidth(160);
		ui->Trash->setFixedWidth(160);
		break;
	case 4:
//		ui->Sync->setFixedWidth(210);
		ui->Trash->setFixedWidth(210);
		break;
	case 5:
//		ui->Sync->setFixedWidth(150);
		ui->Trash->setFixedWidth(150);
		break;
	case 6:
//		ui->Sync->setFixedWidth(190);
		ui->Trash->setFixedWidth(190);
		break;
	case 7:
//		ui->Sync->setFixedWidth(170);
		ui->Trash->setFixedWidth(170);
		break;
	case 8:
//		ui->Sync->setFixedWidth(240);
		ui->Trash->setFixedWidth(240);
		break;
	case 9:
//		ui->Sync->setFixedWidth(180);
		ui->Trash->setFixedWidth(180);
		break;
	case 10:
//		ui->Sync->setFixedWidth(180);
		ui->Trash->setFixedWidth(180);
		break;
	default:
		break;
	}
	
	//listwidget 样式
	QScrollBar *verticalScrollBar = new QScrollBar(this);
	verticalScrollBar->setStyleSheet("QScrollBar:vertical {"
		"background-color:transparent; "
		// "border-image: url(:/Resource/images/vertical.png);"
		" width: 10px;"
		" margin: 0px 0 0px 0;"
		"border-radius: 3px;"
		"border: none;"
		"}"
		"QScrollBar::handle:vertical {"
		"background: #373737;"
		"min-height: 70px;"
		"min-width:6px;"
		"margin: 2 px 0 px 2px 2px;"
		"border-radius:3px;"
		"border: solid;"
		"}"
		"QScrollBar::add-line:vertical {"
		"height: 0px;"
		"subcontrol-position: bottom;"
		"subcontrol-origin: margin;"
		"}"
		" QScrollBar::sub-line:vertical {"
		"height: 1px;"
		"subcontrol-position: top;"
		"subcontrol-origin: margin;"
		"}"
		"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
		" border: 0px solid blue;"
		"width: 3px;"
		"height: 0px;"
		"}"
		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
		"background: none;"
		"}"
	);
	ui->listWidget->setVerticalScrollBar(verticalScrollBar);
	ui->listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
	if (g_fontPixelRatio >= 3)//֧支持300%缩放比例
	{
        ui->listWidget->setStyleSheet("QListWidget{padding-left:30px;padding-top:25px; padding-bottom:25px; "
            "padding-right:5px;border: solid 1.7px #373737;background-color: #121212; border-color:#373737;border-bottom-left-radius:10px;border-bottom-right-radius:10px;}"
            "QListWidget::Item{padding-top:0px; padding-bottom:10px; border:none; border-style:solid;border-radius:10px;border-image:url(:/Resource/image811/default.png);}"
            //"QListWidget::Item:hover{background:skyblue; }"
            "QListWidget::item:selected{color:white; border:none; border-style:solid;border-image:url(:/Resource/image811/selected.png);}"
            "QListWidget::item:selected:!active{border:none;background-color:transparent; }"
        );
	}
	else
	{
        ui->listWidget->setStyleSheet("QListWidget{padding-left:28px;padding-top:25px; padding-bottom:25px; "
           "padding-right:10px;border: solid 1.7px #373737;background-color: #121212;border-color:#373737;border-bottom-left-radius:10px;border-bottom-right-radius:10px;}"
            "QListWidget::Item{padding-top:0px; padding-bottom:10px; border:none; border-style:solid;border-radius:10px;border-image:url(:/Resource/image811/default.png);}"
        //	"QListWidget::Item:hover{background:skyblue; }"
            "QListWidget::item:selected{color:white;border:none; border-style:solid;border-image:url(:/Resource/image811/selected.png);}"
            "QListWidget::item:selected:!active{border:none;background-color:transparent; }"
        );
	}
	ui->Trash->setVisible(true);
    ui->syncButton->setVisible(false);
	if (SUPORT_811_DEVICE)
	{
		ui->closeButton->setVisible(false);//隐藏Close按钮
	    ui->checkButton->setVisible(false);//显示Check/upload按钮
	}
	else
	{
#ifdef HHT_2ND_PROJECT_SUPPORT

        ui->closeButton->setVisible(false);//隐藏Close按钮
        ui->checkButton->setVisible(true);//显示Check/upload按钮

#else
		ui->closeButton->setVisible(false);
#endif
	}
	
	//--------------------------------------------------托盘相关--------------------------------------------------
#if  HHT_CHINA
	QIcon icon = QIcon(":/Resource/image811/newline.png");
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(icon);
	trayIcon->setToolTip(tr("Newline assistant"));
	//trayIcon->setToolTip("HiteVision assistant");
#else
	QIcon icon = QIcon(":/Resource/images/logo.png");
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(icon);
	trayIcon->setToolTip("Newline assistant");
#endif
	trayIcon->show();
	//添加单/双击鼠标相应
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(trayiconActivated(QSystemTrayIcon::ActivationReason)));

	//创建右键弹出菜单
	trayIconMenu = new QMenu(this);
	trayIconMenu->setStyleSheet("QMenu {background-color: white;border: 1px solid white;}"
		"QMenu::item {background-color: transparent;padding:12px 56px;"
		"margin:0px 4px;border-bottom:1px solid #DBDBDB;}"
		"QMenu::item:selected { background-color: #2dabf9;}");
	FontScaleRatio::Instance()->setGuiFont("Helvetica", 12, trayIconMenu);
# if HHT_CHINA
	//quitAction = new QAction(tr("退出"), this);
	quitAction = new QAction(tr("Exit"), this);
	//quitAction->setIcon(QIcon(":/Resource/icons/Exit_25px.png"));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(slot_quitAction()));

	syncAction = new QAction(tr("Manual sync"), this);
	//syncAction->setIcon(QIcon(":/Resource/icons/Synchronize_25px.png"));
	connect(syncAction, SIGNAL(triggered()), this, SLOT(slot_syncAction()));

#else
	quitAction = new QAction(tr("Exit"), this);
	//quitAction->setIcon(QIcon(":/Resource/icons/Exit_25px.png"));
	connect(quitAction, SIGNAL(triggered()), this, SLOT(slot_quitAction()));

	syncAction = new QAction(tr("Manual sync"), this);
	//syncAction->setIcon(QIcon(":/Resource/icons/Synchronize_25px.png"));
	connect(syncAction, SIGNAL(triggered()), this, SLOT(slot_syncAction()));


#endif
	if (SUPORT_811_DEVICE)
	{
		ui->checkButton->setVisible(false);//显示Check/upload按钮
		ui->closeButton->setVisible(false);//隐藏Close按钮
		trayIconMenu->addAction(syncAction);//显示sync Action
		trayIconMenu->removeAction(quitAction);//先删除再显示
		trayIconMenu->addAction(quitAction);//显示quit Action
		trayIcon->setContextMenu(trayIconMenu);
	}
	else
	{

        trayIconMenu->addAction(quitAction);
        trayIcon->setContextMenu(trayIconMenu);
	}
	

	//--------------------------------------------------------------------------------------------------------------
	this->setWindowTitle(APPLICATION_NAME);//设置程序窗体标题,保障main.cpp中单利运行
	if (g_fontPixelRatio >= 3)
	{
		this->setFixedSize(QSize(HHT_WIDTH + 140, HHT_HEIGHT + 160));
	}
	else
	{
		this->setFixedSize(QSize(HHT_WIDTH, HHT_HEIGHT));
	}

}

void MainDialog::initWind()
{
	init();
	ReadRecords();
    sendOPSInfo();
    WriteNewlineCastRegisterInfo(false);
	//设置屏蔽罩
	MaskWidget::Instance()->setMainWidget(this);
	QStringList dialogNames;
	dialogNames << "UploadWidget";
	MaskWidget::Instance()->setDialogNames(dialogNames);
}

void MainDialog::WriteRecords()
{
	//    QString settingsPath = "Settings/Records.txt";
	QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/Records.txt";
	qDebug() << "WriteRecords()------>" << settingsPath;
	QFile file(settingsPath);
	if (!file.exists())
	{
		file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
		file.close();
		HHT_LOG(EN_INFO, "+++++ \"Record.txt\" not exist.");
	}
	else
	{
		bool result = file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
		if (result)
		{
			QTextCodec *code = QTextCodec::codecForName("UTF-8");
			QTextStream out(&file);
            out.setCodec(code);
			for (int i = 0; i < g_appInfoVector.count(); i++)
			{
				QString appinfo = "APP";
				appinfo.append(tr("[%1]=").arg(QString::number(i, 10)));
				appinfo.append(tr("%1#").arg(g_appInfoVector.at(i)._fileName));
				appinfo.append(tr("%1#").arg(g_appInfoVector.at(i)._lnkPath));
				appinfo.append(tr("%1#").arg(g_appInfoVector.at(i)._filePath));
				out << appinfo << endl;
				qDebug() << "FileName[" << i << "] :" << g_appInfoVector.at(i)._fileName;
				qDebug() << "FilePath[" << i << "] :" << g_appInfoVector.at(i)._lnkPath;
				qDebug() << "FilePath[" << i << "] :" << g_appInfoVector.at(i)._filePath;
				out.flush();
			}
            _commit(file.handle());
            qDebug() << "_commit(file.handle());";
            file.close();
		}
		else
		{
			HHT_LOG(EN_ERR, "+++++open \"Record.txt\" failed.");
			file.close();
		}
	}
}

void MainDialog::ReadSettings()
{
    QString settingsPath = QCoreApplication::applicationDirPath() + "/Settings/Settings.ini";
    qDebug()<< __FUNCTION__<<__LINE__<<QCoreApplication::applicationDirPath()<<settingsPath;
	QFile file(settingsPath);
	if (!file.exists())
	{
		return;
	}
    qDebug()<< __FUNCTION__<<__LINE__<<settingsPath;
	QSettings settings(settingsPath, QSettings::IniFormat);
	QString color = settings.value("/SETTINGS/theme").toString();
    BaudRate = settings.value("/SETTINGS/BaudRate").toString();
        qDebug()<<"获取到设置颜色:"<<color;
        qDebug()<<"获取到波特率:"<<BaudRate;
	this->setStyleSheet(tr("background-color:%1").arg(color));
	//读取配置文件
}

void MainDialog::reLoadAppInfo()
{
    QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/Records.txt";
    qDebug()<< __FUNCTION__<<__LINE__<<settingsPath;
    QFile file(settingsPath);
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        file.close();
        return;
    }
    int iconSize = 64;
    bool result = file.open(QIODevice::ReadOnly);
    if (result)
    {
        QTextCodec *code = QTextCodec::codecForName("UTF-8");
        QTextStream in(&file);
        in.setCodec(code);
        int i =0;
        while (!in.atEnd())
        {
            QString lineStr = in.readLine();
            if (!lineStr.contains("APP"))
            {
                ;
            }
            else
            {
                HHTAPPINFO hhtAppInfo;
                QStringList sections = lineStr.split(QRegExp("[=,#]"));
                QString fileName = sections.at(1);//获取名字
                QString lnkPath = sections.at(2);//获取路径
                QString filepath = sections.at(3);
                hhtAppInfo._fileName = fileName;
                hhtAppInfo._lnkPath = lnkPath;
                hhtAppInfo._filePath = filepath;
                QFileInfo fileInfo(lnkPath);
                QPixmap pixmap;
#ifdef HHT_SUPPORT_DIR
                if (fileInfo.isDir())
                {
                    QFileIconProvider icon_provider;
                    hhtAppInfo._appIcon = icon_provider.icon(fileInfo);
                }
#endif
                if (!fileInfo.isDir())
                {
                    QFileInfo file_info(lnkPath);//lnk or exe
                    if (file_info.filePath().contains(".lnk"))
                    {
                        if (file_info.symLinkTarget().isEmpty())
                        {
                            //UWP 快捷方式支持 2017/8/22
                            QByteArray uwpiconbyte = assistant->Getuwpicon(file_info.completeBaseName());
                            QIcon icon;
                            if (!uwpiconbyte.isEmpty())
                            {
                                QPixmap pix;
                                pix.loadFromData(uwpiconbyte);
                                icon = QIcon(pix);
                                hhtAppInfo._appIcon = icon;
                            }
                            else
                            {
                                QFileIconProvider icon_provider;
                                icon = icon_provider.icon(file_info);
                                QPixmap pixmap = icon.pixmap(QSize(100, 100));
                                hhtAppInfo._appIcon = QIcon(pixmap);
                            }

                            //==============================
                        }
                        else
                        {
                            pixmap = getMaxPixmap(file_info.symLinkTarget());
                            hhtAppInfo._appIcon = QIcon(getMaxPixmap(file_info.symLinkTarget()));
                        }
                    }
                    else
                    {//exe
                        pixmap = getMaxPixmap(file_info.filePath());
                        hhtAppInfo._appIcon = QIcon(getMaxPixmap(file_info.filePath()));
                    }
                }
                if(m_pSerialPort->isOpen()){
                    querryTimer->start();
                    QFileInfo file_info(hhtAppInfo._lnkPath);
                    QFileIconProvider icon_provider;
                    QIcon m_icon = icon_provider.icon(file_info);
                    m_pixmap = m_icon.pixmap(iconSize,iconSize);
                    sendFileDate(hhtAppInfo._lnkPath,hhtAppInfo._appIcon,i,"add");
                    i = i+1;
                }

            }
            in.flush();
        }
        if(!m_pSerialPort->isOpen()){
                    HintDialog* hint = new HintDialog();
                    //         hint->showParentCenter(this);
                    hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
                        (this->pos().y() + this->height() / 2) - (hint->height() / 2),
                        hint->width(), hint->height());
#if HHT_CHINA
                        hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
                        hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
                    hint->show();
        }
        file.close();
    }
    else
    {
        file.close();
    }
}


void MainDialog::ReadRecords()
{
    QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/Records.txt";
    qDebug()<< __FUNCTION__<<__LINE__<<settingsPath;
	QFile file(settingsPath);
	if (!file.exists())
	{
		file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
		file.close();
		return;
	}
    qDebug()<< __FUNCTION__<<__LINE__;
    int iconSize = 64;
	bool result = file.open(QIODevice::ReadOnly);
	if (result)
	{
        qDebug()<< __FUNCTION__<<__LINE__;
		QTextCodec *code = QTextCodec::codecForName("UTF-8");
		QTextStream in(&file);
		in.setCodec(code);
        int i =0;
        qDebug()<< __FUNCTION__<<__LINE__;
		while (!in.atEnd())
		{
			QString lineStr = in.readLine();
			if (!lineStr.contains("APP"))
			{
				;
			}
			else
			{
                qDebug()<< __FUNCTION__<<__LINE__;
				HHTAPPINFO hhtAppInfo;
				QStringList sections = lineStr.split(QRegExp("[=,#]"));
				QString fileName = sections.at(1);//获取名字
				QString lnkPath = sections.at(2);//获取路径
				QString filepath = sections.at(3);
				hhtAppInfo._fileName = fileName;
				hhtAppInfo._lnkPath = lnkPath;
				hhtAppInfo._filePath = filepath;
				QFileInfo fileInfo(lnkPath);
                QPixmap pixmap;
                qDebug()<< __FUNCTION__<<__LINE__;

#ifdef HHT_SUPPORT_DIR
				if (fileInfo.isDir())
				{
					QFileIconProvider icon_provider;
					hhtAppInfo._appIcon = icon_provider.icon(fileInfo);
				}
#endif
				if (!fileInfo.isDir())
				{
					QFileInfo file_info(lnkPath);//lnk or exe
					if (file_info.filePath().contains(".lnk"))
					{
						if (file_info.symLinkTarget().isEmpty())
						{
							//UWP 快捷方式支持 2017/8/22
//                            QByteArray uwpiconbyte = assistant->Getuwpicon(file_info.completeBaseName());
							QIcon icon;
//                            if (!uwpiconbyte.isEmpty())
//                            {
//                                QPixmap pix;
//                                pix.loadFromData(uwpiconbyte);
//                                icon = QIcon(pix);
//                                hhtAppInfo._appIcon = icon;
//                            }
//                            else
//                            {
								QFileIconProvider icon_provider;
								icon = icon_provider.icon(file_info);
								QPixmap pixmap = icon.pixmap(QSize(100, 100));
							    hhtAppInfo._appIcon = QIcon(pixmap);
//                            }
							
							//==============================
						}
						else
						{
                            pixmap = getMaxPixmap(file_info.symLinkTarget());
							hhtAppInfo._appIcon = QIcon(getMaxPixmap(file_info.symLinkTarget()));
						}
					}
					else
					{//exe
                        pixmap = getMaxPixmap(file_info.filePath());
						hhtAppInfo._appIcon = QIcon(getMaxPixmap(file_info.filePath()));
					}
				}
				g_appInfoVector.append(hhtAppInfo);
				//==============加载APP======================
                QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
				QFont font;
				font.setFamily("Helvetica");
                qDebug()<< __FUNCTION__<<__LINE__;

				if (g_fontPixelRatio >= 3)
				{
                    qDebug()<< "比例爲150"<< 111111;
					item->setSizeHint(QSize(150, 150));
                    iconSize = 96;
				}
				else
				{
                     qDebug()<< "比例爲120"<< 1111;
					item->setSizeHint(QSize(120, 120));
                    iconSize = 64;
				}

				font.setPointSize(qRound(11 / g_fontPixelRatio));
				item->setFont(font);
				item->setTextColor(Qt::white);
                item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
				//----------------------------------------------------------------------------------
//                item->setIcon(hhtAppInfo._appIcon);
                item->setText(hhtAppInfo._fileName);
				item->setToolTip(hhtAppInfo._lnkPath);
                if(m_pSerialPort->isOpen()){
                    querryTimer->start();
                    QFileInfo file_info(g_appInfoVector.at(i)._lnkPath);
                    QFileIconProvider icon_provider;
                    QIcon m_icon = icon_provider.icon(file_info);
                    m_pixmap = m_icon.pixmap(iconSize,iconSize);
                    sendFileDate(hhtAppInfo._lnkPath,hhtAppInfo._appIcon,i,"add");
                    i = i+1;
                }
                setItemIcon(item,pixmap,false);
			}
			in.flush();
		}
        if(!m_pSerialPort->isOpen()){
                    HintDialog* hint = new HintDialog();
                    //         hint->showParentCenter(this);
                    hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
                        (this->pos().y() + this->height() / 2) - (hint->height() / 2),
                        hint->width(), hint->height());
#if HHT_CHINA
                        hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
                        hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
                    hint->show();
        }
		file.close();
	}
	else
	{
		file.close();
	}
}

QByteArray MainDialog::QIcon2QByteArray(QIcon icon)
{
	if (icon.isNull())
		return NULL;
	QList<QSize>sizes = icon.availableSizes();
	int maxinum = sizes[0].width();
	for (int i = 1; i < sizes.size(); ++i)
	{
		maxinum = qMax(maxinum, sizes[i].width());
	}
	QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(maxinum, maxinum)));//QIcon 转QPixmap
	//    QPixmap pixmap = icon.pixmap(QSize(32,32));//QIcon 转QPixmap
	QByteArray byteArray, hexByteArray;
	//    //方法一：
	//    QDataStream ds(&byteArray,QIODevice::WriteOnly);
	//    ds<<pixmap;
	//方法二:
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	pixmap.save(&buffer, "PNG", 20);
	//对数据进行压缩，读数据需要解压缩,下位机没有对应函数解压不了
	hexByteArray = qCompress(byteArray, 1).toHex(); // better just open file with QFile, load data, compress and toHex?
	return byteArray/*.toHex()*/;
}

bool MainDialog::nativeEvent(const QByteArray &eventType, void *message, long *lResult)
{
	Q_UNUSED(eventType);
	Q_UNUSED(lResult);
	MSG *pMsg = reinterpret_cast<MSG*>(message);
	if (NULL == pMsg)
	{
		qDebug() << "Msg is null";
	}
	if (pMsg->message == SHOWNORNAL)
	{//自定义Msg
		qDebug() << "Msg: WM_SHOWNORNAL";
#if 1
		this->trayiconActivated(QSystemTrayIcon::Trigger);
#else
		this->resize(HHT_WIDTH, HHT_HEIGHT);
		this->showNormal();
#endif
		return true;
	}
	return false;
}

QPixmap MainDialog::getMaxPixmap(const QString sourceFile)
{
	QPixmap max;
	// ExtractIconEx 从限定的可执行文件、动态链接库（DLL）、或者图标文件中生成图标句柄数组
	const UINT iconCount = ExtractIconEx((wchar_t *)sourceFile.utf16(), -1, 0, 0, 0);
	//    qDebug()<<"Max icon count: "<<iconCount;
	if (!iconCount)
	{
		//没有图标exe文件是加载自定义默认icon
		max = QPixmap(":/Resource/icons/default.png");
		return max;
	}
	QScopedArrayPointer<HICON> icons(new HICON[iconCount]);
	ExtractIconEx((wchar_t *)sourceFile.utf16(), 0, icons.data(), 0, iconCount);//最大HICON
	max = QtWin::fromHICON(icons[0]);//获取第一个所谓最佳
	return max;
}

void MainDialog::mousePressEvent(QMouseEvent *event)
{
	this->windowPos = this->pos();
	this->mousePos = event->globalPos();
	this->dPos = mousePos - windowPos;
}

void MainDialog::mouseMoveEvent(QMouseEvent *event)
{
	this->move(event->globalPos() - this->dPos);
	if (upload != 0)
	{
		upload->showParentCenter(this);//上传界面一直停留再MainDialog中心位置
	}
}

void MainDialog::dragEnterEvent(QDragEnterEvent *event)
{
	HHT_LOG(EN_INFO, "==>dragEnterEvent");
	event->acceptProposedAction();
}

void MainDialog::dropEvent(QDropEvent *event)
{
    HHT_LOG(EN_INFO, "==>dragEvent");
    // [[3]]: 当放操作发生后, 取得拖放的数据
    const QMimeData* mimdata = event->mimeData();
    QList<QUrl>urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    QString path = urls.first().toLocalFile();
    HHT_LOG(EN_INFO, "dropEvent [Path]:(%s)", path.toLocal8Bit().data());
    qDebug() << "[Path]:" << path;
    int i = 0;
    if (!path.isEmpty())
    {
        foreach(QUrl url, urls)
        {
            i = i+1;
            QString fileName = url.toLocalFile();//文件名
            QString recodeAbsPath;//记录APP绝对路径
            QString recodeFileName;//记录APP名字
            QString recodefilepath;//记录app安装路径
            QIcon    recodeFileIcon;//记录APP图标
            HHTAPPINFO hhtAppInfo;//HHT app信息
            QFileInfo file_info(fileName);

            qDebug()<<__FUNCTION__<<__LINE__<<fileName;
            QFileIconProvider icon_provider;
            QIcon m_icon = icon_provider.icon(file_info);
            m_pixmap = m_icon.pixmap(48,48);
            int iconSize = 64;
            if (file_info.isDir())//文件为目录
            {
#ifdef HHT_SUPPORT_DIR
                qDebug() << "The drop content is dir";
                QIcon icon;
                QListWidgetItem *item = new QListWidgetItem();
                QFont font;
                QPixmap setIconPixmap;
                font.setFamily("Helvetica");
                font.setPointSize(qRound(11 / g_fontPixelRatio));
                item->setFont(font);

                if (g_fontPixelRatio >= 3)
                {
                    item->setSizeHint(QSize(150, 150));
                    iconSize = 96;
                }
                else
                {
                    item->setSizeHint(QSize(120, 120));
                    iconSize = 64;
                }
                item->setTextColor(Qt::white);
                item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
                if ((file_info.fileName().contains(".lnk")) || (file_info.fileName().contains(".LNK")))
                {
                        QFileInfo fileInfo(file_info.symLinkTarget());
                            QFileIconProvider icon_provider;
                            icon = icon_provider.icon(fileInfo);
                            recodeFileIcon = icon;//图标

                            setIconPixmap = icon.pixmap(QSize(64, 64));
                            item->setText(fileInfo.fileName());
                            recodeFileName = fileInfo.fileName();//名字
                            recodeAbsPath = fileInfo.filePath();//dir绝对路径
                            item->setToolTip(recodeAbsPath);
                            if (recodeFileName == NULL)
                            {
                                recodeFileName = file_info.completeBaseName();
                                item->setText(file_info.completeBaseName());
                                recodeAbsPath = file_info.filePath();//lnkpath
                                item->setToolTip(file_info.filePath());
                            }
                            recodefilepath = "dir";
                }
                else
                {
                    QFileIconProvider icon_provider;
                    icon = icon_provider.icon(file_info);
                    recodeFileIcon = icon;//图标
                    setIconPixmap = icon.pixmap(QSize(iconSize, iconSize));

                    item->setText(file_info.fileName());
                    recodeFileName = file_info.fileName();//名字
                    recodeAbsPath = file_info.filePath();//dir绝对路径
                    item->setToolTip(recodeAbsPath);
                    recodefilepath = "dir";
                }
//----------------------------------------------------------------------------------------------------------------------
                if (ui->listWidget->count() == 0)
                {
                    hhtAppInfo._fileName = recodeFileName;
                    hhtAppInfo._lnkPath = recodeAbsPath;
                    hhtAppInfo._appIcon = recodeFileIcon;
                    hhtAppInfo._filePath = recodefilepath;

                    HHT_LOG(EN_INFO, "dir count 0 811 add app name:(%s)", hhtAppInfo._fileName.toLocal8Bit().data());
                    QString filename = hhtAppInfo._lnkPath;
                    sendFileDate(filename ,hhtAppInfo._appIcon,g_appInfoVector.size()+1,"add");
                    if(m_pSerialPort->isOpen()){
                        querryTimer->start();
                    }
                    setItemIcon(item,setIconPixmap,false);

                    g_appInfoVector.append(hhtAppInfo);
                    WriteRecords();
                }
                else
                {
                    bool isExist = false;
                    for (int i = 0; i < ui->listWidget->count(); i++)
                    {
                        if (ui->listWidget->item(i)->toolTip() == recodeAbsPath)
                        {
                            isExist = true;
                            break;
                        }
                    }
                    if (isExist)
                    {
                        HintDialog *hint = new HintDialog();
#if HHT_CHINA
                        hint->setMassage(tr("The item is already in the list ."), -1);
#else
                        hint->setMassage(tr("The item is already in the list ."), -1);
#endif
                        hint->showParentCenter(this);
                        hint->show();
                    }
                    else
                    {
                        hhtAppInfo._fileName = recodeFileName;
                        hhtAppInfo._lnkPath = recodeAbsPath;
                        hhtAppInfo._appIcon = recodeFileIcon;
                        hhtAppInfo._filePath = recodefilepath;
                        HHT_LOG(EN_INFO, "dir 811 add app name:(%s)", hhtAppInfo._fileName.toLocal8Bit().data());
                        QString filename = hhtAppInfo._lnkPath;
                        if(m_pSerialPort->isOpen()){
                            querryTimer->start();
                        }
                        sendFileDate(filename ,hhtAppInfo._appIcon,g_appInfoVector.size()+1,"add");

                        setItemIcon(item,setIconPixmap,false);
                        g_appInfoVector.append(hhtAppInfo);
                        WriteRecords();
                    }
                }
                //----------------------------------------------------------------------------------------------------------------------------
#else
                HintDialog *hint = new HintDialog();
                hint->setMassage(tr("Format not supported. You can add .exe and .lnk files only . ").arg(file_info.fileName()), -2);
                hint->resize(hint->width() - 60, hint->height());
                hint->showParentCenter(this);
                hint->show();
#endif
            }
            else if (file_info.fileName().contains(".exe") || file_info.fileName().contains(".lnk") || file_info.fileName().contains(".EXE") || file_info.fileName().contains(".LNK"))
            {
                QIcon icon;
                QListWidgetItem *item = new QListWidgetItem();
                QFont font;
                QPixmap setIconPixmap;
                font.setFamily("Helvetica");
                font.setPointSize(qRound(11 / g_fontPixelRatio));
                item->setFont(font);

                if (g_fontPixelRatio >= 3)
                {
                    item->setSizeHint(QSize(150, 150));
                    iconSize = 96;
                }
                else
                {
                    item->setSizeHint(QSize(120, 120));
                    iconSize = 64;
                }
                item->setTextColor(Qt::white);
               item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);

				
                if (file_info.fileName().contains(".lnk") || file_info.fileName().contains(".LNK"))
                {//获取lnk对应的exe绝对路径
                    HHT_LOG(EN_INFO, "LNK.====>filePath:(%s)", file_info.filePath().toLocal8Bit().data());
                    qDebug() << "LNK.====>filePath: " << file_info.filePath();
                    QString target = file_info.symLinkTarget();
                    qDebug() << "------>" << target;
                    HHT_LOG(EN_INFO, "target:(%s)", target.toLocal8Bit().data());
                    // 支持桌面UWP快捷方式软件 2017/8/22添加
                    if (target.isEmpty()) {
                        qDebug() << "LNK.====>UWP type file lnk";
                        HHT_LOG(EN_INFO, "LNK.====>UWP type file lnk");
                        uwpitemadd = assistant->findUWPMenuItems(file_info.completeBaseName());
                        if (uwpitemadd->displayName() != NULL)
                        {
                                QByteArray uwpiconbyte = uwpitemadd->png();
                                QPixmap pix;
                                pix.loadFromData(uwpiconbyte);
                                QIcon uappIcon = QIcon(pix);
                                recodeFileIcon = uappIcon;

                                setIconPixmap = uappIcon.pixmap(QSize(iconSize, iconSize));
                        }
                        else
                        {
                            QFileIconProvider icon_provider;
                            QIcon icon = icon_provider.icon(file_info);
                            QPixmap pixmap = icon.pixmap(QSize(48,48));
                            QIcon appIcon = QIcon(pixmap);
                            recodeFileIcon = appIcon;

                            setIconPixmap = appIcon.pixmap(QSize(iconSize, iconSize));
                        }
                        recodeFileName = file_info.completeBaseName();
                        item->setText(file_info.completeBaseName());
                        recodeAbsPath = file_info.filePath();//lnkpath
                        item->setToolTip(file_info.filePath());
                        recodefilepath = "uwp";
                    }
                    else
                    {
                        // =====================================
#ifdef  HHT_SUPPORT_DIR
                        QFileInfo fileInfo(target);
                        if (fileInfo.isDir())
                        {//快捷方式目标文件
                            //文件夹
                            QFileIconProvider icon_provider;
                            icon = icon_provider.icon(QFileInfo(target));
                            recodeFileIcon = icon;//图标

                            setIconPixmap = icon.pixmap(QSize(iconSize, iconSize));
                            QFileInfo targetDir = QFileInfo(target);
                            item->setText(targetDir.fileName());
                            recodeFileName = targetDir.fileName();//名字
                            recodeAbsPath = target;//dir绝对路径
                            item->setToolTip(recodeAbsPath);
                            recodefilepath = file_info.symLinkTarget();
                        }
#endif
                        else if (target.contains(".exe") || target.contains(".EXE"))//快捷方式且指向文件为exe文件
                        {
                            HHT_LOG(EN_INFO, "LNK 2 =====>:(%s)", file_info.filePath().toLocal8Bit().data());

                            // 获取图标
                            QFileIconProvider provider;
                            icon = provider.icon(QFileInfo(target));


                            qDebug() << "LNK 2 =====> " << file_info.filePath();
//                            icon = QIcon(getMaxPixmap(file_info.symLinkTarget()));

                            recodeFileIcon = icon;//图标

                            setIconPixmap = icon.pixmap(QSize(iconSize, iconSize));
                            item->setText(file_info.completeBaseName());
                            recodeFileName = file_info.completeBaseName();
                            recodeAbsPath = file_info.filePath();//lnk绝对路径
                            item->setToolTip(recodeAbsPath);
                            recodefilepath = file_info.symLinkTarget();
                        }
                        else
                        {
                            goto file_can_not_support;
                        }
                    }
					
                }
                else if (file_info.fileName().contains(".exe") || file_info.fileName().contains(".EXE"))//桌面快捷方式是exe文件
                {//获取原文件exe绝对路径
                    // QFileIconProvider icon_provider;
                    // icon = icon_provider.icon(file_info);

                    icon = QIcon(getMaxPixmap(file_info.filePath()));
                    recodeFileIcon = icon;//图标

                    setIconPixmap = icon.pixmap(QSize(iconSize, iconSize));
                    item->setText(file_info.completeBaseName());
                    recodeFileName = file_info.completeBaseName();
                    //修改为拖入文件路径而非目标路径
                    qDebug() << "EXE====>filePath: " << file_info.filePath();
                    HHT_LOG(EN_INFO, "EXE====>filePath:(%s)", file_info.filePath().toLocal8Bit().data());
                    recodeAbsPath = file_info.filePath();
                    item->setToolTip(recodeAbsPath);
                    recodefilepath = file_info.symLinkTarget();
                    if (recodefilepath.isEmpty())
                    {
                        recodefilepath = recodeAbsPath;
                    }
                }

                //----------------------------------------------------------------------------------------------------------------------
                if (ui->listWidget->count() == 0)
                {
                    hhtAppInfo._fileName = recodeFileName;
                    hhtAppInfo._lnkPath = recodeAbsPath;
                    hhtAppInfo._appIcon = recodeFileIcon;
                    hhtAppInfo._filePath = recodefilepath;

                    HHT_LOG(EN_INFO, "link count 0 811 add app name:(%s)", hhtAppInfo._fileName.toLocal8Bit().data());
                    QString filename =  hhtAppInfo._lnkPath;
                    if(m_pSerialPort->isOpen()){
                        querryTimer->start();
                    }
                    sendFileDate(filename ,hhtAppInfo._appIcon,g_appInfoVector.size()+1,"add");

                    setItemIcon(item,setIconPixmap,false);
                    g_appInfoVector.append(hhtAppInfo);
                    WriteRecords();
                }
                else
                {
                    bool isExist = false;
                    for (int i = 0; i < ui->listWidget->count(); i++)
                    {
                        if (ui->listWidget->item(i)->toolTip() == recodeAbsPath)
                        {
                            isExist = true;
                            break;
                        }
                    }
                    if (isExist)
                    {
                        HintDialog *hint = new HintDialog();
#if HHT_CHINA
                        hint->setMassage(tr("The item is already in the list ."), -1);
#else
                        hint->setMassage(tr("The item is already in the list ."), -1);
#endif
                        hint->showParentCenter(this);
                        hint->show();
                    }
                    else
                    {
                        hhtAppInfo._fileName = recodeFileName;
                        hhtAppInfo._lnkPath = recodeAbsPath;
                        hhtAppInfo._appIcon = recodeFileIcon;
                        hhtAppInfo._filePath = recodefilepath;

                        QString filename =  hhtAppInfo._lnkPath;
                        sendFileDate(filename ,hhtAppInfo._appIcon,g_appInfoVector.size()+1,"add");
                        if(m_pSerialPort->isOpen()){
                            querryTimer->start();
                        }

                        setItemIcon(item,setIconPixmap,false);

                        g_appInfoVector.append(hhtAppInfo);
                        WriteRecords();
                    }
                }
                //----------------------------------------------------------------------------------------------------------------------------
            }
            else
            {
file_can_not_support:
                HintDialog *hint = new HintDialog();
#ifdef  HHT_SUPPORT_DIR
# if HHT_CHINA
                hint->setMassage(tr("Format not supported. You can add .exe and file folder only .").arg(file_info.fileName()), -2);
#else
                hint->setMassage(tr("Format not supported. You can add .exe and file folder only . ").arg(file_info.fileName()), -2);
#endif
#else
                hint->setMassage(tr("Format not supported. You can add .exe and .lnk files only . ").arg(file_info.fileName()), -2);
#endif
                hint->resize(hint->width() - 60, hint->height());
                hint->showParentCenter(this);
                hint->show();
            }
        }
    }
    ui->listWidget->setCurrentRow(-1);//自动滚动到最底层
    ui->Trash->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
    ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style:normal;font - stretch:normal;font - family:NotoSans;");
    ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
}



void MainDialog::setItemIcon(QListWidgetItem *item,QPixmap pixmap,bool isSync){
    qDebug() << __FUNCTION__<<pixmap.size();
    if(isSync){
       QWidget *widget =  ui->listWidget->itemWidget(item);
       QList<QLabel*> label  = widget->findChildren<QLabel*>();
       for(int i = 0;i<label.length();i++){
           label.at(i)->setHidden(true);
       }
       widget->setLayout(nullptr);
       ui->listWidget->removeItemWidget(item);
       item->setIcon(QIcon(pixmap));
       ui->listWidget->setStyleSheet("QListWidget{padding-left:30px;padding-top:25px; padding-bottom:25px; "
           "padding-right:5px;border: solid 1.7px #373737;background-color: #121212; border-color:#373737;border-bottom-left-radius:10px;border-bottom-right-radius:10px;}"
           "QListWidget::Item{padding-top:10px; padding-bottom:10px; border:none; border-style:solid;border-radius:10px;border-image:url(:/Resource/image811/default.png);}"
           //"QListWidget::Item:hover{background:skyblue; }"
           "QListWidget::item:selected{color:white; border:none; border-style:solid;border-image:url(:/Resource/image811/selected.png);}"
           "QListWidget::item:selected:!active{border:none;background-color:transparent; }"
       );
       ui->listWidget->update();
    }else{
        if (g_fontPixelRatio >= 3){
            QWidget *pWidget = new QWidget(ui->listWidget);
            pWidget->setStyleSheet("background-color: #212121;");
            pWidget->setFixedSize(122,110);
//            pWidget->setWindowOpacity(0);
            pWidget->setContentsMargins(0, 0, pWidget->contentsMargins().right(), pWidget->contentsMargins().bottom());
            QLabel *pLabel2 = new QLabel(pWidget);
            pLabel2->setFixedSize(120,120);
            pLabel2->setPixmap(pixmap);
            pLabel2->setStyleSheet( "QLabel{padding-left:25px;}"
                                   );
            ui->listWidget->setStyleSheet("QListWidget{padding-left:30px;padding-top:25px; padding-bottom:25px; "
                "padding-right:5px;border: solid 1.7px #373737;background-color: #121212; border-color:#373737;border-bottom-left-radius:10px;border-bottom-right-radius:10px;}"
                "QListWidget::Item{padding-top:10px; padding-bottom:10px; border:none; border-style:solid;border-radius:10px;border-image:url(:/Resource/image811/default.png);}"
                //"QListWidget::Item:hover{background:skyblue; }"
                "QListWidget::item:selected{color:white; border:none; border-style:solid;border-image:url(:/Resource/image811/selected.png);}"
                "QListWidget::item:selected:!active{border:none;background-color:transparent; }"
            );
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, pWidget);
        }else{
            QWidget *pWidget = new QWidget(ui->listWidget);
            pWidget->setStyleSheet("background-color: #212121;");
            pWidget->setFixedSize(95,90);
            pWidget->setWindowOpacity(0);
            pWidget->setContentsMargins(0, 0, pWidget->contentsMargins().right(), pWidget->contentsMargins().bottom());
            QLabel *pLabel2 = new QLabel(pWidget);
            pLabel2->setFixedSize(80,80);
            pLabel2->setPixmap(pixmap);
            pLabel2->setStyleSheet( "QLabel{padding-left:7px;}"
                                   );
            QVBoxLayout *Vboxlayout = new QVBoxLayout(pWidget);
            Vboxlayout->setSpacing(0); // 设置布局间距为0
            Vboxlayout->addWidget(pLabel2);
            ui->listWidget->setStyleSheet("QListWidget{padding-left:30px;padding-top:25px; padding-bottom:25px; "
                "padding-right:5px;border: solid 1.7px #373737;background-color: #121212; border-color:#373737;border-bottom-left-radius:10px;border-bottom-right-radius:10px;}"
                "QListWidget::Item{padding-top:0px; padding-bottom:10px; border:none; border-style:solid;border-radius:10px;border-image:url(:/Resource/image811/default.png);}"
                //"QListWidget::Item:hover{background:skyblue; }"
                "QListWidget::item:selected{color:white; border:none; border-style:solid;border-image:url(:/Resource/image811/selected.png);}"
                "QListWidget::item:selected:!active{border:none;background-color:transparent; }"
            );
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, pWidget);
        }
    }
}


void MainDialog::slot_TrashOpenCOMFailed()
{
	HintDialog *hint = new HintDialog();

#if HHT_CHINA
    hint->setMassage(QString::fromUtf8(" 串口(COM2) 打开失败 . "), -2);
#else
    hint->setMassage(tr(" Serial port (COM2) open failed . "), -2);
#endif

	hint->showParentCenter(this);
	hint->show();
}


void MainDialog::slot_deleteAppFromVector(QString appName)
{
    qDebug() << __FUNCTION__;
    if(m_pSerialPort->isOpen()){
        qDebug()<<__FUNCTION__<<"delete APP "<< appName;
        if (g_appInfoVector.count() > 0)
        {
            for (int i = 0; i < g_appInfoVector.count(); i++)
            {
                if (g_appInfoVector.at(i)._fileName == appName)
                {
                    QString filename = g_appInfoVector.at(i)._lnkPath;
                    sendFileDate(filename,g_appInfoVector.at(i)._appIcon,i,"delete");
                    querryTimer->start();
                    break;
                }
            }
        }
        g_listWidget->setCurrentRow(-1);
        ui->Trash->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
        ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style:normal;font - stretch:normal;font - family:NotoSans;");
        ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
    }else{
        HintDialog* hint = new HintDialog();
        //         hint->showParentCenter(this);
        hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
            (this->pos().y() + this->height() / 2) - (hint->height() / 2),
            hint->width(), hint->height());
#if HHT_CHINA
            hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
            hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
        hint->show();
    }
}

//删除APP同步Android失败槽函数
void MainDialog::slot_deleteAppFromVectorFailed()
{
	HintDialog *hint = new HintDialog();
# if HHT_CHINA
	hint->setMassage(tr("Delete app and sync with smart system failed ."), -2);
#else
	hint->setMassage(tr("Delete app and sync with smart system failed . "), -2);
#endif
	hint->resize(hint->width() - 40, hint->height());
	hint->showParentCenter(this);
	hint->show();
}

void MainDialog::trayiconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		//单击托盘图标
		//双击托盘图标
	{
		this->setVisible(true);
		this->resize(HHT_WIDTH, HHT_HEIGHT);
		this->activateWindow();
		this->showNormal();
	}
	break;
	default:
		break;
	}
}

//同步APP
void MainDialog::slot_syncAction()
{
    qDebug() << "MainDialog::slot_syncAction start";
	this->showNormal();
	hhtHelper::Sleep(100);//睡眠10ms
	HHT_LOG(EN_INFO, "======>[X9X8X6]: TRY TO SYNC APPS.\n");
	on_checkButton_clicked();
    qDebug() << "MainDialog::slot_syncAction end";
}

void MainDialog::slot_quitAction() {
	qDebug() << "quit signal";
	WriteRecords();
	if (!g_appInfoVector.isEmpty())
	{
		g_appInfoVector.clear();
		
	}
	HHT_LOG(EN_INFO, "quit signal");
	done(1);
	this->close();
}

void MainDialog::slot_showMainDialog()
{
	this->trayiconActivated(QSystemTrayIcon::Trigger);
}



//全部同步按钮，其他函数可调用该接口实现全部APP上传
void MainDialog::on_checkButton_clicked()
{

        qDebug() << __FUNCTION__;
        HHT_LOG(EN_INFO, "[click sync app.]");
        hhtHelper::Sleep(100);//睡眠10ms
        //写配置文件
        WriteRecords();
        if (!m_pSerialPort->isOpen())
        {//打开串口失败
            HintDialog* hint = new HintDialog();
            //         hint->showParentCenter(this);
            hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
                (this->pos().y() + this->height() / 2) - (hint->height() / 2),
                hint->width(), hint->height());
#if HHT_CHINA
                hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
                hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
            hint->show();
        }
        else{//打开串口成功
                qDebug() << "====>打开串口成功";
                HHT_LOG(EN_INFO, "======>[X9X8X6]: SYNC APPS.\n");
				if (g_listWidget->count() > 0)
				{

                    qDebug() << "====>start";
                    QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
                    qDebug() << __FUNCTION__<<itemList.size();
                    for (int i = 0; i < itemList.size(); i++)
                    {
                        QListWidgetItem *sel = itemList[i];
                        qDebug() << __FUNCTION__<<sel->text();
                        for (int i = 0; i < g_appInfoVector.count(); i++){
                            if(g_appInfoVector.at(i)._fileName==sel->text()){
        #if HHT_CHINA
                                if (g_appInfoVector.at(i)._fileName == QStringLiteral(APPLICATION_NAME))
        #else
                                if (g_appInfoVector.at(i)._fileName == APPLICATION_NAME)
        #endif
                                {//屏蔽向Android端发送自己信息
                                    qDebug() << "Prevent send " << g_appInfoVector.at(i)._fileName << " to Android";
                                    HHT_LOG(EN_INFO, "Prevent send (%s) to Android", g_appInfoVector.at(i)._fileName.toStdString().c_str());
                                }
                                else{
                                    querryTimer->start();
                                    QString filename =  g_appInfoVector.at(i)._lnkPath;
                                    qDebug()<<__FUNCTION__<<__LINE__<<filename;
                                    QFileInfo file_info(g_appInfoVector.at(i)._lnkPath);
                                    QFileIconProvider icon_provider;
                                    QIcon m_icon = icon_provider.icon(file_info);
                                    m_pixmap = m_icon.pixmap(48,48);
                                    sendFileDate(filename,g_appInfoVector.at(i)._appIcon,i,"add");
                                }
                            }

                        }

                    }

				}
        }
        
}


void MainDialog::onSuccess(QString type,int num){
    if(isSuccess){
        querryTimer->stop();
        if(type=="add"){
           for(int i=0;i < ui->listWidget->count();i++){
               QListWidgetItem *item = ui->listWidget->item(i);
               if(num == i){
                   ui->listWidget->setItemSelected(item,true);

                   QFileInfo fileInfo(g_appInfoVector.at(i)._lnkPath);
                   int iconSize = 64;
                   if(g_fontPixelRatio>=3){
                       iconSize = 96;
                   }else{
                       iconSize = 64;
                   }
                   QPixmap pixmap = g_appInfoVector.at(i)._appIcon.pixmap(iconSize,iconSize);
                   setItemIcon(item,pixmap,true);
                   break;
               }else{
                   ui->listWidget->setItemSelected(item,false);
               }
           }
            HHT_LOG(EN_INFO, "[add apps ssuccess.]");
        }else if(type=="delete"){
            for(int i=0;i < ui->listWidget->count();i++){
                if(num == i){
                    QListWidgetItem *item = ui->listWidget->item(i);
                    int r = ui->listWidget->row(item);
                    delete ui->listWidget->takeItem(r);
                   //界面中listWidget中删除
                    //记录中删除
                    g_appInfoVector.remove(num);
                    WriteRecords();
                    break;
                }
            }

            HHT_LOG(EN_INFO, "[delete apps ssuccess.]");
        }
        qDebug()<< __FUNCTION__<< type << "apps Success";
        UploadWidget* Upload = new UploadWidget(this);
        Upload->showParentCenter(this);
        Upload->show();
        isSuccess = false;
    }else{
        HHT_LOG(EN_INFO, "===>operation failed.");
    }
}


void MainDialog::sendFileDate(QString filename,QIcon icon,int number,QString type){
    if(m_pSerialPort->isOpen()){
        HHT_LOG(EN_INFO, "[start send data  to apps android.]");
        HHT_LOG(EN_INFO, "[type.]",type.toLocal8Bit().data());
        //QString转UTF-8后发送，不这样安卓收到的中文会乱码
        qDebug() <<__FUNCTION__<<filename<<number<<type;
        QTextCodec *pCodec=QTextCodec::codecForName("UTF-8");
        QTextEncoder *encoderWithoutBom =pCodec->makeEncoder( QTextCodec::IgnoreHeader );
        QByteArray filenameArray=encoderWithoutBom ->fromUnicode(filename);
        qDebug() << "fileNameArray"<<filenameArray;
        if(type=="add"){
            QByteArray pixArray;
            QBuffer buffer;
            if(buffer.open(QIODevice::WriteOnly)){
                m_pixmap.save(&buffer,"png");
                pixArray.append(buffer.data());
                buffer.close();
            }
            int file_Total_frame=pixArray.length()/500+1;
            for(int j=0;j<file_Total_frame;j++)
            {
                QByteArray file_data;
                if(j==file_Total_frame-1)
                {
                    file_data=pixArray.mid(500*j);
                }
                else
                {
                    file_data=pixArray.mid(500*j,500);
                }
                QString  datatype="datatype:"+QString::number(File_listAdd,10)+";";
                QString file_num="file_num:"+QString::number(number,10)+";";
                QString file_path="file_path:"+filename+";";
                QString file_data_Total_frame="file_data_Total_frame:"+QString::number(file_Total_frame,10)+";";
                QString file_data_frame_num="file_data_frame_num:"+QString::number(j+1,10)+";";
                QString file_data_len="file_data_len:"+QString::number(file_data.length(),10)+";";
                QString file_check_data = "file_check_data:0;";
                QString HeadEnd="HeadEnd";
                QString data =datatype+file_num+file_path+file_data_Total_frame+file_data_frame_num+file_data_len+HeadEnd;

                QByteArray dataArray=encoderWithoutBom->fromUnicode(data);
                dataArray.append(file_data);

                qDebug()<<"data="<<data;
                qDebug()<<"file_data="<<file_data;
                qDebug()<<"data_len="<<dataArray.length();
                QByteArray ArrayData;
                int data_len = Packed_Data(dataArray,&ArrayData);
                m_pSerialPort->write(ArrayData,data_len);
            }

        }else if(type=="delete"){
            QString  datatype="datatype:"+QString::number(File_listDelete,10)+";";
            QString file_num="file_num:"+QString::number(number,10)+";";
            QString file_path="file_path:"+filename+";";
            QString file_data_Total_frame="file_data_Total_frame:0;";
            QString file_data_frame_num="file_data_frame_num:0;";
            QString file_data_len="file_data_len:0;";
            QString file_check_data = "file_check_data:0;";
            QString HeadEnd="HeadEnd";
            QString data =datatype+file_num+file_path+file_data_Total_frame+file_data_frame_num+file_data_len+HeadEnd;

            QByteArray dataArray=encoderWithoutBom->fromUnicode(data);

            qDebug()<<"data_len="<<dataArray.length();
            QByteArray ArrayData;
            qDebug()<< "dataArray"<<dataArray;
            int data_len = Packed_Data(dataArray,&ArrayData);
            m_pSerialPort->write(ArrayData,data_len);

        }else if(type=="response"){
            QString  datatype="datatype:"+QString::number(Operation_Result_Return,10)+";";
            QString file_num="file_num:"+QString::number(number,10)+";";
            QString file_path="file_path:"+filename+";";
            QString file_data_Total_frame="file_data_Total_frame:0;";
            QString file_data_frame_num="file_data_frame_num:0;";
            QString file_data_len="file_data_len:0;";
            QString file_check_data = "file_check_data:false;";
            QString HeadEnd="HeadEnd";

            QString data =datatype+file_num+file_path+file_data_Total_frame+file_data_frame_num+file_data_len+file_check_data+HeadEnd;

            QByteArray dataArray=encoderWithoutBom->fromUnicode(data);

            qDebug()<<"data_len="<<dataArray.length();
            QByteArray ArrayData;
            qDebug()<< "dataArray"<<dataArray;
            int data_len = Packed_Data(dataArray,&ArrayData);
            m_pSerialPort->write(ArrayData,data_len);
        }
        HHT_LOG(EN_INFO, "[start send data  to apps android  finish.]");
    }else{
        HintDialog* hint = new HintDialog();
        //         hint->showParentCenter(this);
        hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
            (this->pos().y() + this->height() / 2) - (hint->height() / 2),
            hint->width(), hint->height());
#if HHT_CHINA
            hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
            hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
        hint->show();
    }

}

    /*dataInArray输入数据。ArrayData打包后的数据地址
    return  打包后数据长度
    */
int MainDialog::Packed_Data(QByteArray dataInArray,QByteArray *ArrayData)
 {
        ArrayData->resize(1024);
        (*ArrayData)[0]=0x7F;
        (*ArrayData)[1]=0x55;
        (*ArrayData)[2]=0xAA;
        (*ArrayData)[3]=0x01;

        (*ArrayData)[4]=0x00;//数据总帧数
        (*ArrayData)[5]=0x00;//目前是第几帧

        (*ArrayData)[6]=(dataInArray.count()>>8);//数据长度高位
        (*ArrayData)[7]=(dataInArray.count()<<8)>>8;//低位

        unsigned char k=dataInArray.count();

        unsigned char a=(*ArrayData)[6];
        unsigned char b=(*ArrayData)[7];
        unsigned short c=(a|(a<<8)|(b));
        qDebug()<<"k=========="<<k;
        qDebug()<<"a=========="<<a;
        qDebug()<<"b=========="<<b;
        qDebug()<<"c=========="<<c;
        int i=0;
        for(i=0;i<dataInArray.length();i++){
            (*ArrayData)[i+8]=dataInArray[i];
        }
        (*ArrayData)[i+8]=0xff;
        (*ArrayData)[i+8+1]=0xaf;
        (*ArrayData)[i+8+2]=0xef;
        (*ArrayData)[i+8+3]=0xcf;
        (*ArrayData)[i+8+4]='\0';
        int len=8+dataInArray.length()+4;
        return len;
 }



void MainDialog::ReceviceData()
{
    if(m_pSerialPort->isOpen())
    {
        QByteArray buf;
        QByteArray ArrayData;                //解包后得到的数据
        buf = m_pSerialPort->readAll();
        if(buf.length()>0){
            HHT_LOG(EN_INFO, "[start recevice data.]");
            qDebug() << __FUNCTION__<<"receviceData success";
            qDebug()<<"====="<<buf.toHex();
            Analysis_of_Serial_Port_Protocol(buf,buf.length(),&ArrayData);
            qDebug()<<"收到數據ArrayData=========="<<QString::fromLocal8Bit(ArrayData);
            QString ReceData = QString::fromLocal8Bit(ArrayData);
            Parsedata(ReceData);
        }
    }
}

void MainDialog::Parsedata(QString data){
    HHT_LOG(EN_INFO, "[start Parse data.]:(%s)",data.toLocal8Bit().data());
    qDebug()<< __FUNCTION__<<data;
    if(data!=""){
        QStringList list = data.split(";");
        qDebug()<< __FUNCTION__<<list;
        if(list.length() >= 8){
            int dataType =  list[0].split(":")[1].toInt();
            QString file_num =  list[1].split(":")[1];
            QString file_path = "";
            if(list[2].split(":").length()>=3){
               file_path =  list[2].split(":")[1]+":"+list[2].split(":")[2];
            }
            QString file_data_Total_frame =  list[3].split(":")[1];
            QString file_data_frame_num =  list[4].split(":")[1];
            QString file_data_len =  list[5].split(":")[1];
            QString file_check_data =list[6].split(":")[1];
            QString HeadEnd =  list[7];
            QString _type = QString::number(dataType);
            qDebug()<<"data"<<dataType<<file_num<<file_path<<file_data_Total_frame<<file_data_frame_num<<file_data_len;
            HHT_LOG(EN_INFO, "[Date Type]:(%s)",_type.toLocal8Bit().data());
            int file_number = file_num.toInt();
            for(int i = 0;i<g_appInfoVector.length();i++){
                if(file_path == g_appInfoVector.at(i)._lnkPath){
                    file_number = i;
                    break;
                }
            }
            switch (dataType){
                case 1:
                        break;
                case 2:
                        if(file_check_data=="true"){
                            isSuccess=true;
                            onSuccess("check",file_number);
                        }
                        break;
                case 3:
                        break;
                case 4:
                        if(file_check_data=="true"){
                            isSuccess=true;
                            onSuccess("add",file_number);
                        }
                        syncSuccessAPP.append(g_appInfoVector.at(file_number)._fileName);
                        break;
                case 5:
                        if(file_check_data=="true"){
                            isSuccess=true;
                            onSuccess("delete",file_number);
                        }else if(file_check_data=="false"){
                            isSuccess=true;
                           onSuccess("delete",file_number);
                        }
                        break;
                case 6:
                        break;
                case 7:
                        break;
                case 8:
                        break;
                case 9:
                        for(int i=0;i<g_appInfoVector.size();i++){
                            if(file_path==g_appInfoVector.at(i)._lnkPath){
                                slot_openWindowsApp(g_appInfoVector.at(i)._fileName);
                            }
                        }
                        break;
                case 10:
                        break;
                case 11:
                        //切换用户
                        reLoadAppInfo();
                        break;
                case 12:
                        WriteNewlineCastRegisterInfo(false);
                        break;
            }
        }else{
             qDebug()<< __FUNCTION__<<"data losede";
        }
    }else{
         qDebug()<< __FUNCTION__<<"Operation failed";
    }

}


int MainDialog::Analysis_of_Serial_Port_Protocol(QByteArray buffer, int Size,QByteArray *data_result)
{

    int len=0;
    if (Size > 0 ) {
        onDataReceived(buffer, Size);                                                                                        //将收到数据放到缓存区
    }

    qDebug()<<"DataBuffer====="<<DataBuffer.toHex();
    //qDebug()<<"DataBuffer====="<<(unsigned char)DataBuffer[0];
    if(ReadDatapost1!=WriteDatapost1)                                                                                        //缓存区有未处理数据
    {
        if(ReadDatapost1>WriteDatapost1)
        {
            for(int i=0;i<1024-ReadDatapost1;i++)                                                                  //循环每个未处理数据
            {
                unsigned char current_value=(unsigned char)DataBuffer[ReadDatapost1+i];
                if(current_value==0x7F|| data_head_detection>=1 || data_head_num>=1){                                    //判断该未处理数据是否为数据头

                    if(current_value==0x7F){
                        copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                        data_head_detection=1;
                    }else {
                        if(data_head_detection==1){
                            if(current_value==0x55){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_detection=2;
                            }else{
                                data_head_detection=0;
                            }
                        }else if(data_head_detection==2){
                            if(current_value==0xaa){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_detection=3;
                            }else{
                                data_head_detection=0;
                            }
                        }else if(data_head_detection==3){
                            if(current_value==0x01){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_num=4;
                                data_head_detection=0;
                                continue;
                            }else{
                                data_head_detection=0;
                            }
                        }else{
                            //data_head_num=0;
                        }
                    }

                    if(data_head_num>=4){
                        copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);

                        if(data_head_num==7){
                            unsigned char a=Buffer_data[6];
                            unsigned char b=Buffer_data[7];
                            unsigned short c=(a|(a<<8)|(b));
                            data_len=c;
                            qDebug()<<"++++++++++++++++ data_len="<<data_len;
                            if(data_len>1024-12){
                                data_head_num=0;
                                data_len=0;
                                data_end_num=0;
                            }
                        }
                        if(data_head_num==7+data_len+2){
                            if((unsigned char)Buffer_data[data_head_num]==0xaf){
                                if(data_end_num==0){
                                    data_end_num++;
                                }else{
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }else if(data_head_num==7+data_len+3){
                            if((unsigned char)Buffer_data[data_head_num]==0xef){
                                if(data_end_num==1){
                                    data_end_num++;
                                }else{
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }else if(data_head_num==7+data_len+4){
                            if((unsigned char)Buffer_data[data_head_num]==0xcf){
                                if(data_end_num==2){
                                    data_end_num++;
                                }else{
                                    data_head_num=0;
                                    data_end_num=0;
                                    data_len=0;
                                }
                                if(data_end_num==3){
                                    Buffer_data_recevice_NUM=data_len;
                                    len=data_len;
                                    copyQByteArray(&Buffer_data,8,data_result,0,data_len);
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                    continue;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }
                        data_head_num++;
                    }
                }
                if(data_end==1 || data_end==2)
                {
                    data_end=0;
                    break;
                }
            }
            ReadDatapost1=0;
        }
        else {
            for(int i=0;i<WriteDatapost1-ReadDatapost1;i++)//循环每个未处理数据
            {
                unsigned char current_value=(unsigned char)DataBuffer[ReadDatapost1+i];
                if(current_value==0x7F || data_head_detection>=1  || data_head_num>=1){                                    //判断该未处理数据是否为数据头
                    if(current_value==0x7F){
                        copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                        data_head_detection=1;
                    }else {
                        if(data_head_detection==1){
                            if(current_value==0x55){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_detection=2;
                            }else{
                                data_head_detection=0;
                            }
                        }else if(data_head_detection==2){
                            if(current_value==0xaa){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_detection=3;
                            }else{
                                data_head_detection=0;
                            }
                        }else if(data_head_detection==3){
                            if(current_value==0x01){
                                copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                                data_head_num=4;
                                data_head_detection=0;
                                continue;
                            }else{
                                data_head_detection=0;
                            }
                        }else{
                            //data_head_num=0;
                        }
                    }

                    if(data_head_num>=4){
                        copyQByteArray(&DataBuffer,ReadDatapost1+i,&Buffer_data,data_head_num,1);
                        if(data_head_num==7){
                            unsigned char a=Buffer_data[6];
                            unsigned char b=Buffer_data[7];
                            unsigned short c=(a|(a<<8)|(b));
                            data_len=c;
                            qDebug()<<"++++++++++++++++ data_len="<<data_len;
                            if(data_len>1024-12){
                                data_head_num=0;
                                data_len=0;
                                data_end_num=0;
                            }
                        }
                        if(data_head_num==7+data_len+2){
                            if((unsigned char)Buffer_data[data_head_num]==0xaf){
                                if(data_end_num==0){
                                    data_end_num++;
                                }else{
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }else if(data_head_num==7+data_len+3){
                            if((unsigned char)Buffer_data[data_head_num]==0xef){
                                if(data_end_num==1){
                                    data_end_num++;
                                }else{
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }else if(data_head_num==7+data_len+4){
                            if((unsigned char)Buffer_data[data_head_num]==0xcf){
                                if(data_end_num==2){
                                    data_end_num++;
                                }else{
                                    data_head_num=0;
                                    data_end_num=0;
                                    data_len=0;
                                }
                                if(data_end_num==3){
                                    Buffer_data_recevice_NUM=data_len;
                                    len=data_len;
                                    copyQByteArray(&Buffer_data,8,data_result,0,data_len);
                                    data_end_num=0;
                                    data_head_num=0;
                                    data_len=0;
                                    continue;
                                }
                            }else{
                                data_end_num=0;
                                data_head_num=0;
                                data_len=0;
                            }
                        }
                        data_head_num++;
                    }
                }
                if(data_end==1 || data_end==2)
                {
                    data_end=0;
                    break;
                }
            }
            ReadDatapost1=WriteDatapost1;
        }
    }
    return len;
}

//将收到的数据一个一个存入待处理缓存区
void MainDialog::onDataReceived(QByteArray buffer,int Size)
{
    if(WriteDatapost1+Size>=1024) {
        if(WriteDatapost1+Size==1024) {
            copyQByteArray(&buffer,0,&DataBuffer,WriteDatapost1,Size);
            WriteDatapost1=0;
        } else {
            copyQByteArray(&buffer,0,&DataBuffer,WriteDatapost1,1024-WriteDatapost1);
            int a=Size-(1024-WriteDatapost1);
            int b=1024-WriteDatapost1;

            WriteDatapost1=0;

            copyQByteArray(&buffer,b,&DataBuffer,WriteDatapost1,a);
            WriteDatapost1=a;
        }
    } else {
        copyQByteArray(&buffer,0,&DataBuffer,WriteDatapost1,Size);
        WriteDatapost1+=Size;
    }
}

void MainDialog::copyQByteArray(QByteArray *src,int srcPos,QByteArray *dest,int destPos,int len)
{
    for(int i=0;i<len;i++)
    {
        (*dest)[destPos+i]=(*src)[srcPos+i];
    }
}



//隐藏至托盘
void MainDialog::on_trayButton_clicked()
{
	if (!g_appInfoVector.isEmpty())
	{
		WriteRecords();
	}
	this->showMinimized();
	this->hide();
}

void MainDialog::on_aboutButton_clicked()
{
	AboutDialog *about = new AboutDialog();
	about->showParentCenter(this);
	about->show();
}

//已经屏蔽该按钮
void MainDialog::on_minButton_clicked()
{
	if (!g_appInfoVector.isEmpty())
	{
		WriteRecords();
	}
	this->showMinimized();
}

//同步上传，调用on_checkButton_clicked()接口
void MainDialog::on_syncButton_clicked()
{
//	g_nJustClearAppInAndroidFlags = 1;
//	on_checkButton_clicked();//全部上传同步
}

void MainDialog::on_closeButton_clicked()
{
	if (!g_appInfoVector.isEmpty())
	{
		WriteRecords();
	}
	this->showMinimized();
	this->hide();
}

void MainDialog::on_listWidget_clicked(const QModelIndex &index)
{
}

void MainDialog::on_listWidget_itemSelectionChanged()
{
	QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
	if (itemList.size() > 0)
	{
		ui->Trash->setStyleSheet("background-color:#0097a7;border-radius:10px;");
		ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#ffffff;font - style:normal;font - stretch:normal;font - family:NotoSans;");
		ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/delete_1.png);");
	}
	else
	{
		ui->Trash->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
		ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style:normal;font - stretch:normal;font - family:NotoSans;");
		ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
	}
}

void MainDialog::on_Trash_signal_mousepressevent()
{
    ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#ffffff;font - style:normal;font - stretch:normal;font - family:NotoSans;");
    ui->Trash->setStyleSheet("background-color:#0097a7;border-radius:10px;");
    ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/delete-open_1.png);");
}
//delete 删除app
void MainDialog::on_Trash_signal_mousereleaseevent()
{
    qDebug() << __FUNCTION__;
    if(m_pSerialPort->isOpen()){
        QList<QListWidgetItem*> itemList = ui->listWidget->selectedItems();
        for (int i = 0; i < itemList.size(); i++)
        {

            QListWidgetItem *sel = itemList[i];
            qDebug()<<__FUNCTION__<<"delete APP "<< sel->text();
            if (g_appInfoVector.count() > 0)
            {
                for (int i = 0; i < g_appInfoVector.count(); i++)
                {
                    if (g_appInfoVector.at(i)._fileName == sel->text())
                    {
                        QString filename = g_appInfoVector.at(i)._lnkPath;
                        sendFileDate(filename,g_appInfoVector.at(i)._appIcon,i,"delete");
                        querryTimer->start();

                        break;
                    }
                }
            }
        }
        g_listWidget->setCurrentRow(-1);
        ui->Trash->setStyleSheet("background-color:#3c3c3c;border-radius:10px;");
        ui->deletetext->setStyleSheet("background-color:transparent;font-size:22px;color:#717171;font - style:normal;font - stretch:normal;font - family:NotoSans;");
        ui->deleteicon->setStyleSheet("border-image:url(:/Resource/image811/disable-delete_1.png);");
    }else{
        HintDialog* hint = new HintDialog();
        //         hint->showParentCenter(this);
        hint->setGeometry((this->pos().x() + this->width() / 2) - (hint->width() / 3),
            (this->pos().y() + this->height() / 2) - (hint->height() / 2),
            hint->width(), hint->height());
#if HHT_CHINA
            hint->setMassage(QString::fromUtf8("Serial port  open failed ."), -2);
#else
            hint->setMassage(tr(" Serial port  open failed . "), -2);
#endif
        hint->show();
    }

}


void MainDialog::WriteNewlineCastRegisterInfo(bool isRegister){

    QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/NewlineCastRegister.ini";
    qDebug() << "WriteNewlineCastRegisterInfo()------>" << settingsPath;
    QFile file(settingsPath);
    if (!file.exists())
    {
        qDebug() <<"NewlineCastRegister.ini  not exist.";
        file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        file.close();
        HHT_LOG(EN_INFO, "+++++ \"NewlineCastRegister.ini\" not exist.");
    }
    else
    {
        bool result = file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
        if (result)
        {
            QTextCodec *code = QTextCodec::codecForName("UTF-8");
            QTextStream out(&file);
            out.setCodec(code);

            out << "isRegister = " <<isRegister<< endl;
            qDebug() << "isRegister = " << isRegister;
            out.flush();

            _commit(file.handle());
            qDebug() << "_commit(file.handle());";
            file.close();
        }
        else
        {
            HHT_LOG(EN_ERR, "+++++open \"NewlineCastRegister.ini\" failed.");
            file.close();
        }
    }

}

void MainDialog::sendOPSInfo(){
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QString macAddress;
    foreach(QNetworkInterface iface,interfaces){
        if (iface.isValid() && iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            // 获取 MAC 地址
            QString macAddress = iface.hardwareAddress();
            qDebug() << "MAC Address:" << macAddress;
        }
    }
}



