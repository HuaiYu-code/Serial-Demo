#ifndef UPLOADOKWIDGET
#define UPLOADOKWIDGET

#include <string>

#include <QWidget>
#include <QPushButton>
#include <QTimerEvent>
#include <QPainter>
#include <QPaintEvent>

//�Զ��嶯̬�ԺŵĿؼ�
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
	int sTimerId;              //��ʱ��Id
	int sTimeNum;              //��ʱ��

	QColor color_line;         //�ߵ���ɫ
	QPoint point_start;        //���
	QPoint point_mid;          //�յ�
	QPoint point_end;          //�յ�
	std::list<QPoint> mPointList;     //��¼�Ժŵ����е�
	QTimer* updateTimer;
private slots:
	void updateWidget();
};

#endif // UPLOADOKWIDGET.H

