#include "TcmuServer.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>

#include "HttpServer/HttpPages.h"

TcmuServer::TcmuServer(const HttpServerCfg& httpServerCfg)
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
			json.append("temperature", _hardData.temperature);

			PageApplicationJson page(httpSocket);
			page.httpResponse() << json.toUtf8();
			return true;
		};
	}

	startTimer(1000);
}

void TcmuServer::timerEvent( QTimerEvent* event )
{
	_hardData.temperature = (double)(qrand() % 400) / 10;
}


void TcmuServer::pageIndexHtml( HttpSocket& httpSocket )
{
/*
	httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
	httpSocket.httpResponse << 
		"<html>\r\n"
		"<head>\r\n"
		"<title>" << QCoreApplication::applicationName() << "</title>\r\n"
		"</head>\r\n"
		"<body>\r\n";

//	httpSocket.httpResponse << httpSocket.httpRequest.page() << "<br/>";
	httpSocket.httpResponse << 
		"<h1>Page not found</h1>"
		"<p>The page you were trying to access doesn't exist or has been removed.</p>"
		"</body>\r\n"
		"</html>\r\n";
*/
}

void TcmuServer::pageRPhp( HttpSocket& httpSocket )
{
//	const QList<QByteArray>& getList = httpSocket.httpRequest.getList();

//    // notice(QString("List count: %1").arg(getList.count()));

//	bool retParseTcmuMsg = _tcmuMsgDecoder.parseTcmuMsg(getList);

//	httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
//	httpSocket.httpResponse <<
//		"<html>\r\n"
//		"<head>\r\n"
//		"<title>" << QCoreApplication::applicationName() << "</title>\r\n"
//		"</head>\r\n"
//		"<body>\r\n";

//	if (retParseTcmuMsg == true)
//	{
//		httpSocket.httpResponse << "<RFPC>" << _tcmuMsgDecoder.retHttp() << "</RFPC>\r\n";
//	}
//	else
//	{
//		httpSocket.httpResponse << "<RFPC>" << "ERR" << "</RFPC>\r\n</BR>" << _tcmuMsgDecoder.retHttp();
//	}

//	httpSocket.httpResponse <<
//		"</body>\r\n"
//		"</html>\r\n";

//    if (retParseTcmuMsg) notice("OUT: " + _tcmuMsgDecoder.retHttp());
}

void TcmuServer::pageTcmutest( HttpSocket& httpSocket )
{
	httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);

	httpSocket.httpResponse << 
		"<html>\r\n"
		"<head>\r\n"
		"<title>tcmutest</title>\r\n";

	httpSocket.httpResponse << 
		"<script type='text/javascript'>\r\n"
		"function sendmsg(arg)\r\n"
		"{\r\n"
		"location.href = '/r.php?id=' + document.getElementById('TcmuAddrId').value + '&'+ arg;\r\n"
		"}\r\n"
		"</script>\r\n";
	
	httpSocket.httpResponse << 
		"</head>\r\n"
		"<body>\r\n"
		"<br/>\r\n";


	httpSocket.httpResponse << "Terminal Id <select id='TcmuAddrId' >";
	httpSocket.httpResponse << "</br>";

//	TrmTable& trmTable = _dataModel->staticTableModel->trmTableModel->data();
//	for (int i = 0; i < trmTable.rowCount(); ++i)
//	{
//		const TrmTableRow* tableRow = trmTable.tableRow(i);
//		if(tableRow == NULL)
//			continue;

//		if(tableRow->wdha <= 0)
//			continue;

//		httpSocket.httpResponse << "<option>" << tableRow->wdha << "</option>";
//	}

	httpSocket.httpResponse << "</select>";

//	httpSocket.httpResponse << "</br>TerminalID <input id='TcmuAddrId' type='text' size='6' value='1' ></br>\r\n";


	QString tcmutestFileName = _httpServer.httpServerCfg().sharePath + "/tcmutest.txt";
	QFile tcmutestFile(tcmutestFileName);

	if (tcmutestFile.open(QIODevice::ReadOnly))
	{
		QByteArray buf = tcmutestFile.readAll();

		while(buf.indexOf("\r\n") >= 0)
			buf.replace("\r\n", "\n");

		while(buf.indexOf("\t\t") >= 0)
			buf.replace("\t\t", "\t");
/*
		while(buf.indexOf("\t ") >= 0)
			buf.replace("\t ", " ");
		while(buf.indexOf(" \t") >= 0)
			buf.replace(" \t", " ");
*/
		while(buf.indexOf("  ") >= 0)
			buf.replace("  ", " ");

		QList<QByteArray> lines = buf.split('\n');

		for(int iLine = 0; iLine < lines.size(); ++iLine)
		{
			const QByteArray& line = lines.at(iLine);

			if (line.isEmpty())
			{
				httpSocket.httpResponse << "</br>\r\n";
				continue;
			}

			if (line.startsWith('#'))
			{
				continue;
			}

			QList<QByteArray> fields = line.split('\t');

			if (fields.size() == 2)
			{
				httpSocket.httpResponse << "</br> <a href=\"javascript: sendmsg('" << fields.at(1) << "');\" >" << fields.at(0) << "</a>\r\n";
			}
		}
	}

	httpSocket.httpResponse << 
		"</body>\r\n"
		"</html>";

}
