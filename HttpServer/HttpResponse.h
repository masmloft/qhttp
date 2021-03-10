#pragma once

#include <QtGlobal>
#include <QString>
#include <QByteArray>

class HttpResponse
{
public:
	enum Code
	{
		NotFound404Code = 0, 
		Ok200Code, 
		MovedPermanently301Code
	};

	enum class ContentType
	{
		Begin = 0,
		TextHtml = 0,
		TextCss,
		ApplicationXJavascript,
		ApplicationFile,
		ApplicationJson,
		End
	};

	HttpResponse();
	void clear();

	inline Code code()const{return _code;};
	inline void setCode(const Code& value){_code = value;};
	inline void setLocation(const QString& value){_location = value;};
	inline void setContentType(const ContentType& value){_contentType = value;};
	inline void setCookie(const QString& value){_cookie = value;};

	QByteArray httpHeader();
	inline const QByteArray& htmlData()const{return _htmlData;};
	inline QByteArray& htmlData(){return _htmlData;};


	HttpResponse& operator<<(const char* value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(char* value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(const QString& value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(QString& value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(const QByteArray& value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(QByteArray& value){_htmlData.append(value); return *this;};
	HttpResponse& operator<<(qint8 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(qint16 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(qint32 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(qint64 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(quint8 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(quint16 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(quint32 value){_htmlData.append(QString::number(value)); return *this;};
	HttpResponse& operator<<(quint64 value){_htmlData.append(QString::number(value)); return *this;};

private:
	Code _code;
	QString _location;
	QString _cookie;
	ContentType _contentType;
	QByteArray _htmlData;

	static qint32 _keepAliveTimeout; 
};

