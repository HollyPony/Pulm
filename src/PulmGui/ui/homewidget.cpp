#include "homewidget.h"
#include "ui_homewidget.h"

#include <QtCore/QSettings>
#include <QtCore/QPropertyAnimation>

#include <betaseriesapi.h>

#include "unseenepisodewidget.h"

//******************************************************************************
// HomeWidget Class
//******************************************************************************
HomeWidget::HomeWidget(BetaSeriesApi *api, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget)
    , _api(api)
    , _refreshNewsTimer(-1)
    , _maximumNewsHeight(0)
    , _newsSlideAnimation(0)
{
    ui->setupUi(this);
    _maximumNewsHeight = ui->newsTextBrowser->maximumHeight();

    if (!QSettings().value("showNews").toBool()) {
        ui->newsGroupBox->setChecked(false);
        ui->newsTextBrowser->setMaximumHeight(0);
    }
}

HomeWidget::~HomeWidget()
{
    if (_newsSlideAnimation)
        _newsSlideAnimation->deleteLater();
    delete ui;
}

void HomeWidget::refreshHomeReceived(QVariant feeds)
{
    QString site = "https://www.betaseries.com";

    ui->newsTextBrowser->clear();
    foreach (QVariant feed, feeds.toList()) {
        QVariantMap feedMap = feed.toMap();

        QString event;
        event += "<span";
        event += "><a href=\"" + site + "/membre/" + feedMap["user"].toString() + "\">"+ feedMap["user"].toString() +"</a> ";
        event += feedMap["html"].toString().replace("href=\"", "href=\"" + site);
        event += " <span style=\"color:#C0C0C0; font-size:45%;\">" + feedMap["date"].toDateTime().toString("ddd M. MMM hh:mm") + "</style>";
        event += "</span>";

        ui->newsTextBrowser->append(event);
    }
}

void HomeWidget::refreshUnseenSeries(QVariant series)
{
    ui->unseenSeriesList->clear();

    foreach (QVariant serie, series.toList()) {
        UnseenEpisodeWidget *unseenEpisodeWidget = new UnseenEpisodeWidget(
                                                       _api
                                                       , serie.toMap()
                                                       , ui->unseenSeriesList);
        QListWidgetItem *episodeDetailsItem = new QListWidgetItem(ui->unseenSeriesList);

        // TODO Use safe method for size row
        episodeDetailsItem->setSizeHint(unseenEpisodeWidget->sizeHint());
        episodeDetailsItem->setData(Qt::UserRole, unseenEpisodeWidget->currentId());
        ui->unseenSeriesList->setItemWidget(episodeDetailsItem, unseenEpisodeWidget);
    }
}

void HomeWidget::hideEvent(QHideEvent *hideEvent)
{
    if (_newsSlideAnimation) {
        _newsSlideAnimation->deleteLater();
        _newsSlideAnimation = 0;
    }

    if (_refreshNewsTimer != -1) {
        killTimer(_refreshNewsTimer);
        _refreshNewsTimer = -1;
    }

    QWidget::hideEvent(hideEvent);
}

void HomeWidget::showEvent(QShowEvent *showEvent)
{
    _newsSlideAnimation = new QPropertyAnimation(ui->newsTextBrowser, "maximumHeight", this);

    if (_api->isUserConnected()) {
        // Init
        ui->userLb->setText(tr("%1Disconnect%2")
                            .arg(_api->userLogin() + " (<a href=\"disconnect\">")
                            .arg("</a>)"));

        // Show new
        ui->userLb->show();
        ui->unseenSeriesList->setEnabled(true);
    } else {
        // Hide old
        ui->unseenSeriesList->clear();
        ui->unseenSeriesList->setEnabled(false);

        // Init
        ui->userLb->setText(tr("%1Connect%2").arg("<a href=\"connect\">").arg("</a>"));
    }

    if (ui->newsGroupBox->isChecked()) {
        _api->getTimelineHome();
        _refreshNewsTimer = startTimer(8000);
    }

    QWidget::showEvent(showEvent);
}

void HomeWidget::timerEvent(QTimerEvent *timerEvent)
{
    if (timerEvent->timerId() == _refreshNewsTimer) {
        _api->getTimelineHome();
    }
}

void HomeWidget::episodeWatched(double id)
{
    QAbstractItemModel *model = ui->unseenSeriesList->model();
    QModelIndexList matches = model->match(model->index(0, 0), Qt::UserRole, id);
    if (!matches.isEmpty()) {
        QModelIndex modelIndex = matches.first();
        QListWidgetItem *listWidgetItem = ui->unseenSeriesList->item(modelIndex.row());
        UnseenEpisodeWidget *w = qobject_cast<UnseenEpisodeWidget*>(ui->unseenSeriesList->itemWidget(listWidgetItem));
        w->animateNextEpisode();
        listWidgetItem->setData(Qt::UserRole, w->currentId());
        if (w->currentId() == -1) {
            ui->unseenSeriesList->removeItemWidget(listWidgetItem);
            ui->unseenSeriesList->takeItem(modelIndex.row());
        }
    }
}

void HomeWidget::on_newsGroupBox_toggled(bool toggled)
{
    QSettings().setValue("showNews", toggled);
    if (toggled) {
        _api->getTimelineHome();
        _refreshNewsTimer = startTimer(8000);
    } else if (_refreshNewsTimer != -1) {
        killTimer(_refreshNewsTimer);
        _refreshNewsTimer = -1;
    }

    if (_newsSlideAnimation) {
        _newsSlideAnimation->setDirection((QAbstractAnimation::Direction)toggled);
        _newsSlideAnimation->setDuration(400);
        _newsSlideAnimation->setStartValue(_maximumNewsHeight);
        _newsSlideAnimation->setEndValue(0);
        _newsSlideAnimation->start();
    }
}

void HomeWidget::on_userLb_linkActivated(const QString &link)
{
    if (link == "disconnect")
        _api->postMembersDestroy();
    else if (link == "connect")
        emit connectionPbClicked();
}
