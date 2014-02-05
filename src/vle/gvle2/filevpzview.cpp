#include "filevpzview.h"
#include "ui_filevpzview.h"

fileVpzView::fileVpzView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileVpzView)
{
    ui->setupUi(this);
    mVpz = 0;
    mUsed = 0;
}

fileVpzView::~fileVpzView()
{
    delete ui;
    if (mVpz)
        delete mVpz;
}
