#include "uploadsucwidget.h"
#include "Pub/ss_pub.h"
#include <QDebug>

UploadsucWidget::UploadsucWidget(QWidget* parent) :
	QWidget(parent),
	point_start(6, 23),
	point_mid(26, 43),
	point_end(66, 3),
	color_line(56, 166, 223),
	sTimeNum(0)
{
	this->setFixedSize(100, 65);

	//定时更新
	updateTimer = new QTimer(this);
	updateTimer->setInterval(2);
	connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateWidget()));
	updateTimer->start();

	//添加左侧线段的点
	for (int xpos = point_start.x(); xpos < point_mid.x(); xpos++) {
		float k = (point_mid.y() - point_start.y()) / (point_mid.x() - point_start.x());
		int ypos = point_start.y() + k * (xpos - point_start.x());
		mPointList.push_back(QPoint(xpos, ypos));
//		qDebug() << "----------->" << "[" << xpos << "," << ypos << "]";
	}

	//添加右侧线段的点
	for (int xpos = point_mid.x(); xpos < point_end.x(); xpos++) {
		float k = (point_end.y() - point_mid.y()) / (point_end.x() - point_mid.x());
		int ypos = point_mid.y() + k * (xpos - point_mid.x());
		mPointList.push_back(QPoint(xpos, ypos));
//		qDebug() << "----------->" << "[" << xpos << "," << ypos << "]";
	}
}

void UploadsucWidget::timerEvent(QTimerEvent* ev)
{
	if (sTimerId == ev->timerId()) {
		sTimeNum++;
		if (point_start.x() + sTimeNum > point_end.x()) {
			this->killTimer(sTimerId);
			PUB_SS.closeUpload();
		}
		this->update();
	}
}

void UploadsucWidget::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(color_line));
	painter.setRenderHint(QPainter::Antialiasing, true);  //反走样
	//绘制对勾
	std::list<QPoint>::iterator iter = mPointList.begin();
	for (; iter != mPointList.end(); ++iter) {
		QPoint point = *iter;
		if (point_start.x() + sTimeNum > point.x()) {
			painter.drawEllipse(point, 4, 4);
		}
		else {
			break;
		}
	}
	
}

void UploadsucWidget::StartTimer()
{
	sTimerId = this->startTimer(1);//5毫秒
}

void UploadsucWidget::updateWidget()
{
	sTimeNum++;
	if (point_start.x() + sTimeNum > point_end.x()) {
		updateTimer->stop();
		PUB_SS.closeUpload();
	}
	this->update();

}