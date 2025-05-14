#ifndef PINGWORKER_H
#define PINGWORKER_H

#include <QObject>
#include <QProcess>

class PingWorker : public QObject {
    Q_OBJECT
public:
    explicit PingWorker(const QString& ip);

signals:
    void pingFinished(bool success);

public slots:
    void doPing();

private:
    bool isPingSuccessful(const QString& output);
private:
    QString ipAddress = "192.168.0.13";
};

#endif // PINGWORKER_H
