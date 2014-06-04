#include "resizefilter.h"

#include <QtCore/QEvent>

ResizeTabWidgetFilter::ResizeTabWidgetFilter(QTabWidget *target)
    : QObject(target), target(target)
{
}

bool ResizeTabWidgetFilter::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        uint size = target->size().width() / target->count();
        target->setStyleSheet(QString("QTabWidget#"+ object->objectName() +
                                      " > QTabBar::tab { width: %1px; } ")
                              .arg(size));
    }

    return false;
}
