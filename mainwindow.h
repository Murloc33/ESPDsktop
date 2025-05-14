#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>
#include <QColorDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

enum Method {
	GET,
	POST
};

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_pb_fill_clicked();
	void on_pb_pong_clicked();
	void on_pb_rainbow_clicked();
	void on_pb_noise_clicked();
	void on_pb_stars_clicked();
	void on_pb_river_clicked();

	void on_hs_brightness_valueChanged(int value);
	void on_pb_on_clicked();
	void on_pb_color_clicked();

    void slotSetDeviceStatus(bool isOnline);

private:
	void SendRequest(const QString &url, const Method method, const QUrlQuery &postData = {});


	void SetDeviceMode(int mode);
	void SetDeviceBrightness(int brightness);
	void SetDeviceState(int state);
	void SetDeviceColor(int color);

	void GetDeviceInfo();
	void SetUpDeviceInfo(QByteArray info);

	void UpdateMode();;

	bool isDeviceConnected(const QString& ipAddress);

    void startPingInSeparateThread();

private:
	Ui::MainWindow	*ui;

    QString			m_ipAddress = "192.168.0.19";

	int				m_cur_active_mode = 0;

	QTimer			m_timer;
	int				m_checkInterval = 1000;
	bool			m_lastStatus = false;

	bool			m_cur_state = true;

	QColorDialog	color_dia {};

	struct {
		int cur_active_mode;
		int cur_active_brightness;
		int cur_active_state;
		int cur_active_color;
	} device_info;

	QNetworkAccessManager	*m_manager = new QNetworkAccessManager(this);
	QNetworkReply			*m_reply;
};
#endif // MAINWINDOW_H
