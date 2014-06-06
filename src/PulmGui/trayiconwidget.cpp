#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QPaintEvent>

#include <QDebug>

TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MacTrayIcon)
{
    ui->setupUi(this);

    setVisible(false);
}

TrayIconWidget::~TrayIconWidget()
{
    delete ui;
}

QListWidget *TrayIconWidget::listWidget()
{
    return ui->listWidget;
}

void TrayIconWidget::on_btnQuit_clicked()
{
    qApp->quit();
}
