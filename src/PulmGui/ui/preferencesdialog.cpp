#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QtCore/QSettings>

#include <QDebug>

#include "qupd_ignoredversions.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    if (parent && parent->isVisible())
        setWindowFlags(Qt::Sheet);

    ui->filePath->setText(QSettings().fileName());

    restorePreferences();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::applyPreferences()
{
    if (ui->displaySystrayCb->isChecked()) emit showSystray();
    else                                   emit hideSystray();

    if (ui->skipLastVersionBp->isChecked())
        QSettings().remove(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY);
}

void PreferencesDialog::savePreferences()
{
    applyPreferences();
    QSettings().setValue("showMainWindowOnLaunch", ui->mainWindowOnLaunchCb->isChecked());
    QSettings().setValue("displayTrayIcon", ui->displaySystrayCb->isChecked());
}

void PreferencesDialog::restorePreferences()
{
    ui->displaySystrayCb->setChecked(QSettings().value("displayTrayIcon", true).toBool());
    ui->mainWindowOnLaunchCb->setChecked(QSettings().value("showMainWindowOnLaunch", true).toBool());

    bool lastVersionIsSkipped = QSettings().contains(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY);
    ui->skipLastVersionBp->setEnabled(lastVersionIsSkipped);
    ui->z_skipLastVersion->setEnabled(lastVersionIsSkipped);
    QString lastSkippedVersion = ":";
    if (lastVersionIsSkipped) {
        lastSkippedVersion = QSettings().value(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY, "").toString();
        lastSkippedVersion = " ("+ lastSkippedVersion +") :";
    }
    ui->z_skipLastVersion->setText(tr("Skip the last version %1").arg(lastSkippedVersion));
}

void PreferencesDialog::on_displaySystrayCb_stateChanged(int state)
{
    switch (state) {
    case Qt::Checked:
        ui->mainWindowOnLaunchCb->setEnabled(true);
        ui->z_mainWindowOnLaunch->setEnabled(true);
        break;
    case Qt::PartiallyChecked:
    case Qt::Unchecked:
        ui->mainWindowOnLaunchCb->setChecked(true);
        ui->mainWindowOnLaunchCb->setEnabled(false);
        ui->z_mainWindowOnLaunch->setEnabled(false);
        break;
    }
}

void PreferencesDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::ApplyRole:
        savePreferences();
        break;
    case QDialogButtonBox::ResetRole:
        restorePreferences();
        break;
    case QDialogButtonBox::RejectRole:
        break;
    case QDialogButtonBox::NRoles: // Number of roles
        qDebug() << "NRoles";
        break;
    case QDialogButtonBox::YesRole:
    case QDialogButtonBox::NoRole:
    case QDialogButtonBox::HelpRole:
    case QDialogButtonBox::ActionRole:
    case QDialogButtonBox::DestructiveRole:
    case QDialogButtonBox::InvalidRole:
        break;
//    default:
//        ui->buttonBox->clicked(button);
    }
}
