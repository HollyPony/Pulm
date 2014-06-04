#ifndef MACTRAYICON_H
#define MACTRAYICON_H

#include <QWidget>

namespace Ui {
class MacTrayIcon;
}

class QListWidget;

class TrayIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayIconWidget(QWidget *parent = 0);
    ~TrayIconWidget();

    QListWidget *listWidget();

private slots:
    void on_btnQuit_clicked();

private:
    Ui::MacTrayIcon *ui;
};

#endif // MACTRAYICON_H
