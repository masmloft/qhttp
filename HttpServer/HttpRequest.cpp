#include "HttpRequest.h"

HttpRequest::HttpRequest() :
_lineReceived(0),
_isHeaderReceived(false),
_method(ZeroMethod),
_contentLength(0)
{

}

void HttpRequest::clear()
{
	_lineReceived = 0;
	_isHeaderReceived = false;
	_method = ZeroMethod; 
	_url.clear(); 
	_page.clear();
	_getList.clear();
	_host.clear(); 
	_contentLength = 0; 
	_sessionId.clear(); 
	_cacheControl.clear(); 
	_postData.clear();
}

bool HttpRequest::putLine(const QByteArray& value)
{
	_lineReceived++;

	if (_lineReceived == 1)
	{
		if (value.toUpper().startsWith("GET ") == true)
		{
			_method = GetMethod;

			int i = value.indexOf(" ", 4);
			if (i <= 0)
			{
				_lineReceived = 0;
				return false;
			}

			_url = value.mid(4, i - 4);

			if (_url.isEmpty())
			{
				_lineReceived = 0;
				return false;
			}
		}
		else if (value.toUpper().startsWith("POST ") == true)
		{
			_method = PostMethod;

			int i = value.indexOf(" ", 5);
			if (i <= 0)
			{
				_lineReceived = 0;
				return false;
			}

			_url = value.mid(5, i - 5);

			if (_url.isEmpty())
			{
				_lineReceived = 0;
				return false;
			}
		}
		else
		{
			_lineReceived = 0;
			return false;
		}

		int i = _url.indexOf("?");
		if (i > 0)
		{
			_page = _url.mid(0, i);
			_getList = _url.mid(i + 1).split('&');
		}
		else
		{
			_page = _url;
		}
	}
	else
	{
		if (value.toUpper().startsWith("HOST: ") == true)
		{
			_host = value.mid(6, value.size() - 6 - 2);
		}
		else if (value.toUpper().startsWith("COOKIE: ") == true)
		{
			QByteArray allCookie = value.mid(8, value.size() - 8 - 2);
//			allCookie.remove(" ");
			allCookie = allCookie.trimmed();
			QList<QByteArray> sl = allCookie.split(';');
			for (int i = 0; i < sl.size(); i++)
			{
				const QByteArray& str = sl.at(i);
				if (str.toUpper().startsWith("MHSESSID=") == true)
				{
					_sessionId = str.mid(9, str.size() - 9);
				}
			}
		}
		else if (value.toUpper().startsWith("CONTENT-LENGTH: ") == true)
		{
			QByteArray str = value.mid(16, value.size() - 16 - 2);
			bool ok = false;
			_contentLength = str.toInt(&ok);
			if (ok == false)
				_contentLength = 0;
		}
		else if (value == "\r\n")
		{
			_isHeaderReceived = true;
		}
	}
	return true;
}

bool HttpRequest::isAccepted()
{
	if (_method == ZeroMethod)
		return false;
	if (_url.isEmpty())
		return false;

	if (_host.isEmpty())
		return false;

	if (_method == GetMethod)
		if (_isHeaderReceived == true)
			return true;

	if (_method == PostMethod)
		if (_isHeaderReceived == true)
			if (_contentLength == _postData.size())
				return true;

	return false;
}

QByteArray HttpRequest::getVar(const QByteArray& key)
{
	QByteArray k = key;
	k.append('=');
	for (int i = 0; i < _getList.size(); i++)
	{
		const QByteArray v = _getList.at(i);
		if (v.indexOf(k) == 0)
		{
			QByteArray ba(v.mid(k.size()));
			ba.replace("+", " ");
			return QByteArray::fromPercentEncoding(ba);
		}
	}
	return QByteArray();
}

QByteArray HttpRequest::postVar(const QByteArray& key)
{
	QByteArray k = key;
	k.append('=');

	QList<QByteArray> postData = _postData.split('&');
	for (int i = 0; i < postData.size(); i++)
	{
		const QByteArray& v = postData.at(i);
		if (v.indexOf(key) == 0)
		{
			QByteArray ba(v.mid(k.size()));
			ba.replace("+", " ");
			return QByteArray::fromPercentEncoding(ba);
		}
	}
	return QByteArray();
}
