#include "WebServer.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>

#include "HttpServer/HttpPages.h"

WebServer::WebServer(const HttpServerCfg& httpServerCfg)
	: _httpServer(httpServerCfg)
{
	{
		_httpServer.pageFuncMap["/API/TIME"] = [](HttpSocket& httpSocket) -> bool
		{
			PageApplicationJson page(httpSocket);
			page.httpResponse() << Json("time", QDateTime::currentDateTime().toMSecsSinceEpoch()).toUtf8();
			return true;
		};
	}

	{
		_httpServer.pageFuncMap["/API/STATE"] = [this](HttpSocket& httpSocket) -> bool
		{
			Json json;
			json.append("time", QDateTime::currentDateTime().toMSecsSinceEpoch());
			json.append("temperature", _dataModel.hard.temperature);

			PageApplicationJson page(httpSocket);
			page.httpResponse() << json.toUtf8();
			return true;
		};
	}

	{
		_httpServer.pageFuncMap["/API/SESSION"] = [this](HttpSocket& httpSocket) -> bool
		{
			Json json;
			json.append("end", _dataModel.session.end);
			json.append("finished", _dataModel.session.finished);
			json.append("id", _dataModel.session.id);
			json.append("running", _dataModel.session.running);
			json.append("start", _dataModel.session.start);
			json.append("timer", _dataModel.session.timer);

			PageApplicationJson page(httpSocket);
			page.httpResponse() << json.toUtf8();
			return true;
		};
	}

	startTimer(1000);
}

void WebServer::timerEvent( QTimerEvent* event )
{
	_dataModel.hard.temperature = (double)(qrand() % 400) / 10;
}
