#include "help.h"
#include "ui_help.h"
#include <QResource>
#include <QDebug>

/**
 * @brief help::help Default constructor
 *        Load the index page into the help tab (according to language)
 */
help::help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::help)
{
    ui->setupUi(this);

    mLocaleName = QLocale::system().name();
    QString lang = mLocaleName.left(mLocaleName.indexOf("_"));

    QString rc = ":/help/help/index";

    QResource r(rc + "-" + lang);
    if ( ! r.isValid())
        r.setFileName(rc);

    if ( r.isValid() )
    {
        QString pageContent((char *)r.data());
        ui->textBrowser->setHtml(pageContent);
    }
}

/**
 * @brief help::~help Default destructor
 *        Clean memory
 */
help::~help()
{
    delete ui;
}
