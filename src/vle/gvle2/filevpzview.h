#ifndef FILEVPZVIEW_H
#define FILEVPZVIEW_H

#include <QWidget>

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

namespace Ui {
class fileVpzView;
}

class fileVpzView : public QWidget
{
    Q_OBJECT

public:
    explicit fileVpzView(QWidget *parent = 0);
    ~fileVpzView();
    void setVpz(vle::vpz::Vpz *v)
    {
        mVpz = v;
    }
    vle::vpz::Vpz * vpz()
    {
        return mVpz;
    }
    bool isUsed()
    {
        return (mUsed != 0);
    }
    void used(bool isUsed)
    {
        if (isUsed)
            mUsed++;
        else
            mUsed--;
    }

private:
    int              mUsed;
    Ui::fileVpzView *ui;
    vle::vpz::Vpz   *mVpz;
};

#endif // FILEVPZVIEW_H
