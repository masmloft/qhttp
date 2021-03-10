#pragma once
#include <QtGlobal>
#include <QString>
#include <QStringList>

class HttpRequest
{
public:
	enum Method{ZeroMethod = 0, GetMethod, PostMethod};

	HttpRequest();
	void clear();

	bool isAccepted();

	bool putLine(const QByteArray& value);

	inline bool isHeaderReceived() { return _isHeaderReceived; }

	inline const Method& method() const { return _method; }
	inline const QByteArray& url() const { return _url; }
	inline const QByteArray& page() const { return _page; }
	inline const QList<QByteArray>& getList() const { return _getList; }
	inline const QString& host() const { return _host; }
	inline const int& contentLength() const { return _contentLength; }

	inline const QByteArray& sessionId() const { return _sessionId; }
	inline void setSessionId(const QByteArray& val) { _sessionId = val; }

	inline const QString& cacheControl() const { return _cacheControl; }
	
	inline const QByteArray& postData() { return _postData; }
	inline const QList<QByteArray> postList() const { return _postData.split('&'); }
	inline void appendPostData(const QByteArray& val) { _postData += val; }

	QByteArray getVar(const QByteArray& key);
	QByteArray postVar(const QByteArray& key);

private:
	int _lineReceived;
	bool _isHeaderReceived;

	Method _method;
	QByteArray _url;
	QByteArray _page;
	QList<QByteArray> _getList;
	QString _host;
	int _contentLength;
	QByteArray _sessionId;
	QString _cacheControl;
	QByteArray _postData;

};
