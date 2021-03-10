#pragma once

#include <QString>

class HttpServerCfg
{
public:
	QString host = "0.0.0.0";
	int port = 80;
	QString sharePath;
	bool isLogEnable = true;
	int keepaliveSocket = 1000;
	int keepaliveSession = 1000;
	QString serverName = "HttpServer";
};
