#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>

#include <QtWidgets/QApplication>

#include "ui/mainwindow.h"

#include <qupd_updater.h>

static void applyTranslation() {
    QTranslator qtTranslator;
    if (qtTranslator.load("qt_"+ QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtTranslator);

    QTranslator appTranslator;
    if (appTranslator.load(":/tr/" + QLocale::system().name()))
        qApp->installTranslator(&appTranslator);
}

int main(int argc, char *argv[])
{
    qputenv("QT_MESSAGE_PATTERN", "[%{type}] (%{file}:%{line}) - %{message}");

    QSettings::setDefaultFormat(QSettings::NativeFormat);
    QApplication::setApplicationName("Serizer");
    QApplication::setApplicationVersion("0.1");
    QApplication::setOrganizationName("Liomka");
    QApplication::setOrganizationDomain("liomka.users.sourceforge.net");

    QApplication app(argc, argv);

    if (!QUPDUpdater::instance()->isUpdateRequested()) {
        app.setQuitOnLastWindowClosed(false);

        applyTranslation();

        MainWindow *window = new MainWindow;

//        QUPDUpdater::instance()->setParentWidget(window);
//        QUPDUpdater::instance()->setFeedURL("https://sourceforge.net/projects/serizer/files/Serizer.xml/download");
//        QUPDUpdater::instance()->checkForUpdates(false);
    }
    return app.exec();
}
