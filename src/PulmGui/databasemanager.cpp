#include "databasemanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

DatabaseManager::DatabaseManager(QObject *parent)
    : BetaSeriesApi(parent)
{
    if (!QSqlDatabase::contains()) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

        db.setDatabaseName("database.sqlite");

        if (!db.open()) {
            qErrnoWarning("test qErrno");
            qCritical() << "SQLError:" << db.lastError().text();
            // TODO raise/exception manager
            return;
        }

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS T_SERIES_IMAGES"
                   "( IMG_ID   INTEGER"
                   ", IMG_DATA BLOB"
                   ", PRIMARY KEY (IMG_ID))");

        if (query.lastError().isValid())
            qCritical() << query.lastError().text();
    }
}

DatabaseManager::~DatabaseManager()
{
}

QByteArray DatabaseManager::getSeriePicture(double id)
{
    qDebug() << "DatabaseManager::getSeriePicture" << id;
    QSqlQuery query;
    query.exec("SELECT IMG_DATA FROM T_SERIES_IMAGES"
               " WHERE"
               " IMG_ID = '" + QString::number(id) + "';");

    query.next();

    if (query.lastError().isValid() || query.isNull(0)) {
        if (query.lastError().isValid())
            qCritical() << id << query.lastError().text();
        BetaSeriesApi::getPicturesShows(id);
        return NULL;
    } else {
        return query.value(0).toByteArray();
    }
}

/**
 * @brief DatabaseManager::getEpisodesList
 * @param limit
 * @param showId
 */
void DatabaseManager::getEpisodesList(uint limit, uint showId) {
    QSqlQuery query;
    query.exec("SELECT * FROM T_SERIES_SHOWS");
    BetaSeriesApi::getEpisodesList(limit, showId);
}

//------------------------------------------------------------------------------
// OVERRIDE API
//------------------------------------------------------------------------------
void DatabaseManager::picturesShowsEvent(uint showId, QByteArray image)
{
    QSqlQuery query;
    query.prepare("INSERT INTO T_SERIES_IMAGES"
                  "( IMG_ID, IMG_DATA )"
                  " VALUES"
                  " (?, ?)");
    query.addBindValue(showId);
    query.addBindValue(image);
    query.exec();

    if (query.lastError().isValid())
        qCritical() << query.lastError().text();
    else
        BetaSeriesApi::picturesShowsEvent(showId, image);
}
