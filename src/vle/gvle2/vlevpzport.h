/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef VLEVPZPORT_H
#define VLEVPZPORT_H

#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QWidget>
#include "vlevpz.h"
#include "vlevpzconn.h"

class vleVpzPort : public QWidget
{
    Q_OBJECT
public:
    enum portType { TypeUnknown, TypeInput, TypeOutput };
public:
    explicit vleVpzPort(QWidget *parent = 0);
    ~vleVpzPort();
    vleVpzModel *getModel();
    QString  getName(QString *name = 0);
    void     setName(QString name);
    void     setType(portType type);
    portType getType();
    int      getPosY();
    void     select(bool status);
    vleVpzConn* getConn();
    void        setConn(vleVpzConn *conn);
    void        unConn(vleVpzConn *conn);
    bool        isConnected(vleVpzConn *conn);
    void        edit(bool doEdit = true);

public slots:
    void contextMenu(const QPoint & pos);
    void editEnd();

protected:
    void hideEvent  (QHideEvent *event);
    void showEvent  (QShowEvent *event);
    void enterEvent (QEvent     *event);
    void leaveEvent (QEvent     *event);
    void mousePressEvent (QMouseEvent *event);
    void updLabel();
private:
    vleVpzModel *getTopModel();

private:
    QString  mName;
    portType mType;
    bool     mIsSelected;
    vleVpzModel *mModel;
    QList <vleVpzConn *> mConnections;
    int      mSavedPosX;

    QLabel   mIcon;
    QLabel   mTitle;
    QLineEdit mEdit;
};

#endif // VLEVPZPORT_H
