#ifndef UPLOADOKWIDGET
#define UPLOADOKWIDGET

#include <string>

#include <QWidget>
#include <QPushButton>
#include <QTimerEvent>
#include <QPainter>
#include <QPaintEvent>

//自定义动态对号的控件
class UploadsucWidget : public QWidget
{
	Q_OBJECT
public:
	explicit UploadsucWidget(QWidget* parent = 0);

	void StartTimer();

protected:
	void timerEvent(QTimerEvent* ev);
	void paintEvent(QPaintEvent* ev);

private:
	int sTimerId;              //定时器Id
	int sTimeNum;              //计时数

	QColor color_line;         //线的颜色
	QPoint point_start;        //起点
	QPoint point_mid;          //拐点
	QPoint point_end;          //终点
	std::list<QPoint> mPointList;     //记录对号的所有点
	QTimer* updateTimer;
private slots:
	void updateWidget();
};

#endif // UPLOADOKWIDGET.H

