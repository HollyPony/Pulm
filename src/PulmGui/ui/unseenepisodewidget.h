#ifndef UNSEENEPISODEWIDGET_H
#define UNSEENEPISODEWIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include <QtCore/QVariantMap>

class BetaSeriesApi;

//******************************************************************************
// ElidedLabel Class
//******************************************************************************
class ElidedLabel : public QLabel
{
    Q_OBJECT

protected:
    virtual void resizeEvent(QResizeEvent *event);

};

//******************************************************************************
// MarkAsButton Class
//******************************************************************************
class MarkAsButton : public QPushButton
{
    Q_OBJECT

public:
    explicit MarkAsButton(uint episodeId, QWidget *parent = 0);

public slots:
    void onClick();

signals:
    void episodeSeenClicked(uint);

private:
    uint _episodeId;

};

//******************************************************************************
// UnseenEpisodeWidget Class
//******************************************************************************
class UnseenEpisodeWidget: public QWidget
{
    Q_OBJECT

public:
    explicit UnseenEpisodeWidget(BetaSeriesApi *api, QVariantMap serie, QWidget *parent = 0);

    double currentId() {return _currentId;}

public slots:
    void animateNextEpisode();

signals:
    void markAsSeen(double);

protected:
    virtual void showEvent(QShowEvent *showEvent);

private slots:
    void setupNewCurrent();

private:
    QWidget *_makeWidget();

    BetaSeriesApi *_api;
    QWidget *_currentWidget;
    QWidget *_nextWidget;

    QVariantMap _serie;

    double _currentId;

};

#endif // UNSEENEPISODEWIDGET_H
