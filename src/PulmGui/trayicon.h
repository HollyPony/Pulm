#ifndef TRAYICON_H
#define TRAYICON_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include <QtWidgets/QSystemTrayIcon>

class BetaSeriesApi;
class TrayIconWidget;

#if defined Q_OS_MAC
class QActionGroup;
#endif

#if defined Q_OS_WIN
class QListWidget;
#endif

class TrayIcon : public QObject {

    Q_OBJECT

public:
    explicit TrayIcon(BetaSeriesApi *api, QObject *parent = 0);
    ~TrayIcon();

public slots:
    void hide();
    void show();

    void refreshUnseenSeries(QVariant series);
    void episodeWatched(double id);
    void userConnected();
    void userDisconnected();

signals:
    void connectionRequested();
    void showMainWindowRequested();

private slots:
    void trayTriggered(QSystemTrayIcon::ActivationReason reason);

private:
    BetaSeriesApi *_api;
    QSystemTrayIcon *_tray;

#if defined Q_OS_MAC
    QActionGroup *_unseenActionGroup;
#endif

#if defined Q_OS_WIN
    QListWidget *_unseenListWidget;
#endif
};

#endif // TRAYICON_H
