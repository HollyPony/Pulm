#include "unseenepisodewidget.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

#include <QtGui/QResizeEvent>

#include <QtCore/QPropertyAnimation>
#include <QtCore/QParallelAnimationGroup>

#include <betaseriesapi.h>

//******************************************************************************
// ElidedLabel Class
//******************************************************************************
void ElidedLabel::resizeEvent(QResizeEvent *event)
{
    // TODO prevent when label is longer than is parent
//    qDebug() << "Label resize" << event->size();

//    QString text("some long text without elipsis");
//    QFontMetrics metrics(label->font());
//    QString elidedText = metrics.elidedText(text, Qt::ElideRight, label->width());
//    label->setText(elidedText);

    QWidget::resizeEvent(event);
}


//******************************************************************************
// MarkAsButton Class
//******************************************************************************
MarkAsButton::MarkAsButton(uint episodeId, QWidget *parent)
    : QPushButton(parent)
    , _episodeId(episodeId)
{
    setText(tr("Seen"));
    connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void MarkAsButton::onClick()
{
    setEnabled(false);
    emit episodeSeenClicked(_episodeId);
}

//******************************************************************************
// UnseenEpisodeWidget Class
//******************************************************************************
UnseenEpisodeWidget::UnseenEpisodeWidget(BetaSeriesApi *api, QVariantMap serie, QWidget *parent)
    : QWidget(parent)
    , _api(api)
    , _currentWidget(0)
    , _nextWidget(0)
    , _serie(serie)
    , _currentId(0)
{
    _currentWidget = _makeWidget();
    _currentWidget->setEnabled(true);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(_currentWidget);
    setLayout(mainLayout);
}

void UnseenEpisodeWidget::showEvent(QShowEvent *showEvent)
{
    if (!_currentWidget) {
        QPoint finalPos = _currentWidget->pos();
        QPropertyAnimation *slideIn = new QPropertyAnimation(_currentWidget, "pos", this);
        slideIn->setDuration(600);
        slideIn->setStartValue(QPoint(finalPos.x() + width(), finalPos.y()));
        slideIn->setEndValue(finalPos);
        slideIn->setEasingCurve(QEasingCurve::OutQuart);
        slideIn->start(QAbstractAnimation::DeleteWhenStopped);

        slideIn->connect(slideIn, SIGNAL(finished()), this, SLOT(setupNewCurrent()));
    }
    QWidget::showEvent(showEvent);
}

void UnseenEpisodeWidget::animateNextEpisode()
{
    _currentWidget->setEnabled(false);
    _nextWidget = _makeWidget();

    if (_nextWidget) {
        layout()->addWidget(_nextWidget);

        _currentWidget->setMinimumWidth(_currentWidget->width());
        _nextWidget->setMinimumWidth(_currentWidget->width());

        QPoint finalPos = _currentWidget->pos();
        int duration = 600;

        QPropertyAnimation *slideOut = new QPropertyAnimation(_currentWidget, "pos", this);
        slideOut->setDuration(duration);
        slideOut->setStartValue(finalPos);
        slideOut->setEndValue(QPoint(finalPos.x() - _currentWidget->width(), finalPos.y()));
        slideOut->setEasingCurve(QEasingCurve::OutQuart);

        QPropertyAnimation *slideIn = new QPropertyAnimation(_nextWidget, "pos", this);
        slideIn->setDuration(duration);
        slideIn->setStartValue(QPoint(finalPos.x() + _currentWidget->width(), finalPos.y()));
        slideIn->setEndValue(finalPos);
        slideIn->setEasingCurve(QEasingCurve::OutQuart);

        QParallelAnimationGroup *group = new QParallelAnimationGroup(_currentWidget);
        group->addAnimation(slideOut);
        group->addAnimation(slideIn);

        group->start(QAbstractAnimation::DeleteWhenStopped);
        group->connect(group, SIGNAL(finished()), this, SLOT(setupNewCurrent()));
    }
}

void UnseenEpisodeWidget::setupNewCurrent()
{
    if (_currentWidget)
        _currentWidget->deleteLater();

    if (_nextWidget)
        _currentWidget = _nextWidget;

    if (_currentWidget) {
        _currentWidget->setEnabled(true);
        _currentWidget->setMaximumWidth(QWIDGETSIZE_MAX);
        _currentWidget->setMinimumWidth(0);
    }
    _nextWidget = 0;
}

QWidget *UnseenEpisodeWidget::_makeWidget()
{
    if (_serie["unseen"].toList().isEmpty()) {
        _currentId = -1;
        return 0; // TODO DO IT RIGHT !!!
    }

    QVariantList unseenEpisodes = _serie["unseen"].toList();
    QVariantMap episode = unseenEpisodes.takeFirst().toMap();
    _serie["unseen"] = unseenEpisodes;
    QString episodeTitle = episode["title"].toString();
    QString episodeCode  = episode["code"].toString();
    _currentId = episode["id"].toDouble();

    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    QLabel *episodeLabel = new QLabel(widget);
    episodeLabel->setWordWrap(true);
    episodeLabel->setText(_serie["title"].toString() + " - " +
            episodeCode + " - " +
            episodeTitle);

    layout->addWidget(episodeLabel);

    MarkAsButton *button = new MarkAsButton(_currentId, widget);
    connect(button, SIGNAL(episodeSeenClicked(uint)),
            _api, SLOT(postEpisodesWatched(uint)));

    layout->addWidget(button);

    layout->setStretch(0, 1);
    widget->setLayout(layout);
    widget->setEnabled(false);
    return widget;
}
