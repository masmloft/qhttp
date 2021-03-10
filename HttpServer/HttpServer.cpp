#include "HttpServer.h"

#include <QFile>
#include <QStringList>
#include <QUrl>


HttpServer::HttpServer(const HttpServerCfg& httpServerCfg)
	: QTcpServer()
	, _httpServerCfg(httpServerCfg)
{
//    if (_httpServerModelPtr->isLogEnable->value() == true)
//        _logFile.create(_serverName.isEmpty() ? QString("HttpServer") : _serverName);
    open();
//    connectModel(_httpServerModelPtr->isLogEnable);
}

HttpServer::~HttpServer()
{
}

bool HttpServer::open()
{
    _isOpened = true;
	bool ret = QTcpServer::listen(QHostAddress(_httpServerCfg.host), _httpServerCfg.port);
	if (ret == false)
	{
		const QString str = "Cannot listen " + _httpServerCfg.host + ":" + QString::number(_httpServerCfg.port);
		qWarning() << str;
//        _logFile.warningStr(str);
    }
    return ret;
}

void HttpServer::close()
{
    _isOpened = false;
    QTcpServer::close();
}

void HttpServer::incomingConnection(int socketDescriptor)
{
    HttpSocket* httpSocket = new HttpSocket(socketDescriptor, this);
    connect(httpSocket, SIGNAL(readyRead()), this, SLOT(tcpSocketReadyRead()));
	connect(httpSocket, SIGNAL(disconnected()), this, SLOT(tcpSocketDisconnect()));
}

void HttpServer::tcpSocketDisconnect()
{
    if (HttpSocket* httpSocket = static_cast<HttpSocket*>(sender())) {
        httpSocket->deleteLater();
    }
}

void HttpServer::tcpSocketReadyRead()
{
	if (HttpSocket* httpSocket = static_cast<HttpSocket*>(sender()))
	{
		if (httpSocket->parse() == false)
		{
            httpSocket->close();
            return;
        }

		//_logFile.noticeStr(httpSocket->httpRequest.url());

		if (httpSocket->httpRequest.isAccepted())
		{
			reqHttpPage(*httpSocket);
            httpSocket->clear();
        }
    }
}

void HttpServer::notice(QString message)
{
//    if (_httpServerModelPtr->isLogEnable->value())
//        _logFile.noticeStr(message);
}

void HttpServer::warning(QString message)
{
//    if (_httpServerModelPtr->isLogEnable->value())
//        _logFile.warningStr(message);
}

bool HttpServer::reqHttpPage(HttpSocket& httpSocket)
{
	bool ret = false;

	const QByteArray page = httpSocket.httpRequest.page().toUpper().trimmed();

	{
		auto it = pageFuncMap.find(page);
		if(it != pageFuncMap.end())
		{
			return it->second(httpSocket);
		}
	}

	{
		QString fileName(page);

		if (fileName[0] != QChar('/'))
			fileName = "/" + fileName;
		fileName = ":/resources" + fileName;
		fileName = fileName.toLower();

		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
		{
			httpSocket.httpResponse.clear();
			httpSocket.httpResponse << file.readAll();

			if (fileName.indexOf(".html") > 0)
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::TextHtml);
			else if (fileName.indexOf(".css") > 0)
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::TextCss);
			else if (fileName.indexOf(".js") > 0)
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::ApplicationXJavascript);
			file.close();
			httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
			httpSocket.sendResponse();
			return true;
		}
	}

	{
		QString fileName(page);

		fileName = _httpServerCfg.sharePath + "/www/" + fileName;
		fileName = fileName.toLower();

		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly) == true)
		{
			httpSocket.httpResponse.clear();
			httpSocket.httpResponse << file.readAll();
			file.close();

			if (fileName.endsWith(".html", Qt::CaseInsensitive))
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::TextHtml);
			else if (fileName.endsWith(".css", Qt::CaseInsensitive))
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::TextCss);
			else if (fileName.endsWith(".js", Qt::CaseInsensitive))
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::ApplicationXJavascript);
			else
				httpSocket.httpResponse.setContentType(HttpResponse::ContentType::ApplicationFile);

			httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
			httpSocket.sendResponse();
			return true;
		}
	}

	if(reqOtherPageFunc)
	{
		return reqOtherPageFunc(httpSocket);
	}

	return ret;
}
