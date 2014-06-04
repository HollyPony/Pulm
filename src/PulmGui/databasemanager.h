#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <BetaSeriesApi.h>

class DatabaseManager : public BetaSeriesApi
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = 0);
    ~DatabaseManager();

    QByteArray getSeriePicture(double id);
    void getEpisodesList(uint limit = 0, uint showId = 0);

protected:
    virtual void picturesShowsEvent(uint showId, QByteArray image);

};

#endif // DATABASEMANAGER_H
