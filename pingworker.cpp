#include "pingworker.h"

PingWorker::PingWorker(const QString &ip)
    : QObject(nullptr)
    , ipAddress(ip)
{}
void PingWorker::doPing() {
    QProcess pingProcess;
    #ifdef Q_OS_WIN
        pingProcess.start("ping", QStringList() << "-n" << "1" << "-w" << "1000" << ipAddress);
    #else
        pingProcess.start("ping", QStringList() << "-c" << "1" << "-W" << "1" << ipAddress);
    #endif

    if (!pingProcess.waitForFinished(2000)) {
        pingProcess.kill();
        emit pingFinished(false);
        return;
    }

    QString output = QString::fromLocal8Bit(pingProcess.readAllStandardOutput());
    bool success = isPingSuccessful(output);
    emit pingFinished(success);
}

bool PingWorker::isPingSuccessful(const QString &output)
{
#ifdef Q_OS_WIN
    return output.contains("TTL=") || output.contains("Reply from");
#else
    return output.contains("1 received") || output.contains("ttl=");
#endif
}
