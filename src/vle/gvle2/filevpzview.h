/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef FILEVPZVIEW_H
#define FILEVPZVIEW_H

#include <QGraphicsScene>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "ui_filevpzrtool.h"
#include "vlevpz.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#define ROW_NAME 0
#define ROW_DYN  1

namespace Ui {
class fileVpzView;
class fileVpzRTool;
}

class fileVpzView : public QWidget
{
    Q_OBJECT

public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondParameter };

public:
    explicit fileVpzView(QWidget *parent = 0);
    ~fileVpzView();
    void setVpz(vleVpz *v);
    vleVpz* vpz();
    bool isUsed(int *reason);
    void usedBySim(bool isUsed);
    void save();
    QWidget *getTool();
    void treeInsertModel(vleVpzModel *model, QTreeWidgetItem *base);
    void diagSelectModel(vleVpzModel *base);
    void treeUpdateModel(vleVpzModel *model, QString oldName, QString newName);
    void condUpdateTree(QList <vpzExpCondValue *> *valueList, QList<QTreeWidgetItem *> *widList);
    void condParamTreeSelectByValue(vpzExpCondValue *reqValue, QTreeWidgetItem *base);
    void condValueShowDetail(vpzExpCondValue *value);

public slots:
    void onTreeModelSelected();
    void onViewTreeMenu(const QPoint pos);
    void onFocusChanged(vleVpzModel *model);
    void onModelDblClick(vleVpzModel *model);
    void onPropertyChanged(QTableWidgetItem *item);
    void onConditionTreeSelected();
    void onConditionTreeMenu(const QPoint pos);
    void onConditionChanged(QTreeWidgetItem *item, int column);
    void onCondParamTreeSelected();
    void onCondParamTreeMenu(const QPoint pos);
    void onCondParamListDblClick(QListWidgetItem *item);
    void onCondParamListMenu(const QPoint pos);
    void onParamTableChange();
    void onCondParamTableDblClick(int row, int col);
    void onCondParamTableMenu(const QPoint pos);
    void onParamTextChange();
    void onCondParamCancel();
    void onCondParamSave();

private:
    void paramTreeUpdateList(vpzExpCondValue *value);
    void paramTreeUpdateTable(vpzExpCondValue *value);
    void paramTreeUpdateText(vpzExpCondValue *value);

private:
    bool             mUseSim;
    Ui::fileVpzView *ui;
    Ui::fileVpzRtool*uiTool;
    vleVpz          *mVpz;
    QWidget         *mWidgetTool;
    QList<QTreeWidgetItem *> mViewsItems;
    vleVpzModel     *mCurrentModel;
private:
    QTreeWidgetItem *mCondEdition;
    QMenu           *mCondTreeContextMenu;
private:
    QGraphicsScene   mScene;
};

#endif // FILEVPZVIEW_H
