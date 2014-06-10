#ifndef VPZEXPCOND_H
#define VPZEXPCOND_H
#include <QList>
#include <QString>
#include <QDomNode>

class vleVpz;
class vleVpzModel;
class vpzExpCond;

class vpzExpCondValue
{
public:
    enum ValueType { TypeUnknown, TypeBoolean, TypeDouble, TypeInteger, TypeNull, TypeString,
                     TypeComplex,
                   TypeMap, TypeMatrix, TypeSet, TypeTable, TypeTuple, TypeXml };

public:
    vpzExpCondValue(vpzExpCondValue *parent = 0);
    ~vpzExpCondValue();
    ValueType type();
    void      setType(ValueType type);
    QString   getTypeName();
    bool      isAtomic();
    void      setName(QString name);
    QString   getName();
    QString   getDisplayName();
    QString   getDisplayValue();
    void      addObject(vpzExpCondValue *obj);
    void      delObject(vpzExpCondValue *obj);
    bool      getBoolean();
    void      setBoolean(bool b);
    double    getDouble();
    void      setDouble(double v);
    int       getInteger();
    void      setInteger(int i);
    QString   getString();
    void      setString(QString text);

    // Accessors for complex types
    vpzExpCondValue *getMapLast();
    QList <vpzExpCondValue *> *getMap()
    {
        return &mMap;
    }
    QList <vpzExpCondValue *> *getMatrix();
    void      getMatrixSize (int *row, int *col);
    void      setMatrixSize (int  row, int  col);
    QList <vpzExpCondValue *> *getSet()
    {
        return &mSet;
    }
    QList <vpzExpCondValue *> *getTable();
    void      setTableSize (int  row, int  col);
    void      getTableSize (int *row, int *col);
    void      setTableIndex(int  row, int  col);
    void      getTableIndex(int *row, int *col);
    QList <vpzExpCondValue *> *getTuple();
    int       getTupleIndex();
    void      setTupleIndex(int idx);
    QString   getXml();
    void      setXml(QString xml);

public:
    void    xLoadTable(const QDomNode *node);
    bool    xSave(QDomDocument *doc, QDomElement *baseNode, vpzExpCondValue *value = 0);

public: // For debug only
    QString   mTmpName;
private:
    vpzExpCondValue *mParent;
    ValueType  mType;
    QString    mName;
    bool       mBoolean;
    double     mDouble;
    int        mInteger;
    QString    mString;
    int        mSizeRow;
    int        mSizeCol;
    QList <vpzExpCondValue *> mMap;
    QList <vpzExpCondValue *> mMatrix;
    QList <vpzExpCondValue *> mSet;
    QList <vpzExpCondValue *> mTable;
    QList <vpzExpCondValue *> mTuple;
    int        mTableRow;
    int        mTableCol;
    int        mTupleIndex;
    QString    mXml;
};

class vpzExpCondPort
{
public:
    vpzExpCondPort(vpzExpCond *parent = 0);
    ~vpzExpCondPort();
    QString getName();
    void    setName(QString name);
    vpzExpCond *getParent();
    void    setParent(vpzExpCond *parent);
    void    xLoadNode(const QDomNode &baseNode, vpzExpCondValue *baseValue = 0);
    void    xLoadMatrix(QDomNode *baseNode, vpzExpCondValue *baseValue = 0);
    bool    xSave(QDomDocument *doc, QDomElement *baseNode);
    vpzExpCondValue* createValue(vpzExpCondValue::ValueType type);
    bool    removeValue(vpzExpCondValue *value);
    QList <vpzExpCondValue *> *getValues()
    {
        return &mValues;
    }
private:
    vpzExpCond *mParent;
    QString     mName;
    QList <vpzExpCondValue *> mValues;
};

class vpzExpCond
{
public:
    vpzExpCond(vleVpz *vpz = 0);
    ~vpzExpCond();
    QString getName();
    void    setName(QString name);
    void    xLoadNode(const QDomNode &node);
    void    addPort(vpzExpCondPort *newPort);
    bool    removePort(vpzExpCondPort *port);
    QList <vpzExpCondPort *> *getPorts()
    {
        return &mPorts;
    }
    void registerModel(vleVpzModel *model);
    void unRegisterModel(vleVpzModel *model);

private:
    QString mName;
    QList <vpzExpCondPort *> mPorts;
    QList <vleVpzModel *>    mLinkedModels;
};

#endif // VPZEXPCOND_H
