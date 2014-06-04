#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QtWidgets/QWidget>

#include <QtCore/QVariant>

class QPropertyAnimation;

class BetaSeriesApi;

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(BetaSeriesApi *api, QWidget *parent = 0);
    ~HomeWidget();

public slots:
    void refreshHomeReceived(QVariant feeds);
    void refreshUnseenSeries(QVariant series);

signals:
    void connectionPbClicked();

protected:
    // reimplement
    virtual void hideEvent(QHideEvent *hideEvent);
    virtual void showEvent(QShowEvent *showEvent);
    virtual void timerEvent(QTimerEvent *timerEvent);

private slots:
    void episodeWatched(double id);

    // Ui
    void on_newsGroupBox_toggled(bool toggled);
    void on_userLb_linkActivated(const QString &link);

private:
    Ui::HomeWidget *ui;

    BetaSeriesApi *_api;

    int _refreshNewsTimer;
    int _maximumNewsHeight;
    QPropertyAnimation *_newsSlideAnimation;
};

#endif // HOMEWIDGET_H
