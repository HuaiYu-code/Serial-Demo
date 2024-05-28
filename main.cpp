#include "maindialog.h"
#include <QApplication>
#include <QFontDatabase>
#include "Helper/hhthelper.h"
#include <QSharedMemory>
#include <QMessageBox>
#include "Helper/hhthelper.h"
#include <QTranslator>
#include <corecrt_io.h>
#include <QDir>
#include <QSettings>
#include <QTime>
#include "singleapplication.h"
#include "ss_pub.h"
#include "global.h"
extern char g_Version[100];

double g_fontPixelRatio; //全局字体缩放比例
extern INT32  get_curlang;

#define LOG_FILE     qApp->applicationDirPath()+"/logger.txt"
//日志文件
static  void MessageOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex mutex;
	mutex.lock();
	QString text;
	switch (type)
	{
	case QtDebugMsg:
		text = QString("Debug:");
		break;

	case QtWarningMsg:
		text = QString("Warning:");
		break;

	case QtCriticalMsg:
		text = QString("Critical:");
		break;

	case QtFatalMsg:
		text = QString("Fatal:");
		break;
	default:
		break;
	}
	//日志写到文件
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString message = QString("%1 %2%3").arg(current_date_time).arg(text).arg(msg);
	QFile file(LOG_FILE);
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << message << "\r\n";
	file.flush();
	//_commit(file.handle());
	file.close();
	mutex.unlock();
}

// 设置程序自启动 appPath程序路径
#define AUTO_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
void setProcessAutoRunSelf(bool isstart)
{
    // 注册表路径需要使用双反斜杠，如果是32位系统，要使用QSettings::Registry32Format;64位系统 QSettings::Registry64Format
    QSettings *settings = new QSettings(AUTO_RUN, QSettings::NativeFormat);

    // 以程序名称作为注册表中的键
    // 根据键获取对应的值（程序路径）
    QString application_name = QApplication::applicationName(); // 获取应用名称
    QString path = settings->value(application_name).toString();

    // 如果注册表中的路径和当前程序路径不一样，
    // 则表示没有设置自启动或自启动程序已经更换了路径
    // toNativeSeparators的意思是将"/"替换为"\"
    if (isstart)
    {
        QString appPath = QApplication::applicationFilePath(); // 找到应用的目录
        QString newPath = QDir::toNativeSeparators(appPath);
        if (path != newPath)
        {
            settings->setValue(application_name, newPath);
        }
    }
    else
    {
        settings->remove(application_name);
    }
}

void autoRun()
{
    setProcessAutoRunSelf(true);
}

void ReadGet_curlang()
{
    QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/config.txt";
    QFile file(settingsPath);
    if (!file.exists())//default
    {
        get_curlang = 1;
        return;
    }
    else
    {
        QSettings settings(settingsPath, QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("System"));
        get_curlang = settings.value("LANGUAGE/Langtype", get_curlang).toInt();
        qDebug() << "get_curlang" << get_curlang;
        HHT_LOG(EN_INFO, "get_curlang(%d)",get_curlang);
    }
}


void ReadConfig()
{
    QString strCfgPath = "Settings/Config/config.dat";
    QFile file(strCfgPath);
    if (!file.exists())//default
    {
        get_curlang = 1;
        return;
    }
    else
    {
        QSettings settings(strCfgPath, QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("System"));
        get_curlang = settings.value("LANGUAGE/Langtype", get_curlang).toInt();
        qDebug() << "get_curlang" << get_curlang;
        HHT_LOG(EN_INFO, "get_curlang(%d)", get_curlang);
    }
}

void WriteToConfig()
{
    if (get_curlang > 1)
    {
        QString settingsPath = PUB_SS.Pub_GetUserDataPath() + "/config.txt";
        QFile file(settingsPath);
        if (!file.exists())
        {
            file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
            file.close();
            HHT_LOG(EN_INFO, "+++++ \"config.txt\" not exist.");

        }
        else
        {
            bool result = file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
            if (result)
            {
                QTextCodec* code = QTextCodec::codecForName("UTF-8");
                QTextStream out(&file);
                out.setCodec(code);
                //	QString appinfo = "[LANGUAGE]";
                out << "[LANGUAGE]" << endl;
                out << "Langtype=" << get_curlang << endl;
                HHT_LOG(EN_INFO, "get_curlang(%d)", get_curlang);
                out.flush();

                file.close();
            }
            else
            {
                HHT_LOG(EN_ERR, "+++++open \"config.dat\" failed.");
                file.close();
            }
        }
    }
}



int main(int argc, char *argv[])
{  
	//Qt日志捕捉
//    qInstallMessageHandler(MessageOutPut);

    int nRet = 0;
    // 单例应用程序，禁止应用多开
    SingleApplication a(argc, argv);
    if(a.isRuning()) return 0;

     QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));	
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);//Hight DPI support
#endif
	  
#if HHT_CHINA
    QApplication::setApplicationName(QString::fromUtf8(APPLICATION_NAME));
#else
    QApplication::setApplicationName(QString::fromUtf8(APPLICATION_NAME));
#endif
    int index= QFontDatabase::addApplicationFont(":/Resource/Helvetica.ttf");
    if(index!=-1)
    {
        QStringList strList(QFontDatabase::applicationFontFamilies(index));
        if(strList.count()>0)
        {
            qDebug()<<"fontName:"<<strList.at(0);
            QFont  appFont(strList.at(0));
            appFont.setPixelSize(9);
            a.setFont(appFont);
        }
    }
     //获取系统字体缩放比例
     g_fontPixelRatio =hhtHelper::GetFontPixelRatio();
     qDebug()<<"[System font scales ratios: "<<g_fontPixelRatio<<"]";
//     HHT_LOG(EN_INFO,"[System font scales ratios (%f)]\n",g_fontPixelRatio);
     //获取软件版本号
     memset(g_Version,0,100);
     hhtHelper::GetAppVersion(g_Version);

     QString appName = QCoreApplication::applicationName();
     qDebug ()<<"Application Name --->"<<appName;
     //An instacne has already running
     const QString   windowTitle = QString::fromUtf8(APPLICATION_NAME);
     const QString  windowClass = "Qt5QWindowIcon"; //find it out this name with "Microsoft Spy++"
     const wchar_t *WInTitle = reinterpret_cast<const wchar_t*>(windowTitle.utf16());
     const wchar_t *WInClass = reinterpret_cast<const wchar_t*>(windowClass.utf16());
     HWND hwnd = FindWindow(WInClass,WInTitle);
     //HWND hwnd = ::FindWindowA(NULL, "Newline assistant");
     qDebug()<<"[window hwnd---->"<<hwnd<<"]";
     if(hwnd)
     {//
         qDebug()<<"An instance is running";
         SendMessage(hwnd,SHOWNORNAL,0,0);
         return 0;
     }


    Sleep(1000);
    ReadConfig();
    QTranslator* trans = new QTranslator(&a);
    WriteToConfig();
    ReadGet_curlang();
    HHT_LOG(EN_INFO, "get_curlang(%d)", get_curlang);
    qDebug()<<"get_curlang"<<get_curlang;
    switch(get_curlang)
    {
        case 1:
            trans->load(":/lang_en.qm");
            break;
        case 2:
            trans->load(":/lang_Spanish.qm");
            break;
        case 3:
            trans->load(":/lang_Itanlian.qm");
            break;
        case 4:
            trans->load(":/lang_German.qm");
            break;
        case 5:
            trans->load(":/lang_Finnish.qm");
            break;
        case 6:
            trans->load(":/lang_Danish.qm");
            break;
        case 7:
            trans->load(":/lang_Polish.qm");
            break;
        case 8:
            trans->load(":/lang_Russian.qm");
            break;
        case 9:
            trans->load(":/lang_Dutch.qm");
            break;
        case 10:
            trans->load(":/lang_French.qm");
            break;
        case 11:
            trans->load(":/lang_Svenska.qm");
            break;
        case 12:
            trans->load(":/lang_Eesti.qm");
            break;
        case 13:
            trans->load(":/lang_Lietuviu.qm");
            break;
        case 14:
            trans->load(":/lang_Slovenian.qm");
            break;
        case 15:
            trans->load(":/lang_Cestina.qm");
            break;
        case 16:
            trans->load(":/lang_Magyar.qm");
            break;
        case 17:
            trans->load(":/lang_Romana.qm");
            break;
        case 18:
            trans->load(":/lang_Bulgarian.qm");
            break;
        case 19:
            trans->load(":/lang_Ukrainian.qm");
            break;
        case 20:
            trans->load(":/lang_Norwegian.qm");
            break;
        case 21:
            trans->load(":/lang_Catalan.qm");
            break;
        case 22:
            trans->load(":/lang_Valencian.qm");
            break;
        case 23:
            trans->load(":/lang_Galician.qm");
            break;
        case 24:
            trans->load(":/lang_Basque.qm");
            break;
        case 25:
            trans->load(":/lang_Kazakh.qm");
            break;
        case 26:
            trans->load(":/lang_Latvian.qm");
            break;
        case 27:
            trans->load(":/lang_Croatian.qm");
            break;
        case 28:
            trans->load(":/lang_Slovak.qm");
            break;
        case 29:
            trans->load(":/lang_Traditional-Chinese.qm");
            break;
        case 30:
            trans->load(":/lang_Thai.qm");
            break;
        case 31:
            trans->load(":/lang_Vietnam.qm");
            break;
        case 32:
            trans->load(":/lang_Indonesia.qm");
            break;
        case 33:
            trans->load(":/lang_Japanese.qm");
            break;
        case 34:
            trans->load(":/lang_Korean.qm");
            break;
        default:
            break;
    }
    a.installTranslator(trans);
    autoRun();  // 设置开机自动运行, add by chenpeinan

    MainDialog w;
    qDebug()<<"MainDialog Title: "<<w.windowTitle();
    HHT_LOG(EN_INFO, "MainDialog Title:(%s)", w.windowTitle().toStdString().c_str());


#if 1
    w.showMinimized();
    w.hide();
#else
    w.show();
#endif
    return a.exec();
}








