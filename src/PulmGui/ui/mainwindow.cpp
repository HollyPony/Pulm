#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QDebug>
#include <QtCore/QSettings>

//#include <betaseriesapi.h>

#include "trayicon.h"
#include "databasemanager.h"

#include "ui/memberconnectdialog.h"
#include "ui/homewidget.h"
#include "ui/serieswidget.h"
#include "ui/resizefilter.h"
#include "ui/preferencesdialog.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , _betaSeriesApi(new BetaSeriesApi(this))
    , _databaseManager(new DatabaseManager(this))
    , _trayIcon(new TrayIcon(_databaseManager, this))
{
    ui->setupUi(this);
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setVisible(false);

    if (QSettings().value("autoconnect", false).toBool()) {
        ;
    } else {
        ui->actionConnect->setEnabled(true);
    }

    // TODO UNIFORMIZATION !!!!
#if defined Q_OS_MAC
    ui->centralWidget->layout()->setContentsMargins(0, 15, 0, 0);
#else
    ui->tabWidget->installEventFilter(new ResizeTabWidgetFilter(ui->tabWidget));
    ui->centralWidget->setStyleSheet("QTabWidget#tabWidget::pane {border-top: 0px;}"
                                     "QTabWidget#tabWidget > QTabBar::tab {height:35px;font-size:12pt;}");
#endif

    HomeWidget *homeWidget = new HomeWidget(_databaseManager, this);
    ShowsWidget *showsWidget = new ShowsWidget(_databaseManager, this);

    ui->tabWidget->addTab(homeWidget  , tr("Home"));
    ui->tabWidget->addTab(showsWidget, tr("My Series"));

    homeWidget->connect(homeWidget, SIGNAL(connectionPbClicked())
                        , this, SLOT(showConnectMemberDialog()));


    connect(_databaseManager, SIGNAL(loginSuccess()),
            this, SLOT(connected()));
    connect(_databaseManager, SIGNAL(unlogSuccess()),
            this, SLOT(disconnected()));

    if (QSettings().value("displayTrayIcon", true).toBool()) {
        createSystray();
    }

    connect(_databaseManager, SIGNAL(unseenSeriesUpdate(QVariant))
            , homeWidget, SLOT(refreshUnseenSeries(QVariant)));
    connect(_databaseManager, SIGNAL(timelineHomeReceived(QVariant))
            , homeWidget, SLOT(refreshHomeReceived(QVariant)));
    connect(_databaseManager, SIGNAL(episodeWatched(double))
            , homeWidget, SLOT(episodeWatched(double)));

    readPositionSettings();

//    if (!TrayIcon::isInstantiated() || QSettings().value("showMainWindowOnLaunch", true).toBool())
//        show();

    if (QSettings().value("showMainWindowOnLaunch", true).toBool())
        show();
}

MainWindow::~MainWindow()
{
    deleteSystray();
    delete ui;
}

void MainWindow::writePositionSettings()
{
    QSettings().setValue( "geometry", saveGeometry() );
    QSettings().setValue( "savestate", saveState() );
    QSettings().setValue( "maximized", isMaximized() );

    if ( !isMaximized() ) {
        QSettings().setValue( "pos", pos() );
        QSettings().setValue( "size", size() );
    }
}

void MainWindow::readPositionSettings()
{
    restoreGeometry(QSettings().value( "geometry", saveGeometry() ).toByteArray());
    restoreState(QSettings().value( "savestate", saveState() ).toByteArray());
    move(QSettings().value( "pos", pos() ).toPoint());
    resize(QSettings().value( "size", size() ).toSize());
    if ( QSettings().value( "maximized", isMaximized() ).toBool() )
        showMaximized();
}

void MainWindow::showConnectMemberDialog()
{
    MemberConnectDialog *connectionDialog = new MemberConnectDialog(_databaseManager, this);
    connectionDialog->show();
}

void MainWindow::createSystray() {
//    if (!TrayIcon::isInstantiated()) {
        _trayIcon->show();

        connect(_trayIcon, SIGNAL(connectionRequested()),
                this, SLOT(showConnectMemberDialog()), Qt::UniqueConnection);
        connect(_trayIcon, SIGNAL(showMainWindowRequested()),
                this, SLOT(restoreWindow()));
//    }
}

void MainWindow::deleteSystray()
{
//    if (TrayIcon::isInstantiated()) {
//        TrayIcon::resetInstance();
//    } else {
//        qDebug() << "Trying delete _tray but not initialized";
//    }
}

void MainWindow::moveEvent( QMoveEvent* event)
{
    writePositionSettings();
    QMainWindow::moveEvent(event);
}

void MainWindow::resizeEvent( QResizeEvent* event)
{
    writePositionSettings();
    QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writePositionSettings();
    QWidget::closeEvent(event);
}

void MainWindow::connected()
{
    ui->actionConnect->setVisible(false);
    ui->actionDisconnect->setVisible(true);
    _databaseManager->getEpisodesList(); // Do it in SQL Local DB
    if (isVisible()) {
        ui->tabWidget->currentWidget()->hide();
        ui->tabWidget->currentWidget()->show();
    }
}

void MainWindow::disconnected()
{
    ui->actionConnect->setVisible(true);
    ui->actionDisconnect->setVisible(false);
    if (isVisible()) {
        ui->tabWidget->currentWidget()->hide();
        ui->tabWidget->currentWidget()->show();
    }
}

void MainWindow::restoreWindow()
{
    if (this->isHidden())
        this->show();
    if (this->isMinimized())
        this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
}

// Ui on_widget_action slots----------------------------------------------------
void MainWindow::on_actionConnect_triggered()
{
    showConnectMemberDialog();
}

void MainWindow::on_actionDisconnect_triggered()
{
    _databaseManager->postMembersDestroy();
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog *preferences = new PreferencesDialog(this);
    connect(preferences, SIGNAL(hideSystray()),
            this, SLOT(deleteSystray()));
    connect(preferences, SIGNAL(showSystray()),
            this, SLOT(createSystray()));
    preferences->show();
}

void MainWindow::on_actionAbout_Serizer_triggered()
{

}

void MainWindow::on_actionAbout_BetaSeries_triggered()
{

}

void MainWindow::on_actionAbout_Qt_triggered()
{
    qApp->aboutQt();
}
