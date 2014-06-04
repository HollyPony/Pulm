#ifndef RESIZEFILTER_H
#define RESIZEFILTER_H

#include <QtWidgets/QTableWidget>

class ResizeTabWidgetFilter : public QObject
{
public:
    explicit ResizeTabWidgetFilter(QTabWidget *target);

    bool eventFilter(QObject *object, QEvent *event);

private:
    QTabWidget *target;

};

#endif // RESIZEFILTER_H
