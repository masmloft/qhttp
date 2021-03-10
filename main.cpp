#include <memory>
#include <csignal>

#ifdef QT_GUI_LIB
#	include <QApplication>
#else
#	include <QCoreApplication>
#endif
#include <QDebug>
#include <QDir>

#include "TcmuServer.h"

#ifndef _WIN64
class CleanExit : public QObject
{
public:
    explicit CleanExit(QObject* parent = 0)
    {
        signal(SIGINT, &CleanExit::exitQt);
        signal(SIGTERM, &(CleanExit::exitQt));
    }

private:
    static void exitQt(int sig)
    {
        qDebug() << "Shutdown application CTRL+C.";
        QCoreApplication::exit(0);
    }
};
#endif

int main(int argc, char *argv[])
{
#ifdef QT_GUI_LIB
    QApplication a(argc, argv);
#else
    QCoreApplication a(argc, argv);
#endif

	HttpServerCfg httpServerCfg;
	httpServerCfg.port = 81;
	//httpServerCfg.sharePath = "e:/share";
	httpServerCfg.sharePath = QCoreApplication::applicationDirPath();
	TcmuServer tcmuServer(httpServerCfg);

	int ret = a.exec();

    return ret;
}
