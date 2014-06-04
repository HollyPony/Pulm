#ifndef MEMBERCONNECTDIALOG_H
#define MEMBERCONNECTDIALOG_H

#include <QtWidgets/QDialog>

class QPushButton;

class BetaSeriesApi;

namespace Ui {
class MemberConnectDialog;
}

class MemberConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MemberConnectDialog(BetaSeriesApi *api, QWidget *parent = 0);
    ~MemberConnectDialog();

private slots:
    void _tryToConnect();
    void _loginError();
    void _registerToBetaseries();

private:
    Ui::MemberConnectDialog *ui;

    BetaSeriesApi *_api;

    QPushButton *_connectionButton; // TODO Why ?
};

#endif // MEMBERCONNECTDIALOG_H
