#include "trayicon.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidgetAction>

#if defined Q_OS_MAC
#include <QtWidgets/QActionGroup>
#endif

#if defined Q_OS_WIN
#include <QtWidgets/QListWidget>
#endif

#include <betaseriesapi.h>

#include "ui/unseenepisodewidget.h"
#include "trayiconwidget.h"

#include <QDebug>

TrayIcon::TrayIcon(BetaSeriesApi *api, QObject *parent)
    : QObject(parent)
    , _api (api)
    , _tray (new QSystemTrayIcon(qApp))
{
    _tray->setIcon(QIcon(":/icons/logo"));
    _tray->setVisible(true);
    QMenu *trayMenu = new QMenu;
    trayMenu->setMinimumWidth(300);

// If QWidget Traymenu suported
#if defined Q_OS_WIN
    TrayIconWidget *trayIconWidget = new TrayIconWidget(trayMenu);
    _unseenListWidget = trayIconWidget->listWidget();
    QWidgetAction *trayIconWidgetAction = new QWidgetAction(trayMenu);
    trayIconWidgetAction->setDefaultWidget(trayIconWidget);
    trayMenu->addAction(trayIconWidgetAction);

    connect(trayIconWidget, SIGNAL(showMainWindowRequested()), this, SIGNAL(showMainWindowRequested()));
#endif
#if defined Q_OS_MAC
    trayMenu->addAction(tr("Open Pulm"));
    trayMenu->addSeparator();

    _unseenActionGroup = new QActionGroup(_tray);
    _unseenActionGroup->addAction(QIcon(":/icons/logo"), "Ep1");
    trayMenu->addActions(_unseenActionGroup->actions());
    trayMenu->addSeparator();

    QAction *action = new QAction(_tray);
    action->setText(tr("Quit"));
    trayMenu->addAction(action);
#endif


    _tray->setContextMenu(trayMenu);

    _tray->connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                   this, SLOT(trayTriggered(QSystemTrayIcon::ActivationReason)));

    // Connect Systray Events
    connect(_api, SIGNAL(loginSuccess()),
            this, SLOT(userConnected()), Qt::UniqueConnection);
    connect(_api, SIGNAL(unlogSuccess()),
            this, SLOT(userDisconnected()), Qt::UniqueConnection);
    connect(_api, SIGNAL(unseenSeriesUpdate(QVariant)),
            this, SLOT(refreshUnseenSeries(QVariant)), Qt::UniqueConnection);
    connect(_api, SIGNAL(episodeWatched(double)),
            this, SLOT(episodeWatched(double)), Qt::UniqueConnection);

    if (_api->isUserConnected())
        userConnected();
    else
        userDisconnected();
}

TrayIcon::~TrayIcon()
{
    if (_tray) {
        _tray->contextMenu()->deleteLater();
        _tray->hide();
        _tray->deleteLater();
        _tray = NULL;
    }
}

void TrayIcon::hide()
{
    _tray->hide();
}

void TrayIcon::show()
{
    _tray->show();
}

#if defined Q_OS_MAC
void TrayIcon::refreshUnseenSeries(QVariant series)
{
//    _unseenListWidget->setEnabled(true);
//    _unseenListWidget->clear();

//    foreach (QVariant serie, series.toList()) {
//        UnseenEpisodeWidget *unseenEpisodeWidget = new UnseenEpisodeWidget(
//                                                       _api,
//                                                       serie.toMap(),
//                                                       _unseenListWidget);

//        QListWidgetItem *episodeDetailsItem = new QListWidgetItem(_unseenListWidget);

//        episodeDetailsItem->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), unseenEpisodeWidget->sizeHint().height()));
//        episodeDetailsItem->setData(Qt::UserRole, unseenEpisodeWidget->currentId());
//        _unseenListWidget->setItemWidget(episodeDetailsItem, unseenEpisodeWidget);
//    }

//    if (_unseenListWidget->model()->rowCount() == 0) {
//        _unseenListWidget->clear();
//        QListWidgetItem *item = new QListWidgetItem(tr("You have seen all the episodes!"), _unseenListWidget);
//        item->setTextAlignment(Qt::AlignCenter);
//        item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
//        _unseenListWidget->setEnabled(false);
//    }
}
#endif

// If QWidget Traymenu supported
#if defined Q_OS_WIN
void TrayIcon::refreshUnseenSeries(QVariant series)
{
    _unseenListWidget->setEnabled(true);
    _unseenListWidget->clear();

    foreach (QVariant serie, series.toList()) {
        UnseenEpisodeWidget *unseenEpisodeWidget = new UnseenEpisodeWidget(
                                                       _api,
                                                       serie.toMap(),
                                                       _unseenListWidget);

        QListWidgetItem *episodeDetailsItem = new QListWidgetItem(_unseenListWidget);

        episodeDetailsItem->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), unseenEpisodeWidget->sizeHint().height()));
        episodeDetailsItem->setData(Qt::UserRole, unseenEpisodeWidget->currentId());
        _unseenListWidget->setItemWidget(episodeDetailsItem, unseenEpisodeWidget);
    }

    if (_unseenListWidget->model()->rowCount() == 0) {
        _unseenListWidget->clear();
        QListWidgetItem *item = new QListWidgetItem(tr("You have seen all the episodes!"), _unseenListWidget);
        item->setTextAlignment(Qt::AlignCenter);
        // TODO : Test again if this line is useful
//        item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
        _unseenListWidget->setEnabled(false);
    }
}
#endif

#if defined Q_OS_MAC
void TrayIcon::episodeWatched(double id)
{
}
#endif

// If QWidget Traymenu supported
#if defined Q_OS_WIN
void TrayIcon::episodeWatched(double id)
{
    QAbstractItemModel *model = _unseenListWidget->model();
    QModelIndexList matches = model->match(model->index(0, 0), Qt::UserRole, id);
    if (!matches.isEmpty()) {
        QModelIndex modelIndex = matches.first();
        QListWidgetItem *listWidgetItem = _unseenListWidget->item(modelIndex.row());
        UnseenEpisodeWidget *w = qobject_cast<UnseenEpisodeWidget*>(_unseenListWidget->itemWidget(listWidgetItem));
        w->animateNextEpisode();
        listWidgetItem->setData(Qt::UserRole, w->currentId());
        if (w->currentId() == -1) {
            _unseenListWidget->removeItemWidget(listWidgetItem);
            _unseenListWidget->takeItem(modelIndex.row());
        }
    }
    if (_unseenListWidget->model()->rowCount() == 0) {
        _unseenListWidget->clear();
        QListWidgetItem *item = new QListWidgetItem(tr("You have seen all the episodes!"), _unseenListWidget);
        item->setTextAlignment(Qt::AlignCenter);
//        item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
        _unseenListWidget->setEnabled(false);
    }
}
#endif

void TrayIcon::userConnected()
{
}

#if defined Q_OS_MAC
void TrayIcon::userDisconnected()
{
}
#endif

// If QWidget Trayicon supported
#if defined Q_OS_WIN
void TrayIcon::userDisconnected()
{
    _unseenListWidget->clear();
    // TODO Waybe for WIN
    _unseenListWidget->setMaximumHeight(44);

    QListWidgetItem *item = new QListWidgetItem(_unseenListWidget);
    QPushButton *connectButton = new QPushButton(_unseenListWidget);

    connectButton->setText(tr("Connection"));
    connectButton->connect(connectButton, SIGNAL(clicked()),
                           this, SIGNAL(connectionRequested()));

//    item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
    item->setTextAlignment(Qt::AlignCenter);

    _unseenListWidget->setItemWidget(item, connectButton);
}
#endif

void TrayIcon::trayTriggered(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        emit showMainWindowRequested();
    default:
        break;
    }
}
