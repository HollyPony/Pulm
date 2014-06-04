#include "serieswidget.h"
#include "ui_serieswidget.h"

#include <QtCore/QSortFilterProxyModel>

#include <QtGui/QStandardItemModel>

#include <betaseriesapi.h>

ShowsWidget::ShowsWidget(BetaSeriesApi *api, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SeriesWidget)
    , _api (api)
    , _showListModel(new QStandardItemModel(this))
    , _episodeTreeModel(new QStandardItemModel(this))
{
    ui->setupUi(this);

    // ShowList
    _showListModel->setColumnCount(2);

    _filterProxyModel = new QSortFilterProxyModel(ui->showsList);
    _filterProxyModel->setSourceModel(_showListModel);
    _filterProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->showsList->setModel(_filterProxyModel);
    if (ui->showsList->horizontalHeader()) {
        ui->showsList->horizontalHeader()->setSectionResizeMode(COLUMN_SHOWS  , QHeaderView::Stretch);
        ui->showsList->horizontalHeader()->setSectionResizeMode(COLUMN_PERCENT, QHeaderView::ResizeToContents);
    }

    // EpisodesTree
    ui->episodeTree->setModel(_episodeTreeModel);
}

ShowsWidget::~ShowsWidget()
{
    delete ui;
}

void ShowsWidget::hideEvent(QHideEvent *hideEvent)
{
    disconnect(_api, 0, this, 0);
    disconnect(ui->filterLineEdit, 0, this, 0);
    disconnect(ui->showsList->selectionModel(), 0, this, 0);
    disconnect(ui->episodeTree->selectionModel(), 0, this, 0);

    _episodeTreeModel->clear();
    _showListModel->clear();
    ui->commentsShowBrowser->clear();

    QWidget::hideEvent(hideEvent);
}

void ShowsWidget::showEvent(QShowEvent *showEvent)
{
    if (_api->isUserConnected()) {
        ui->showsBox->setEnabled(true);

        _showListModel->clear();
        _episodeTreeModel->clear();
        ui->commentsShowBrowser->clear();

        // Connect api
        connect(_api, SIGNAL(memberInfosReceived(QVariantMap)),
                this, SLOT(updateShowList(QVariantMap)));
        connect(_api, SIGNAL(showEpisodesReceived(long, QVariantList)),
                this, SLOT(updateEpisodesList(long, QVariantList)));
        connect(_api, SIGNAL(commentsShowReceived(QVariantList)),
                this, SLOT(updateShowComments(QVariantList)));
        connect(_api, SIGNAL(episodeDisplayReceived(QVariantMap)),
                this, SLOT(updateEpisodeDetails(QVariantMap)));

        // Connect Ui
        connect(ui->filterLineEdit, SIGNAL(textChanged(QString)),
                _filterProxyModel, SLOT(setFilterRegExp(QString)));
        connect(ui->showsList->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(showsListCurrentRowChanged(QModelIndex,QModelIndex)));
        connect(ui->episodeTree->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(episodesTreeCurrentRowChanged(QModelIndex, QModelIndex)));

        _api->getMembersInfos();
    } else {
        ui->showsBox->setEnabled(false);
        ui->episodesBox->setEnabled(false);
        ui->detailsBox->setEnabled(false);
    }

    QWidget::showEvent(showEvent);
}

void ShowsWidget::updateShowList(QVariantMap memberInfos)
{
    ui->showsBox->setEnabled(true);

    _showListModel->setRowCount(0);

    foreach (QVariant show, memberInfos["shows"].toList()) {
        QVariantMap showMap = show.toMap();
        double  showId    = showMap["id"].toDouble();
        QString showTitle = showMap["title"].toString();
        QVariantMap user  = showMap["user"].toMap();

        QList<QStandardItem *> row;
        QStandardItem *col1 = new QStandardItem;
        col1->setText(showTitle);
        col1->setData(showId, ID_ROLE);
        QStandardItem *col2 = new QStandardItem;
        col2->setText(user["status"].toString() + "%");

        row << col1 << col2;
        _showListModel->appendRow(row);
    }

    ui->showsList->horizontalHeader()->setSectionResizeMode(COLUMN_SHOWS  , QHeaderView::Stretch);
    ui->showsList->horizontalHeader()->setSectionResizeMode(COLUMN_PERCENT, QHeaderView::ResizeToContents);
}

void ShowsWidget::updateEpisodesList(long showId, QVariantList episodesList)
{
    int row = ui->showsList->selectionModel()->currentIndex().row();
    QModelIndex currentIndexSelected = _showListModel->index(row, 0);

    if (currentIndexSelected.data(ID_ROLE).toDouble() == showId) {
        ui->episodesBox->setEnabled(true);

        QMap<int, QStandardItem *> itemMap;
        foreach(QVariant episode, episodesList) {
            QVariantMap episodeMap = episode.toMap();

            double episodeId     = episodeMap["id"].toDouble();
            QString episodeTitle = episodeMap["title"].toString();
            int episodeNumber    = episodeMap["episode"].toInt();
            double episodeSeason = episodeMap["season"].toDouble();
            bool userSeenEpisode = episodeMap["user"].toMap()["seen"].toBool();

            if (itemMap[episodeSeason] == 0) {
                itemMap[episodeSeason] = new QStandardItem(tr("Season %1").arg(episodeSeason));
                _episodeTreeModel->appendRow(itemMap[episodeSeason]);
            }

            QStandardItem *episodeItem = new QStandardItem(QString::number(episodeNumber) + " - " + episodeTitle);
            episodeItem->setCheckable(true);
            episodeItem->setData(episodeId, ID_ROLE);
            episodeItem->setData(userSeenEpisode, EPISODE_SEEN_ROLE);
            if (userSeenEpisode) {
                episodeItem->setCheckState(Qt::Checked);
            } else {
                episodeItem->setCheckState(Qt::Unchecked);
            }

            itemMap[episodeSeason]->appendRow(episodeItem);
        }
    }
}

void ShowsWidget::updateShowComments(QVariantList commentsList)
{
    QString site = "https://www.betaseries.com";
    ui->commentsShowBrowser->clear();

    foreach(QVariant comment, commentsList) {
        QVariantMap commentMap = comment.toMap();

        QString event;
        event += "<span>";
        event += "<a href=\"" + site + "/membre/" + commentMap["login"].toString() + "\">"+ commentMap["login"].toString() +"</a> - ";
        event += commentMap["text"].toString();
        event += " <span style=\"color:#C0C0C0; font-size:45%;\">" + commentMap["date"].toDateTime().toString("ddd M. MMM hh:mm") + "</style>";
        event += "</span>";

        ui->commentsShowBrowser->append(event);
    }
}

void ShowsWidget::updateEpisodeDetails(QVariantMap episodeDetails)
{
    ui->detailsBox->setEnabled(true);

    ui->episodeLbl->setText(episodeDetails["code"].toString() +" - "+ episodeDetails["title"].toString());
    ui->descBrowser->setText(episodeDetails["description"].toString());
}

void ShowsWidget::showsListCurrentRowChanged( const QModelIndex &current, const QModelIndex &/*previous*/)
{
    QModelIndex index = _showListModel->index(current.row(), 0);

    ui->episodesBox->setEnabled(false);
    ui->detailsBox->setEnabled(false);

    _episodeTreeModel->clear();
    ui->commentsShowBrowser->clear();

    if (index.isValid()) {
        double showIdSelected = index.data(ID_ROLE).toDouble();
        _api->getShowsEpisodes(showIdSelected);
        _api->getCommentsShow(showIdSelected);
    }
}

void ShowsWidget::episodesTreeCurrentRowChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if (current.isValid()) {
        if (current.parent().isValid()) {
            QVariant episodeId = _episodeTreeModel->itemFromIndex(current)->data(ID_ROLE);
            if (episodeId != QVariant::Invalid) {
                _api->getEpisodesDisplay(episodeId.toDouble());
            }
        } else {
            ui->episodeTree->collapseAll();
            ui->episodeTree->expand(current);
        }
    }
}

void ShowsWidget::on_browseShows_clicked()
{
}
