#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtWidgets/QDialog>

class QAbstractButton;

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void applyPreferences();
    void savePreferences();
    void restorePreferences();

signals:
    void hideSystray();
    void showSystray();

private slots:
    void on_displaySystrayCb_stateChanged(int state);
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::PreferencesDialog *ui;

};

#endif // PREFERENCESDIALOG_H
