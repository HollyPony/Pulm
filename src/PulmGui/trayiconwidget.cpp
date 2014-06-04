#include "trayiconwidget.h"
#include "ui_trayiconwidget.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QPaintEvent>

#include <QDebug>

// TODO Merge with trayIcon.cpp and .mm

TrayIconWidget::TrayIconWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MacTrayIcon)
{
    ui->setupUi(this);

    setVisible(false);

    qDebug() << "TrayIconWidget::TrayIconWidget" << ui->listWidget->sizeHint();
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
