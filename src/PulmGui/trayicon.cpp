#include "trayicon.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidgetAction>

#include <QtWidgets/QListWidget>

#include <betaseriesapi.h>

#include "ui/unseenepisodewidget.h"
#include "trayiconwidget.h"

#include <QDebug>

TrayIcon::TrayIcon(BetaSeriesApi *api, QObject *parent)
    : QObject(parent)
    , _api (api)
    , _tray (new QSystemTrayIcon(qApp))
    , _trayIconWidget (new TrayIconWidget)
    , _unseenListWidget (_trayIconWidget->listWidget())
{
    _tray->setIcon(QIcon(":/icons/logo"));
    _tray->setVisible(true);
    QMenu *trayMenu = new QMenu;
    trayMenu->setMinimumWidth(300);

    trayMenu->addAction("Test");
    QWidgetAction *acc = new QWidgetAction(trayMenu);
    QLabel *label = new QLabel("okok");
    acc->setDefaultWidget(label);
    trayMenu->addAction(acc);
    label->show();

    // Test Ui Mode
    QWidgetAction *trayIconWidgetAction = new QWidgetAction(trayMenu);
    trayIconWidgetAction->setDefaultWidget(_trayIconWidget);
    trayMenu->addAction(trayIconWidgetAction);

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

void TrayIcon::refreshUnseenSeries(QVariant series)
{
    qDebug() << "TrayIcon::refreshUnseenSeries";
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
        item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
        _unseenListWidget->setEnabled(false);
    }
}

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

void TrayIcon::userConnected()
{
}

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
    qDebug() << "TrayIcon::userDisconnected" << _unseenListWidget->sizeHint();

    _unseenListWidget->setItemWidget(item, connectButton);
}

void TrayIcon::trayTriggered(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        emit showMainWindowRequested();
    default:
        break;
    }
}
