#include "betaseriesapi.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtCore/QStringList>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QDebug>

BetaSeriesApi::BetaSeriesApi(QObject *parent)
    : QObject(parent)
    , _networkManager(new QNetworkAccessManager(this))
{
    connect(_networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onResult(QNetworkReply*)));
    connect(_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(_networkManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this, SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));
}

BetaSeriesApi::~BetaSeriesApi()
{
    if (isUserConnected())
        postMembersDestroy();
    if (_networkManager)
        _networkManager->deleteLater();
}

/******************************************************************************/
// PUBLIC METHODS
/******************************************************************************/
bool BetaSeriesApi::isUserConnected()
{
    return _connectedUser.isConnected;
}

QString BetaSeriesApi::userLogin()
{
    return _connectedUser.login;
}

void BetaSeriesApi::getUpdateFile()
{
    QUrl url("https://sourceforge.net/projects/serizer/files/Serizer.xml/download");
    _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getTimelineHome(uint nbpp, double since_id)
{
    QUrl url("https://api.betaseries.com/timeline/home");

    QUrlQuery params;
    params.addQueryItem("nbpp", QString::number(nbpp));
    if (since_id != 0) params.addQueryItem("since_id", QString::number(since_id, 'f').split(".")[0]);
    url.setQuery(params);

    _replyList << _networkManager->get(createRequestWithUrl(url));
}

/**
 * @brief BetaSeriesApi::getEpisodesList Get 
 * @param limit
 * @param showId
 */
void BetaSeriesApi::getEpisodesList(uint limit, uint showId)
{
    QUrl url("https://api.betaseries.com/episodes/list");

    QUrlQuery params;
    params.addQueryItem("token", _connectedUser.token);
    if (limit != 0) params.addQueryItem("limit", QString::number(limit));
    if (showId != 0) params.addQueryItem("showId", QString::number(showId));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getPicturesShows(double id)
{
    QUrl url("https://api.betaseries.com/pictures/shows");

    QUrlQuery params;
    params.addQueryItem("id", QString::number(id));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getMembersInfos(double id, bool summary)
{
    QUrl url("https://api.betaseries.com/members/infos");

    QUrlQuery params;
    params.addQueryItem("token", _connectedUser.token);
    if (id == 0) id = _connectedUser.id;
    params.addQueryItem("id", QString::number(id));
    params.addQueryItem("summary", QString::number(summary));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getShowsEpisodes(double id, double season, double episode)
{
    QUrl url("https://api.betaseries.com/shows/episodes");

    QUrlQuery params;
    params.addQueryItem("token",  _connectedUser.token);
    params.addQueryItem("id",      QString::number(id));
    params.addQueryItem("season",  QString::number(season));
    params.addQueryItem("episode", QString::number(episode));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getCommentsShow(double id, int limit)
{
    QUrl url("https://api.betaseries.com/comments/comments");

    QUrlQuery params;
    params.addQueryItem("token", _connectedUser.token);
    params.addQueryItem("type", "show");
    params.addQueryItem("id", QString::number(id));
    params.addQueryItem("nbpp", QString::number(limit));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

void BetaSeriesApi::getEpisodesDisplay(double id)
{
    QUrl url("https://api.betaseries.com/episodes/display");

    QUrlQuery params;
    params.addQueryItem("token", _connectedUser.token);
    params.addQueryItem("id", QString::number(id));
    url.setQuery(params);

   _replyList << _networkManager->get(createRequestWithUrl(url));
}

/******************************************************************************/
// PUBLIC SLOTS
/******************************************************************************/
/**
 * @brief BetaSeriesApi::postMembersAuth
 * @param login
 * @param password
 */
void BetaSeriesApi::postMembersAuth(QString login, QString password)
{
    closeCurrentConnections();

    QUrl url("https://api.betaseries.com/members/auth");

    QUrlQuery params;
    params.addQueryItem("login", login);
    params.addQueryItem("password", QString(QCryptographicHash::hash((password.toUtf8()),QCryptographicHash::Md5).toHex()));
    url.setQuery(params);

   _replyList << _networkManager->post(createRequestWithUrl(url), params.query(QUrl::FullyEncoded).toUtf8());
}

/**
 * @brief BetaSeriesApi::postMembersDestroy
 */
void BetaSeriesApi::postMembersDestroy() // TODO delete see doc
{
    closeCurrentConnections();
    // TODO Disconnect all current connections
    if (isUserConnected()) {
        QUrl url("https://api.betaseries.com/members/destroy");

        QUrlQuery params;
        params.addQueryItem("token", _connectedUser.token);
        url.setQuery(params);

       _replyList << _networkManager->post(createRequestWithUrl(url), params.query(QUrl::FullyEncoded).toUtf8());
    }
}

/**
 * @brief BetaSeriesApi::postEpisodesWatched
 * @param id
 * @param note
 */
void BetaSeriesApi::postEpisodesWatched(uint id, int note)
{
    qDebug() << "BetaSeriesApi::postEpisodesWatched :" << id;
    QUrl url("https://api.betaseries.com/episodes/watched");

    QUrlQuery params;
    params.addQueryItem("token", _connectedUser.token);
    params.addQueryItem("id", QString::number(id));
    if (note != 0) params.addQueryItem("note", QString::number(note));
    url.setQuery(params);

    _replyList << _networkManager->post(createRequestWithUrl(url), params.query(QUrl::FullyEncoded).toUtf8());
}

/******************************************************************************/
// PROTECTED METHODS
/******************************************************************************/
void BetaSeriesApi::loginSuccessEvent()
{
    emit loginSuccess();
}

void BetaSeriesApi::unlogSuccessEvent()
{
    emit unlogSuccess();
}

void BetaSeriesApi::memberInfosEvent(QVariantMap infos)
{
    emit memberInfosReceived(infos);
}

void BetaSeriesApi::timelineHomeEvent(QVariant timelineHome)
{
    emit timelineHomeReceived(timelineHome);
}

void BetaSeriesApi::unseenSeriesEvent(QVariant unseenSeries)
{
    emit unseenSeriesUpdate(unseenSeries);
}

void BetaSeriesApi::picturesShowsEvent(uint showId, QByteArray image)
{
    emit picturesShowsReceived(showId, image);
}

void BetaSeriesApi::episodeWatchedEvent(double episodeId)
{
    emit episodeWatched(episodeId);
}

void BetaSeriesApi::showEpisodesEvent(long showId, QVariantList episodes)
{
    emit showEpisodesReceived(showId, episodes);
}

void BetaSeriesApi::commentsEpisodeEvent(QVariantList comments)
{
     emit commentsEpisodeReceived(comments);
}

void BetaSeriesApi::commentsShowEvent(QVariantList comments)
{
     emit commentsShowReceived(comments);
}

void BetaSeriesApi::commentsMemberEvent(QVariantList comments)
{
    emit commentsMemberReceived(comments);
}

void BetaSeriesApi::commentsMovieEvent(QVariantList comments)
{
     emit commentsMovieReceived(comments);
}

/**
 * @brief BetaSeriesApi::episodeDisplayEvent Display episode informations.
 */
void BetaSeriesApi::episodeDisplayEvent(QVariantMap infos)
{
    emit episodeDisplayReceived(infos);
}

/******************************************************************************/
// PRIVATE SLOTS
/******************************************************************************/
/**
 * @brief BetaSeriesApi::onResult
 * @param reply
 */
void BetaSeriesApi::onResult(QNetworkReply *reply)
{
    if (reply == 0)
        qWarning() << "Reply is null !";

    switch (reply->error()) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::TimeoutError:
        qDebug() << "TimeOut Error";
        break;
    case QNetworkReply::UnknownContentError:
        qDebug() << "UnkownContentError:" << reply->error() << reply->errorString();
        break;
    default:
        qDebug() << "Unknown error" << reply->error() << reply->errorString();
    }

    QString req = reply->url().path();
    QByteArray data = reply->readAll();
    QString replyType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

    if (replyType == "application/json") {
        QJsonParseError jerr;
        QJsonDocument doc = QJsonDocument::fromJson(data, &jerr);
        if (jerr.error != QJsonParseError::NoError) {
            qWarning() << "Error during parsing of:" << reply->url();
            qWarning() << "Json error:" << jerr.errorString();
            qWarning() << "Data:" << data;
        } else {
            QVariantMap map = doc.object().toVariantMap();
            if (map["errors"] != QVariant::Invalid && !map["errors"].toList().isEmpty()) {
                foreach (QVariant error, map["errors"].toList()) {
                    uint errorCode = error.toMap()["code"].toInt();
                    switch (errorCode) {
                    case 1001: // Invalid API Key
                        emit missingApiKey();
                        break;
                    case 1002: // Invalid type
                        emit invalidType();
                        break;
                    case 1003: // Invalid Action
                        emit invalidAction();
                        break;
                    case 2001: // Invalid Token user
                        emit invalidToken();
                        break;
                    case 2002: // Privacy conflict
                        emit privacyConflict();
                        break;
                    case 2003: // Serie already addded
                        emit serieAlreadyAdded();
                        break;
                    case 2004: // Serie not in account
                        emit serieNotInAccount();
                        break;
                    case 2005: // User has not see this episode
                        emit episodeUnseen();
                        break;
                    case 2006: // They're not friends ... no.
                        emit notFriends();
                        break;
                    case 2007: // Invalid user options.
                        emit invalidUserOptions();
                        break;
                    case 3001: // Missing var
                        emit missingVar();
                        break;
                    case 3002: // Must have 2 char
                        emit atLeast2char();
                        break;
                    case 3003: // Must be a digit
                        emit mustBeDigit();
                        break;
                    case 3004: // Incorrect value
                        emit incorrectValue();
                        break;
                    case 3005: // Unauthorized char
                        emit unauthorizedChar();
                        break;
                    case 3006: // Mail. Not . Valid
                        emit invalidMail();
                        break;
                    case 4001: // Serie does not exist anymore
                        emit invalidSerie();
                        break;
                    case 4002: // User is a chimera
                        emit unknownUser();
                        break;
                    case 4003: // Bad password
                        emit badPassword();
                        break;
                    case 4004: // User already exist !! Haha !!! Youre not very original guy ..
                        emit userAlreadyExist();
                        break;
                    default:
                        qDebug() << "error:" << errorCode << "::" << error.toMap()["text"].toString();
                    }
                }
            } else if (req == "/timeline/home"
                       && map["events"] != QVariant::Invalid) {
                timelineHomeEvent(map["events"]);
            } else if (req == "/members/auth"){
                _connectedUser.isConnected = true;
                _connectedUser.inAccount = map["user"].toMap()["in_account"].toBool();
                _connectedUser.id = map["user"].toMap()["id"].toDouble();
                _connectedUser.login = map["user"].toMap()["login"].toString();
                _connectedUser.hash = map["hash"].toString();
                _connectedUser.token = map["token"].toString();
                loginSuccessEvent();
            } else if (req == "/members/destroy"){
                _connectedUser = User();
                unlogSuccessEvent();
            } else if (req == "/members/infos") {
                memberInfosEvent(map["member"].toMap());
            } else if (req == "/episodes/list") {
                unseenSeriesEvent(map["shows"]);
            } else if (req == "/episodes/watched") {
                episodeWatchedEvent(map["episode"].toMap()["id"].toDouble());
            } else if (req == "/shows/episodes") {
                QUrlQuery query(reply->url().query());
                long id = query.queryItemValue("id").toLong();
                showEpisodesEvent(id, map["episodes"].toList());
            } else if (req == "/comments/comments") {
                QUrlQuery query(reply->url().query());
                QString type = query.queryItemValue("type");
                if (type == "episode") commentsEpisodeEvent(map["comments"].toList());
                if (type == "show")    commentsShowEvent(map["comments"].toList());
                if (type == "member")  commentsMemberEvent(map["comments"].toList());
                if (type == "movie")   commentsMovieEvent(map["comments"].toList());
            } else if (req == "/episodes/display"){
                episodeDisplayEvent(map["episode"].toMap());
            } else {
                qDebug() << doc.toJson();
            }
        }
    } else if (replyType == "image/jpeg") {
        if (req == "/pictures/shows") {
            QUrlQuery params(reply->url());
            picturesShowsEvent(params.queryItemValue("id").toDouble(), data);
        }
    } else {
        qDebug() << data;
    }
}

void BetaSeriesApi::onAuthenticationRequired(QNetworkReply */*reply*/, QAuthenticator */*authenticator*/)
{
    qDebug() << "Authentication  required";
}

void BetaSeriesApi::onSslErrors(QNetworkReply */*reply*/, QList<QSslError> /*errors*/)
{
    qDebug() << "Some SSL errors";
}

QNetworkRequest BetaSeriesApi::createDefaultRequest()
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
#if defined Q_OS_WIN
    request.setHeader(QNetworkRequest::UserAgentHeader, qApp->applicationName() +" (by Liomka)/Win");
#elif defined Q_OS_MAC
    request.setHeader(QNetworkRequest::UserAgentHeader, qApp->applicationName() +" (by Liomka)/Mac");
#elif defined Q_OS_LINUX
    request.setHeader(QNetworkRequest::UserAgentHeader, qApp->applicationName() +" (by Liomka)/Linux");
#else
    request.setHeader(QNetworkRequest::UserAgentHeader, qApp->applicationName() +" (by Liomka)/UnknowPlatform");
#endif
    request.setRawHeader("X-BetaSeries-Version", "2.2");
    // Set the "BETASERIESAPIKEY" variable environment on your Shell or Qt creator.
    request.setRawHeader("X-BetaSeries-Key", qgetenv("BETASERIESAPIKEY"));

    return request;
}

QNetworkRequest BetaSeriesApi::createRequestWithUrl(QUrl url)
{
    QNetworkRequest request = createDefaultRequest();
    request.setUrl(url);
    return request;
}

void BetaSeriesApi::closeCurrentConnections()
{
    foreach (QNetworkReply *reply, _replyList) {
        reply->abort();
    }
}
