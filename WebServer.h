#pragma once

#include <map>
#include <functional>

#include <QObject>
#include <QMap>

#include "./HttpServer/HttpServer.h"

class HardData
{
public:
	double temperature = 0;
};

class WebServer : public QObject
{
public:
	WebServer(const HttpServerCfg& httpServerCfg);
protected:
	void timerEvent(QTimerEvent* event) override;
private:
	HttpServer _httpServer;
	HardData _hardData;
private:

};
