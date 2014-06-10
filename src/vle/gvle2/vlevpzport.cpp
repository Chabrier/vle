/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "vlevpzport.h"
#include "vlevpz.h"
#include <QFont>
#include <QMenu>
#include <QObject>
#include <QtDebug>

/**
 * @brief vleVpzPort::vleVpzPort
 *        Default constructor for port object
 *
 */
vleVpzPort::vleVpzPort(QWidget *parent) :
    QWidget(parent)
{
    mType  = TypeUnknown;
    mModel = 0;
    mConnections.clear();
    mIsSelected = false;

    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT  (contextMenu(const QPoint&)));


    QPixmap pix (":/icon/resources/vpz_inport.png");

    mIcon.setParent(this);
    mIcon.setPixmap(pix);
    mIcon.resize(pix.width(), pix.height());
    mIcon.hide();

    mTitle.setParent(this);
    mTitle.hide();

    mEdit.setParent(this);
    mEdit.hide();
    mEdit.setStyleSheet("background-color: rgb(255, 255, 127);");

    // Configure the font
    QFont titleFont;
    titleFont.setPointSize(6);
    mTitle.setFont(titleFont);
    mEdit.setFont(titleFont);

    updLabel();
}

/**
 * @brief vleVpzPort::~vleVpzPort
 *        Default destructor
 *
 */
vleVpzPort::~vleVpzPort()
{
    // Nothing to do
}

/**
 * @brief vleVpzPort::getModel
 *        Get the model that own the port
 *
 */
vleVpzModel *vleVpzPort::getModel()
{
    vleVpzModel *pm = (vleVpzModel *)parent();
    return pm;
}

/**
 * @brief vleVpzPort::getName
 *        Getter for the "name" property
 *
 */
QString vleVpzPort::getName(QString *name)
{
    if (name)
    {
        name->clear();
        name->append(mName);
    }
    return mName;
}

/**
 * @brief vleVpzPort::setName
 *        Set a name for the port
 *
 */
void vleVpzPort::setName(QString name)
{
    // Save the specified name
    mName = name;

    // Update the label with the name
    mTitle.setText(name);

    // Update the label size, according to text size
    QFontMetrics fm = QFontMetrics(mTitle.font());
    int ww = fm.width(mName);
    mTitle.resize(ww, 10);

    updLabel();
}

/**
 * @brief vleVpzPort::setType
 *        Set the port type (input ou output)
 *
 */
void vleVpzPort::setType(portType type)
{
    mType = type;
    updLabel();
}

/**
 * @brief vleVpzPort::getType
 *        Get the current port type (input or output)
 *
 */
vleVpzPort::portType vleVpzPort::getType()
{
    return mType;
}

/**
 * @brief vleVpzPort::getConn
 *        Get the first connection associated with the port
 *
 */
vleVpzConn* vleVpzPort::getConn()
{
    return mConnections.at(0);
}

/**
 * @brief vleVpzPort::setConn
 *        Set the connection associated with the port
 *
 */
void vleVpzPort::setConn(vleVpzConn *conn)
{
    if (conn == 0)
        return;

    if (mConnections.length())
        qDebug() << this->getModel()->getName() << "::" << this->mName << " multiple conn";

    if ( ! mConnections.contains(conn))
        mConnections.append(conn);
    else
        qDebug() << this->getModel()->getName() << "::" << this->mName << " duplicate conn " << mConnections.length();
}

/**
 * @brief vleVpzPort::unConn
 *        Remove the specified connection
 *
 */
void vleVpzPort::unConn(vleVpzConn *conn)
{
    if (conn == 0)
        return;
    if ( ! mConnections.contains(conn))
        return;

    mConnections.removeOne(conn);
}

/**
 * @brief vleVpzPort::isConnected
 *        Test if the specified connection is used by the port
 *
 */
bool vleVpzPort::isConnected(vleVpzConn *conn)
{
    return mConnections.contains(conn);
}

/**
 * @brief vleVpzPort::getPosY
 *        Get the Y position of the widget into the model
 *        (for compatibility with previous port implementation)
 *
 */
int vleVpzPort::getPosY()
{
    QPoint p = pos();
    return p.y() + 2;
}

/**
 * @brief vleVpzPort::updLabel
 *        Update title and icon position and visibility, according to port type
 *
 */
void vleVpzPort::updLabel()
{
    if (mType == TypeInput)
    {
        mIcon.move(0, 0);
        mTitle.move(mIcon.width() + 2, 0);
    }
    else if (mType == TypeOutput)
    {
        mIcon.move(mTitle.width() + 2, 0);
        mTitle.move(0, 0);
    }

    if (mType != TypeUnknown)
    {
        mIcon.show();
        mTitle.show();
    }

    int maxHeight = mIcon.height();
    if (mTitle.height() > maxHeight)
        maxHeight = mTitle.height();
    resize(mIcon.width() + mTitle.width() + 4, maxHeight);
}

/**
 * @brief vleVpzPort::select
 *        Called when the port is selected/unselected
 *
 */
void vleVpzPort::select(bool status)
{
    mIsSelected = status;

    if (status)
        mTitle.setStyleSheet("color: rgb(255, 0, 0);");
    else
        mTitle.setStyleSheet("color: rgb(0, 0, 255);");
}

/**
 * @brief vleVpzPort::edit
 *        Show/hide an editable version of port (user update the name)
 *
 */
void vleVpzPort::edit(bool doEdit)
{
    int maxWidth;
    maxWidth = getModel()->width() - 4;

    if (doEdit)
    {
        mTitle.hide();
        mEdit.setText(mName);
        mEdit.show();

        setStyleSheet("background-color: rgb(255, 255, 127);");

        resize(maxWidth, mEdit.height());
        mEdit.resize(maxWidth - mIcon.width(), mEdit.height());
        mSavedPosX = x();
        if (mType == TypeOutput)
        {
            move(2, y());
            mIcon.move(width() - mIcon.width(), 0);
        }
        else if (mType == TypeInput)
        {
            mEdit.move(mIcon.width(), 0);
            //move(2, y());
        }
    }
    else
    {
        setStyleSheet("");

        move(mSavedPosX, y());
        mEdit.hide();
        if (mEdit.isModified())
            setName(mEdit.text());
        else
            updLabel();
    }

    mEdit.setFocus();
    QObject::connect(&mEdit, SIGNAL(editingFinished()),
                     this,   SLOT  (editEnd()) );

}

/**
 * @brief vleVpzPort::editEnd (slot)
 *        Called at the end of edition (return pressed or lost focus)
 *
 */
void vleVpzPort::editEnd()
{
    edit(false);
}

/**
 * @brief vleVpzPort::hideEvent (extends QWidget)
 *        Called when widget is hidden
 *
 */
void vleVpzPort::hideEvent (QHideEvent *event)
{
    QWidget::hideEvent(event);
}

/**
 * @brief vleVpzPort::showEvent (extends QWidget)
 *        Called when widget is displayed
 *
 */
void vleVpzPort::showEvent (QShowEvent * event)
{
    QWidget::showEvent(event);

    if (mType != TypeUnknown)
        mIcon.show();
    if (mName.size())
        mTitle.show();
}

/**
 * @brief vleVpzPort::enterEvent (extends QWidget)
 *        Called when the mouse come over the widget rect
 *
 */
void vleVpzPort::enterEvent (QEvent *event)
{
    QWidget::enterEvent(event);

    if (! mIsSelected)
        mTitle.setStyleSheet("color: rgb(0, 127, 0);");
}

/**
 * @brief vleVpzPort::leaveEvent (extends QWidget)
 *        Called when the mouse exit the widget
 *
 */
void vleVpzPort::leaveEvent (QEvent *event)
{
    QWidget::leaveEvent(event);

    if (! mIsSelected)
        mTitle.setStyleSheet("color: rgb(0, 0, 255);");
}

/**
 * @brief vleVpzPort::mousePressEvent (extends QWidget)
 *        Called when widget receive a mouse-click event
 *
 */
void vleVpzPort::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

/**
 * @brief vleVpzPort::contextMenu
 *        Called when a context menu is requested (right click on widget)
 *
 */
void vleVpzPort::contextMenu(const QPoint & pos)
{
    getModel()->contextMenu(pos, this);
}
