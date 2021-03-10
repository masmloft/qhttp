#pragma once

#include <QTcpSocket>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QHostAddress>

#include "HttpResponse.h"
#include "HttpRequest.h"


class HttpSession
{
public:
    HttpSession()
    {
        _lastAccessDateTime = QDateTime::currentDateTime();
    }

    inline QDateTime& lastAccessDateTime(){return _lastAccessDateTime;};
    inline QMap<QString, QString>& sessionValue(){return _sessionValue;};
    inline QHostAddress& hostAddress(){return _hostAddress;};

private:
    QMap<QString, QString> _sessionValue;
    QDateTime _lastAccessDateTime;
    QHostAddress _hostAddress;
};

class HttpSocket;

typedef QList<HttpSocket*> HttpSocketList;

class HttpSocket : public QTcpSocket
{
public:
    HttpRequest httpRequest;
    HttpResponse httpResponse;
public:
	HttpSocket(int socketDescriptor, QObject* parent);
    ~HttpSocket();
public:
    void clear();
    bool parse();
    void sendResponse();

    static HttpSocketList& httpSocketList() { return _httpSocketList; };

    void createSession();
    void deleteSession();
    void setSessionValue(const QString& sessionKey, const QString& sessionValue);
    QString sessionValue(const QString& sessionKey);

    inline bool isUse() const { return _isUse; };
    inline void setUse(bool value) { _isUse = value; };
private:
    static HttpSocketList _httpSocketList;
    static int _keepAliveTime;

    static quint64 _rxSize;
    static quint64 _txSize;

    bool _isUse;
    int _keepAliveTimerId;
    void startKeepAliveTimer();
    void timerEvent(QTimerEvent* event);

    static QMap<QString, HttpSession> _sessionItems;

    static void appendRxSize(qint64 size);
    static void appendTxSize(qint64 size);
};
