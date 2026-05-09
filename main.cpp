#include "widget.h"
#include <QApplication>
#include <QByteArray>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <cstdio>

namespace {
QMutex &logMutex()
{
    static QMutex mutex;
    return mutex;
}

const char *logLevelName(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return "DEBUG";
    case QtInfoMsg:
        return "INFO";
    case QtWarningMsg:
        return "WARN";
    case QtCriticalMsg:
        return "ERROR";
    case QtFatalMsg:
        return "FATAL";
    }
    return "LOG";
}

void installConsoleStreams()
{
}

void commandLineMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString line = QString("[%1] [%2] %3")
                       .arg(timestamp, logLevelName(type), message);

    if (context.file && context.line > 0) {
        line += QString(" (%1:%2)").arg(context.file).arg(context.line);
    }

    const QByteArray utf8 = line.toUtf8();
    FILE *output = (type == QtDebugMsg || type == QtInfoMsg) ? stdout : stderr;
    {
        QMutexLocker locker(&logMutex());
        std::fprintf(output, "%s\n", utf8.constData());
        std::fflush(output);
    }

    if (type == QtFatalMsg) {
        std::abort();
    }
}
}

int main(int argc, char *argv[])
{
    installConsoleStreams();
    qInstallMessageHandler(commandLineMessageHandler);

    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
