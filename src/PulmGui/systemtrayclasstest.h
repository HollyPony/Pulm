#ifndef SYSTEMTRAYCLASSTEST_H
#define SYSTEMTRAYCLASSTEST_H

#include <QSystemTrayIcon>

class SystemTrayClassTest : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit SystemTrayClassTest(QObject *parent = 0);

signals:

public slots:
    void show();
    void showMessage(const QString & title
                     , const QString & message
                     , QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information
                     , int millisecondsTimeoutHint = 10000);

};

#endif // SYSTEMTRAYCLASSTEST_H
