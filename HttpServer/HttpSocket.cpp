#include "HttpSocket.h"
#include "HttpServer.h"

#include <QCoreApplication>
#include <QTimerEvent>
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>

#define MAX_SESSIONS 256
#define SESSIONS_KEEPALIVETIME 300 // sec

#define MAX_BUF (16384)


HttpSocketList HttpSocket::_httpSocketList;
int HttpSocket::_keepAliveTime = 35000; //msec

quint64 HttpSocket::_rxSize = 0;
quint64 HttpSocket::_txSize = 0;

QMap<QString, HttpSession> HttpSocket::_sessionItems;

HttpSocket::HttpSocket(int socketDescriptor, QObject* parent) :
    QTcpSocket(parent),
    _keepAliveTimerId(0)
{
    _httpSocketList.append(this);
    this->setSocketDescriptor(socketDescriptor);
    startKeepAliveTimer();
}

HttpSocket::~HttpSocket()
{
    while(_httpSocketList.contains(this))
        _httpSocketList.removeOne(this);
    this->close();
}

void HttpSocket::clear()
{
    httpRequest.clear();
    httpResponse.clear();
}

void HttpSocket::startKeepAliveTimer()
{
    if (_keepAliveTimerId > 0)
        killTimer(_keepAliveTimerId);
    _keepAliveTimerId = startTimer(_keepAliveTime);
}

void HttpSocket::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == _keepAliveTimerId)
    {
        this->deleteLater();
    }
}

bool HttpSocket::parse()
{
    startKeepAliveTimer();

    if (httpRequest.isHeaderReceived() == false)
    {
        while (this->canReadLine())
        {
            QByteArray line = this->readLine(MAX_BUF);

            appendRxSize(line.size());

            if (line.size() == MAX_BUF)
                return false;

            if (httpRequest.putLine(line) == false)
                return false;

            if (httpRequest.isHeaderReceived() == true)
                break;
        }
    }

    if (httpRequest.method() == HttpRequest::PostMethod)
    {
        if ((httpRequest.contentLength() > 0) && (httpRequest.contentLength() > httpRequest.postData().size()))
        {
            int need = httpRequest.contentLength() - httpRequest.postData().size();
            if ((need > 0) && (need < MAX_BUF))
            {
                QByteArray buf = this->read(need);
                appendRxSize(buf.size());
                httpRequest.appendPostData(buf);
            }
        }
    }

    return true;
}

void HttpSocket::sendResponse()
{
    HttpServer* httpServer = static_cast<HttpServer*>(parent());
    if (httpResponse.code() == HttpResponse::NotFound404Code)
    {
        httpResponse.htmlData() =
                "<html>\r\n"
                "<head>\r\n";

        httpResponse << "<title>" << QCoreApplication::applicationName() << " - 404 Not Found</title>\r\n"
                                                                            "</head>\r\n"
                                                                            "<body>\r\n";

        httpResponse <<
                        "<h1>Page not found</h1>"
                        "<p>The page you were trying to access doesn't exist or has been removed.</p>";

        if (httpRequest.page().size() > 0)
        {
            httpResponse <<
                            "<br/>\r\n"
                            "Page: ";
            httpResponse <<
                            httpRequest.page();
        }

        httpResponse <<
                        "</body>\r\n"
                        "</html>\r\n";

		httpResponse.setContentType(HttpResponse::ContentType::TextHtml);
    }
    else if (httpResponse.code() == HttpResponse::MovedPermanently301Code)
    {
        if (httpResponse.htmlData().size() == 0)
        {
            httpResponse.htmlData() =
                    "<html>\r\n"
                    "<body>\r\n"
                    "MovedPermanently\r\n"
                    "</body>\r\n"
                    "</html>\r\n";
			httpResponse.setContentType(HttpResponse::ContentType::TextHtml);
        }
    }

    QByteArray buf = httpResponse.httpHeader();
    buf += httpResponse.htmlData();

    appendTxSize(buf.size());

    this->write(buf);
    this->flush();

    /*
    QString logFileName = "http-";
    logFileName += QDateTime::currentDateTime().toString(Qt::ISODate);
    logFileName += ".log";
    logFileName.replace(":", "-");
    logFileName = QCoreApplication::applicationDirPath() + logFileName;
*/

    /*
    QFile logFile(logFileName);
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    logFile.write(httpResponse.httpHeader());
    logFile.write(httpResponse.htmlData());
    logFile.close();
    */

    httpResponse.clear();
}

void HttpSocket::createSession()
{
    QByteArray sessionId;
    for (int i = 0; i < 16; i++)
        sessionId += QByteArray::number(qrand() % 0xff, 16);

    httpRequest.setSessionId(sessionId);
    httpResponse.setCookie("MHSESSID=" + sessionId);
}

void HttpSocket::deleteSession()
{
    if (httpRequest.sessionId().isEmpty())
        return;
    _sessionItems.remove(httpRequest.sessionId());
}

void HttpSocket::setSessionValue(const QString& sessionKey, const QString& sessionValue)
{
    if (httpRequest.sessionId().isEmpty())
        return;

    QMap<QString, HttpSession>::iterator i = _sessionItems.find(httpRequest.sessionId());
    if (i != _sessionItems.end())
    {
        if (i.value().hostAddress() != this->peerAddress())
        {
            _sessionItems.remove(i.key());
            return;
        }

        if (i.value().lastAccessDateTime().addSecs(SESSIONS_KEEPALIVETIME) < QDateTime::currentDateTime())
        {
            _sessionItems.remove(i.key());
            return;
        }
        i.value().sessionValue()[sessionKey] = sessionValue;
        i.value().lastAccessDateTime() = QDateTime::currentDateTime();
    }
    else
    {
        if (_sessionItems.size() > MAX_SESSIONS)
        {
            QMap<QString, HttpSession>::iterator idel = _sessionItems.begin();

            for (QMap<QString, HttpSession>::iterator i = _sessionItems.begin(); i != _sessionItems.end(); ++i)
            {
                if (i.value().lastAccessDateTime() < idel.value().lastAccessDateTime())
                    idel = i;
            }
            if (idel != _sessionItems.end())
                _sessionItems.remove(idel.key());

        }
        _sessionItems[httpRequest.sessionId()].sessionValue()[sessionKey] = sessionValue;
        _sessionItems[httpRequest.sessionId()].hostAddress() = this->peerAddress();
        _sessionItems[httpRequest.sessionId()].lastAccessDateTime() = QDateTime::currentDateTime();
    }
}

QString HttpSocket::sessionValue(const QString& sessionKey)
{
    if (httpRequest.sessionId().isEmpty())
        return QString();

    QMap<QString, HttpSession>::iterator iItems = _sessionItems.find(httpRequest.sessionId());
    if (iItems != _sessionItems.end())
    {
        if (iItems.value().hostAddress() != this->peerAddress())
        {
            _sessionItems.remove(iItems.key());
            return QString();
        }

        if (iItems.value().lastAccessDateTime().addSecs(SESSIONS_KEEPALIVETIME) < QDateTime::currentDateTime())
        {
            _sessionItems.remove(iItems.key());
            return QString();
        }

        QMap<QString, QString>::iterator iKey = iItems.value().sessionValue().find(sessionKey);
        if (iKey != iItems.value().sessionValue().end())
        {
            iItems.value().lastAccessDateTime() = QDateTime::currentDateTime();
            return iKey.value();
        }
    }
    return QString();
}

void HttpSocket::appendRxSize(qint64 size)
{
    _rxSize += size;

#ifdef _DEBUG
    //	qDebug() << "HttpSocket::_rxSize: " << _rxSize;
#endif
}

void HttpSocket::appendTxSize(qint64 size)
{
    _txSize += size;
#ifdef _DEBUG
    //	qDebug() << "HttpSocket::_txSize: " << _txSize;
#endif
}
