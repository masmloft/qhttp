#pragma once

#include <map>

#include <QString>
#include <QVariant>

#include "HttpSocket.h"

class Json
{
	std::map<QString, QVariant> _data;
public:
	Json() { }
	Json(const QString& key, const QVariant& val) { append(key, val); }
public:
	QByteArray toUtf8() const
	{
		QByteArray ret;
		ret = "{";
		for(auto it = _data.begin(); it != _data.end();)
		{
			ret += "\"" + it->first.toUtf8() + "\":";
			const QVariant& v = it->second;
			switch (v.type())
			{
			case QVariant::Type::Bool:
				ret += v.toBool() ? "true" : "false";
				break;
			case QVariant::Type::Int:
			case QVariant::Type::UInt:
			case QVariant::Type::LongLong:
			case QVariant::Type::ULongLong:
			case QVariant::Type::Double:
				ret += v.toString();
				break;
			default:
				ret += "\"" + v.toString().toUtf8() + "\"";
			}

			++it;
			if(it == _data.end())
				break;
			ret += ",";
		}
		ret += "}";
		return ret;
	}

	void append(const QString& key, const QVariant& val)
	{
		_data[key] = val;
	}
};


class PageTextHtml
{
public:
	PageTextHtml(HttpSocket& httpSocket)
		: _httpSocket(httpSocket)
	{
		_httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
		_httpSocket.httpResponse.setContentType(HttpResponse::ContentType::TextHtml);

	}

	~PageTextHtml()
	{
		_httpSocket.sendResponse();
	}
public:
	HttpResponse& httpResponse()
	{
		return _httpSocket.httpResponse;
	}
private:
	HttpSocket& _httpSocket;
};


class PageApplicationJson
{
public:
	PageApplicationJson(HttpSocket& httpSocket)
		: _httpSocket(httpSocket)
	{
		_httpSocket.httpResponse.setCode(HttpResponse::Ok200Code);
		_httpSocket.httpResponse.setContentType(HttpResponse::ContentType::ApplicationJson);
	}

	~PageApplicationJson()
	{
		_httpSocket.sendResponse();
	}
public:
	HttpResponse& httpResponse()
	{
		return _httpSocket.httpResponse;
	}
private:
	HttpSocket& _httpSocket;
};
