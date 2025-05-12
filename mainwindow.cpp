#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QColorDialog>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	SetUpCSSStyle();
	GetDeviceInfo();
//	connect(&m_timer, &QTimer::timeout, this, &MainWindow::slotCheckDevice);
//	m_timer.start(m_checkInterval);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::SetDeviceMode(int mode)
{
	QUrlQuery postData;
	postData.addQueryItem("mode", QString::number(mode));
	SendRequest("http://192.168.0.13/mode", Method::POST, postData);
}

void MainWindow::SetDeviceBrightness(int brightness)
{
	QUrlQuery postData;
	postData.addQueryItem("brightness", QString::number(brightness));
	SendRequest("http://192.168.0.13/brightness", Method::POST, postData);
}

void MainWindow::SetDeviceState(int state)
{
	QUrlQuery postData;
	postData.addQueryItem("state", QString::number(state));
	SendRequest("http://192.168.0.13/state", Method::POST, postData);
}

void MainWindow::SetDeviceColor(int color)
{
	QUrlQuery postData;
	postData.addQueryItem("color", QString::number(color));
	SendRequest("http://192.168.0.13/color", Method::POST, postData);
}

void MainWindow::GetDeviceInfo()
{
	SendRequest("http://192.168.0.13/info", Method::GET);
}

void MainWindow::SetUpDeviceInfo(QByteArray info)
{
	QJsonParseError parseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(info, &parseError);

	if (QJsonParseError::NoError == parseError.error) {
		QJsonObject jsonObj = jsonDoc.object();
		m_cur_active_mode = jsonObj.value("mode").toInt();
		UpdateMode();
		m_cur_state = jsonObj.value("state").toBool();
		m_cur_state ? ui->pb_on->setText("Выкл.") : ui->pb_on->setText("Вкл.");
		ui->hs_brightness->setValue(jsonObj.value("brightness").toInt());
		QColor cur_color;
		cur_color.setRgba(jsonObj.value("color").toDouble());
		qDebug() << cur_color.name();
		qDebug() << jsonObj.value("color").toDouble();
		ui->l_color->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(cur_color.name()));
	}
}

void MainWindow::UpdateMode()
{
	ui->pb_fill->setStyleSheet("");
	ui->pb_noise->setStyleSheet("");
	ui->pb_pong->setStyleSheet("");
	ui->pb_rainbow->setStyleSheet("");
	ui->pb_river->setStyleSheet("");
	ui->pb_stars->setStyleSheet("");

	switch (m_cur_active_mode) {
	case 0:
		ui->pb_fill->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(0);
		break;
	case 1:
		ui->pb_pong->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(1);
		break;
	case 2:
		ui->pb_rainbow->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(2);
		break;
	case 3:
		ui->pb_noise->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(3);
		break;
	case 4:
		ui->pb_stars->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(4);
		break;
	case 5:
		ui->pb_river->setStyleSheet("background-color: #FF5722;");
		SetDeviceMode(5);
		break;
	}
}

bool MainWindow::isDeviceConnected(const QString &ipAddress)
{
	QProcess pingProcess;
#ifdef Q_OS_WIN
	pingProcess.start("ping", QStringList() << "-n" << "1" << "-w" << "1000" << ipAddress);
#else
	pingProcess.start("ping", QStringList() << "-c" << "1" << "-W" << "1" << ipAddress);
#endif

	if (!pingProcess.waitForFinished(1500)) {
		pingProcess.kill();  // Принудительно завершаем процесс
		return false;
	}

	return (pingProcess.exitCode() == 0);
}

void MainWindow::on_pb_fill_clicked()
{
	m_cur_active_mode = 0;
	UpdateMode();
}


void MainWindow::on_pb_pong_clicked()
{
	m_cur_active_mode = 1;
	UpdateMode();
}


void MainWindow::on_pb_rainbow_clicked()
{
	m_cur_active_mode = 2;
	UpdateMode();
}


void MainWindow::on_pb_noise_clicked()
{
	m_cur_active_mode = 3;
	UpdateMode();
}


void MainWindow::on_pb_stars_clicked()
{
	m_cur_active_mode = 4;
	UpdateMode();
}


void MainWindow::on_pb_river_clicked()
{
	m_cur_active_mode = 5;
	UpdateMode();
}

void MainWindow::slotCheckDevice()
{
	bool isConnected = isDeviceConnected(m_ipAddress);

	if (isConnected != m_lastStatus) {
		m_lastStatus = isConnected;
		if (isConnected) {
			qDebug() << "Устройство" << m_ipAddress << "подключено к сети";
			ui->l_isConnected->setText("Online");
		} else {
			qDebug() << "Устройство" << m_ipAddress << "отключилось от сети";
			ui->l_isConnected->setText("Offline");
		}
	}
}

void MainWindow::SetUpCSSStyle()
{

}


void MainWindow::on_pb_on_clicked()
{
	if (m_cur_state) {
		SetDeviceState(0);
		m_cur_state = false;
		ui->pb_on->setText("Вкл.");
	} else {
		SetDeviceState(1);
		m_cur_state = true;
		ui->pb_on->setText("Выкл.");
	}
}


void MainWindow::on_pb_color_clicked()
{
	QColor color = color_dia.getColor();

	qDebug() << color_dia.currentColor().rgba();
	SetDeviceColor(color.rgba());
	ui->l_color->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(color.name()));
}

void MainWindow::SendRequest(const QString &url, const Method method, const QUrlQuery &postData)
{
	QUrl q_url(url);
	QNetworkRequest request(q_url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	qDebug() << postData.toString();
	switch (method) {
	case Method::GET:
		m_reply = m_manager->get(request);
		break;
	case Method::POST:
		m_reply = m_manager->post(request, postData.toString().toUtf8());
		break;
	}

	QObject::connect(m_reply, &QNetworkReply::finished, [=]() {
		if (m_reply->error() == QNetworkReply::NoError) {
			QByteArray response = m_reply->readAll();
			if (Method::GET == method) {
				SetUpDeviceInfo(response);
			}
			qDebug() << "Response:" << response;
		} else {
			qDebug() << "Error:" << m_reply->errorString();
		}
	});
}

void MainWindow::on_hs_brightness_valueChanged(int value)
{
    SetDeviceBrightness(value);
}

