#include "trayicon.h"

#import <Cocoa/Cocoa.h>

#include <QtWidgets/QListWidget>

#include <QLayout>

#include <betaseriesapi.h>

#import "macmenu.h"

#include "ui/unseenepisodewidget.h"
#include "trayiconwidget.h"

#include <QMacCocoaViewContainer>

#include <QDebug>

TrayIcon::TrayIcon(BetaSeriesApi *api, QObject *parent)
    : QObject(parent)
    , _api(api)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    _trayIconWidget = new TrayIconWidget;
    _unseenListWidget = _trayIconWidget->listWidget();

    MacMenu *menulet = [[MacMenu alloc]
                       initWithView:reinterpret_cast<NSView *>(_trayIconWidget->winId())];

//    _trayIconWidget->setAttribute(Qt::WA_DontShowOnScreen);
//    _trayIconWidget->setAttribute(Qt::WA_PaintOnScreen);
    _trayIconWidget->setWindowFlags(Qt::FramelessWindowHint);

//    QMacCocoaViewContainer *qmenuLet = new QMacCocoaViewContainer(menulet);

//    _trayIconWidget->setParent(qmenuLet);
    _trayIconWidget->show();

    [pool release];

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
}

void TrayIcon::hide()
{
}

void TrayIcon::show()
{
    _trayIconWidget->repaint();
}

void TrayIcon::refreshUnseenSeries(QVariant series)
{
    qDebug() << "TrayIcon::refreshUnseenSeries";
    _unseenListWidget->setEnabled(true);
    _unseenListWidget->clear();

    foreach (QVariant serie, series.toList()) {
        UnseenEpisodeWidget *unseenEpisodeWidget = new UnseenEpisodeWidget(
                                                       _api
                                                       ,
                                                       serie.toMap()
                                                       , _unseenListWidget);

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
        item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
        _unseenListWidget->setEnabled(false);
    }
}

void TrayIcon::userConnected()
{
}

void TrayIcon::userDisconnected()
{
    _unseenListWidget->clear();

    QListWidgetItem *item = new QListWidgetItem(_unseenListWidget);
    QPushButton *connectButton = new QPushButton(_unseenListWidget);

    connectButton->setText(tr("Connection"));
    connectButton->connect(connectButton, SIGNAL(clicked()),
                           this, SIGNAL(connectionRequested()));

    item->setSizeHint(QSize(_unseenListWidget->sizeHint().width(), 40));
    item->setTextAlignment(Qt::AlignCenter);
//    qDebug() << "TrayIcon::userDisconnected" << _unseenListWidget->sizeHint();

    _unseenListWidget->setItemWidget(item, connectButton);
}
