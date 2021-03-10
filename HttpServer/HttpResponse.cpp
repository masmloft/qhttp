#include "HttpResponse.h"

static char CONTENT_TYPE[(size_t)HttpResponse::ContentType::End][32] =
{
	"text/html",//; charset=windows-1251",
	"text/css",
	"application/x-javascript",
	"application/file",
	"application/json"
};

qint32 HttpResponse::_keepAliveTimeout = 35000;

HttpResponse::HttpResponse() :
_code(NotFound404Code),
_contentType(ContentType::TextHtml)
{

}

void HttpResponse::clear()
{
	_code = NotFound404Code; 
	_location.clear();
	_cookie.clear();
	_contentType = ContentType::TextHtml;
	_htmlData.clear();
}

QByteArray HttpResponse::httpHeader()
{
	QByteArray ret;

	if (_code == Ok200Code)
		ret += "HTTP/1.1 200 OK\r\n";
	else if (_code == MovedPermanently301Code)
		ret += "HTTP/1.1 301 Moved Permanently\r\n";
	else
		ret += "HTTP/1.1 404 Not Found\r\n";

	ret += "Server: MhHttpServer\r\n";

	if (_location.size() > 0)
		ret += QString("Location: %1\r\n").arg(_location);

	if (_cookie.size() > 0)
		ret += QString("Set-Cookie: %1\r\n").arg(_cookie);

	if (_htmlData.size() > 0)
		ret += QString("Content-Length: %1\r\n").arg(_htmlData.size());

	if ((_contentType >= ContentType::Begin) && (_contentType < ContentType::End))
	{
		ret += QString("Content-Type: %1\r\n").arg(CONTENT_TYPE[(size_t)_contentType]);
	}

	ret += QString("Keep-Alive: timeout=%1, max=100\r\n").arg(_keepAliveTimeout / 1000);
	ret += "Connection: Keep-Alive\r\n";
	ret += "\r\n";

	return ret;
}
