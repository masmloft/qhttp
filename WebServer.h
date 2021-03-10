#pragma once

#include <map>
#include <functional>

#include <QObject>
#include <QMap>

#include "./HttpServer/HttpServer.h"

struct DataModel
{
	struct HardData
	{
		double temperature = 0;
	} hard;

	struct Session
	{
		int64_t end = 0;
		bool finished = false;
		int id = 1;
		bool running = false;
		int64_t start = 0;
		int timer = 0;
	} session;
};



class WebServer : public QObject
{
public:
	WebServer(const HttpServerCfg& httpServerCfg);
protected:
	void timerEvent(QTimerEvent* event) override;
private:
	HttpServer _httpServer;
	DataModel _dataModel;
private:

};
