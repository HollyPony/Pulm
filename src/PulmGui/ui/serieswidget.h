#ifndef SERIESWIDGET_H
#define SERIESWIDGET_H

#include <QtWidgets/QWidget>

#include <QtCore/QVariantMap>

class BetaSeriesApi;
class QTreeWidgetItem;
class QSortFilterProxyModel;
class QStandardItemModel;

namespace Ui {
class SeriesWidget;
}

class ShowsWidget : public QWidget
{
    Q_OBJECT

    enum SHOWS_LIST_COLUMNS {
        COLUMN_SHOWS = 0,
        COLUMN_PERCENT = 1
    };

    enum TREE_INFOS {
        ID_ROLE = Qt::UserRole,
        EPISODE_SEEN_ROLE
    };

public:
    explicit ShowsWidget(BetaSeriesApi *api, QWidget *parent = 0);
    ~ShowsWidget();

protected:
    // reimplement
    virtual void hideEvent(QHideEvent *hideEvent);
    virtual void showEvent(QShowEvent *showEvent);

private slots:
    void updateShowList(QVariantMap memberInfos);
    void updateEpisodesList(long showId, QVariantList episodesList);
    void updateShowComments(QVariantList commentsList);
    void updateEpisodeDetails(QVariantMap episodeDetails);
    void showsListCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void episodesTreeCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);

    // Ui
    void on_browseShows_clicked();

private:
    Ui::SeriesWidget *ui;

    BetaSeriesApi *_api;
    QSortFilterProxyModel *_filterProxyModel;
    QStandardItemModel    *_showListModel;
    QStandardItemModel    *_episodeTreeModel;
};

#endif // SERIESWIDGET_H
