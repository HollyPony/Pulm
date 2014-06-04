#include "systemtrayclasstest.h"

#include <QDebug>

SystemTrayClassTest::SystemTrayClassTest(QObject *parent) :
    QSystemTrayIcon(parent)
{
//    setContextMenu(new TrayI);
}

void SystemTrayClassTest::show()
{
    qDebug() << "Show";
}

void SystemTrayClassTest::showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint)
{
    qDebug() << "Message Showned" << title << message;
}
