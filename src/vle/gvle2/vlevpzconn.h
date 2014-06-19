/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef VLEVPZCONN_H
#define VLEVPZCONN_H
#include <QString>
#include <QPolygon>

// Declare classes to avoid recursive inclusion problem
class vleVpzModel;
class vleVpzPort;

class vleVpzConn
{
public:
    enum ConnType { Internal, In, Out };

public:
    vleVpzConn(vleVpzModel *model = 0);
    ~vleVpzConn();
    void setType(ConnType nType);
    ConnType type();
    QString  typeName();
    void setSource(vleVpzModel *model, QString portName);
    void setSource(vleVpzPort  *port);
    void setDestination(vleVpzModel *model, QString portName);
    void setDestination(vleVpzPort *port);
    vleVpzModel *getSource();
    vleVpzModel *getDestination();
    QString getSourceModelName();
    QString getSourcePortName();
    QString getDestinationModelName();
    QString getDestinationPortName();
    bool isValid();
    void route();
    bool isLinkedWith(vleVpzModel *model);
    int  getMaxX() { return mMaxX; }
    void setHighlight(bool hl = true);
    bool isHighlighted();

public:
    QPolygon mLines;
private:
    ConnType     mType;
    vleVpzModel *mParent;
    vleVpzModel *mSource;
    vleVpzPort  *mSourcePort;
    vleVpzPort  *mDestPort;
    int          mMaxX;
    int          mMaxY;
    bool         mHighlight;
};

#endif // VLEVPZCONN_H
