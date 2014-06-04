#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class TrayIcon;

namespace Ui {
class MainWindow;
}

class DatabaseManager;
class TrayIcon;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void writePositionSettings();
    void readPositionSettings();

public slots:
    void showConnectMemberDialog();

protected:
    // reimplemented
    virtual void moveEvent(QMoveEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void connected();
    void disconnected();
    void restoreWindow();
    void createSystray();
    void deleteSystray();

    // Ui
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_actionQuit_triggered();
    void on_actionPreferences_triggered();
    void on_actionAbout_Serizer_triggered();
    void on_actionAbout_BetaSeries_triggered();
    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;

//    BetaSeriesApi *_betaSeriesApi;
    DatabaseManager *_databaseManager;
    TrayIcon *_trayIcon;

};

#endif // MAINWINDOW_H
