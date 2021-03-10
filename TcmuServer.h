#pragma once

#include <map>
#include <functional>

#include <QObject>
#include <QMap>

#include "./HttpServer/HttpServer.h"

class TcmuServer : public QObject
{
public:
	TcmuServer(const HttpServerCfg& httpServerCfg);
protected:
	void timerEvent(QTimerEvent* event) override;
private:
//	using PageFunc = std::function<bool(HttpSocket& httpSocket)>;
//	using PageFuncMap = std::map<QByteArray, PageFunc>;
private:
	HttpServer _httpServer;
//	PageFuncMap _pageFuncMap;
private:
	bool reqPage(HttpSocket& httpSocket);

	void pageIndexHtml(HttpSocket& httpSocket);
	void pageRPhp(HttpSocket& httpSocket);

	void pageTcmutest(HttpSocket& httpSocket);

};
