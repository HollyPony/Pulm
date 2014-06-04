#include "memberconnectdialog.h"
#include "ui_memberconnectdialog.h"

#include <QtWidgets/QPushButton>

#include <QtGui/QDesktopServices>

#include <betaseriesapi.h>

MemberConnectDialog::MemberConnectDialog(BetaSeriesApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MemberConnectDialog)
    , _api(api)
{
    ui->setupUi(this);
    if (parent->isVisible())
        setWindowFlags(Qt::Sheet);

    connect(_api, SIGNAL(missingVar()), SLOT(_loginError()));
    connect(_api, SIGNAL(unknownUser()), SLOT(_loginError()));
    connect(_api, SIGNAL(badPassword()), SLOT(_loginError()));
    connect(_api, SIGNAL(loginSuccess()), SLOT(accept()));

    _connectionButton = new QPushButton(tr("Connect"));
    QPushButton *_registerButton = new QPushButton(tr("Register a BetaSeries account"));

    ui->buttonBox->addButton(_connectionButton, QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(_registerButton, QDialogButtonBox::ActionRole);

    connect(_connectionButton, SIGNAL(clicked()), this, SLOT(_tryToConnect()));
    connect(_registerButton, SIGNAL(clicked()), this, SLOT(_registerToBetaseries()));
}

MemberConnectDialog::~MemberConnectDialog()
{
    delete ui;
}

void MemberConnectDialog::_tryToConnect()
{
    _connectionButton->setEnabled(false);
    _api->postMembersAuth(ui->loginLe->text(), ui->passwdLe->text());
}

void MemberConnectDialog::_loginError()
{
    ui->passwdLe->clear();
    ui->errorLbl->setText("<span style=\"color:red;\">"+ tr("Wrong login") +"</span>");
    _connectionButton->setEnabled(true);
}

void MemberConnectDialog::_registerToBetaseries()
{
    // TODO Make custom register window
    QDesktopServices::openUrl(QUrl("https://www.betaseries.com/inscription"));
}
