#include "vle/gvle2/gvle2win.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QtDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool result;

    std::string vlePrefixDir = vle::utils::Path::path().getPrefixDir();
    std::string localesPath =
        vle::utils::Path::path().buildDirname(vlePrefixDir,
                                              VLE_SHARE_DIRS,"translations");

    QTranslator qtTranslator;
    result = qtTranslator.load("gvle2_" + QLocale::system().name() + ".qm", localesPath.c_str());
    if (result == false)
    {
        qDebug() << "Load Translator : " <<
          QLocale::system().name() << " not found " <<
          "here : " << localesPath.c_str();
    }
    a.installTranslator(&qtTranslator);

    GVLE2Win w;

    w.show();

    result = a.exec();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
