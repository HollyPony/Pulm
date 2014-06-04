#ifndef BETASERIESAPI_H
#define BETASERIESAPI_H

#include "betaseriesapi_global.h"

#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslError>

class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;

class BETASERIESAPISHARED_EXPORT BetaSeriesApi : public QObject
{
    Q_OBJECT

    struct User {
        User()
            : isConnected(false)
            , inAccount(false)
            , id(0)
            , login("")
            , hash("")
            , token("") {}

        bool isConnected;
        bool inAccount;
        double id;
        QString login;
        QString hash;
        QString token;
    };

public:
    explicit BetaSeriesApi(QObject *parent = 0);
    ~BetaSeriesApi();

    bool isUserConnected();
    QString userLogin();

    void getUpdateFile();
#if defined Q_OS_MAC
    void getTimelineHome(uint nbpp = 8, double since_id = 0);
#else
    void getTimelineHome(uint nbpp = 10, double since_id = 0);
#endif
    void getEpisodesList(uint limit = 0, uint showId = 0);
    void getPicturesShows(double id);
    void getMembersInfos(double id = 0, bool summary = false);
    void getShowsEpisodes(double id, double season = 0, double episode = 0);
    void getCommentsShow(double id, int limit = 10);
    void getEpisodesDisplay(double id);

public slots:
    void postMembersAuth(QString login, QString password);
    void postMembersDestroy();
    void postEpisodesWatched(uint id, int note = 0);

signals:
    // Success
    void loginSuccess();
    void unlogSuccess();
    void memberInfosReceived(QVariantMap);
    void timelineHomeReceived(QVariant);
    void unseenSeriesUpdate(QVariant);
    void picturesShowsReceived(uint, QByteArray);
    void episodeWatched(double);
    void showEpisodesReceived(long, QVariantList);
    void commentsEpisodeReceived(QVariantList);
    void commentsShowReceived(QVariantList);
    void commentsMemberReceived(QVariantList);
    void commentsMovieReceived(QVariantList);
    void episodeDisplayReceived(QVariantMap);

    // Errors
    void missingApiKey();
    void invalidType();
    void invalidAction();
    void invalidToken();
    void privacyConflict();
    void serieAlreadyAdded();
    void serieNotInAccount();
    void episodeUnseen();
    void notFriends();
    void invalidUserOptions();
    void missingVar();
    void atLeast2char();
    void mustBeDigit();
    void incorrectValue();
    void unauthorizedChar();
    void invalidMail();
    void invalidSerie();
    void unknownUser();
    void badPassword();
    void userAlreadyExist();

protected:
    virtual void loginSuccessEvent();
    virtual void unlogSuccessEvent();
    virtual void memberInfosEvent(QVariantMap infos);
    virtual void timelineHomeEvent(QVariant timelineHome);
    virtual void unseenSeriesEvent(QVariant unseenSeries);
    virtual void picturesShowsEvent(uint showId, QByteArray image);
    virtual void episodeWatchedEvent(double episodeId);
    virtual void showEpisodesEvent(long showId, QVariantList episodes);
    virtual void commentsEpisodeEvent(QVariantList comments);
    virtual void commentsShowEvent(QVariantList comments);
    virtual void commentsMemberEvent(QVariantList comments);
    virtual void commentsMovieEvent(QVariantList comments);
    virtual void episodeDisplayEvent(QVariantMap infos);

private slots:
    void onResult(QNetworkReply *reply);
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void onSslErrors(QNetworkReply *reply, QList<QSslError> errors);

    QNetworkRequest createDefaultRequest();
    QNetworkRequest createRequestWithUrl(QUrl url);

private:
    void closeCurrentConnections();

    QNetworkAccessManager *_networkManager;
    User _connectedUser;

    QList<QNetworkReply *> _replyList; // TODO Maybe switch for QSet / QVector
};

#endif // BETASERIESAPI_H
