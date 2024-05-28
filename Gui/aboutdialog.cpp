#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "Helper/fontscaleratio.h"
#include "Helper/iconhelper.h"
#include "Helper/hhthelper.h"
extern double      g_fontPixelRatio;
extern char          g_Version[50];
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    FontScaleRatio::Instance()->setGuiFont("Helvetica",12,ui->titleLabel);
    initUiFontSize(g_fontPixelRatio);
    QString Version =hhtHelper::CharToQString(g_Version);
#if HHT_CHINA
    ui->titleLabel->setText(tr("Newline Assistant"));
    ui->versionLabel->setText(tr("Version ")+Version.mid(0,Version.size()-2) );
    ui->okButton->setText(tr("OK"));
	ui->titleLabel-> setStyleSheet("font-size:16px;color:#292929;font - style:normal; font - stretch:normal; font - family:NotoSans;");
	ui->versionLabel->setStyleSheet("font-size:11px;color:#292929;font - style:normal; font - stretch:normal; font - family:NotoSans;");
	ui->label_2->setText(tr("Copyright (C) 2018-2020"));
	ui->label_2-> setStyleSheet("font-size:9px;color:#808080;font - style:normal; font - stretch:normal; font - family:NotoSans;");
#else
    ui->titleLabel->setText("Newline assistant");
    ui->versionLabel->setText("Newline assistant : Version "+Version.mid(0,Version.size()-2) );
#endif
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//窗口置顶
	setAttribute(Qt::WA_TranslucentBackground);
    //this->setWindowModality(Qt::ApplicationModal);//阻塞除当前窗体之外的所有的窗体
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::showParentCenter(QWidget *parentFrm)
{
    this->setGeometry((parentFrm->pos().x()+parentFrm->width()/2)- (this ->width()/2),
                                         (parentFrm->pos().y()+parentFrm->height()/2)-(this->height()/2),
                      this->width(),this->height());
}

void AboutDialog::initUiFontSize(double fontSizeRatio)
{
     QFont font;
     font.setFamily("Helvetica");
     font.setPointSize(14/g_fontPixelRatio);
     ui->versionLabel->setFont(font);
     ui->label_2->setFont(font);
     ui->okButton->setFont(font);
     QFontMetrics fm = QFontMetrics(font);
     int text_width = 10+fm.width(ui->versionLabel->text())+50;
     this->resize(QSize(text_width,this->height()));
}

void AboutDialog::on_closeButton_clicked()
{
    this->close();
}

void AboutDialog::on_okButton_clicked()
{
    this->close();
}

void AboutDialog::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();
    this->mousePos = event->globalPos();
    this->dPos = mousePos - windowPos;
}

void AboutDialog::mouseMoveEvent(QMouseEvent *event)
{
     this->move(event->globalPos() - this->dPos);
}
