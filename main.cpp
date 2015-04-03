#include <QApplication>
#include "iqscreenspy.h"
#include <QCommandLineParser>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("iqScreenSpy");
    QCoreApplication::setOrganizationName("itQuasar");
    QCoreApplication::setApplicationVersion("1.0.0");

    QSettings settings;
    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription(QCoreApplication::translate("main", "Application to automatically record video from your screen.\n\n"
                                                                                    "This application allows you to automatically record video from the screen. "
                                                                                    "It automatically backs up and saw rotates recorded.\n\n"
                                                                                    "To configure application edit file \"%0\".")
                                                .arg(settings.fileName()));
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    commandLineParser.process(app);

    IqScreenSpy spyApp;
    spyApp.start();
    
    return app.exec();
}
