#include <QCoreApplication>
#include "iqscreenspy.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("iqScreenSpy");
    QCoreApplication::setOrganizationName("itQuasar");


    IQScreenSpy spy;
    spy.start();
    
    return a.exec();
}
