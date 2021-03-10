#pragma once

#include <functional>

#include <QTcpServer>

#include "HttpServerModel.h"
#include "HttpSocket.h"

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
	using PageFunc = std::function<bool(HttpSocket& httpSocket)>;
	using PageFuncMap = std::map<QByteArray, PageFunc>;
public:
	PageFuncMap pageFuncMap;
	std::function<bool(HttpSocket& httpSocket)> reqOtherPageFunc;
public:
	HttpServer(const HttpServerCfg& httpServerCfg);
    ~HttpServer();
public:
	const HttpServerCfg& httpServerCfg() const { return _httpServerCfg; }
	bool open();
    void close();
private:
	HttpServerCfg _httpServerCfg;
	bool _isOpened = false;
	HttpSession _httpSession;
private:
	void incomingConnection(int socketDescriptor) override;
	bool reqHttpPage(HttpSocket& httpSocket);
	void notice(QString message);
	void warning(QString message);
private slots:
    void tcpSocketReadyRead();
    void tcpSocketDisconnect();

};
