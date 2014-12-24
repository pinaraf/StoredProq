#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMetaProperty>

void mapRecordToQObject(const QSqlRecord &record, QObject *target)
{
    const QMetaObject *metaObject = target->metaObject();

    for (int i = 0 ; i < record.count() ; i++)
    {
        int propIdx = metaObject->indexOfProperty(record.fieldName(i).toLocal8Bit());
        if (propIdx >= 0)
        {
            QMetaProperty prop = metaObject->property(propIdx);
            prop.write(target, record.value(i));
        }
    }
}

template <typename T>
class SqlQueryResultMapper
{
public:
    SqlQueryResultMapper() {
        metaObject = T::staticMetaObject;
    }

    T* map(QSqlQuery *query)
    {
        T *result = new T();
        query->next();
        QSqlRecord rec = query->record();

        mapRecordToQObject(rec, result);
        return result;
    }

private:
    QMetaObject metaObject;
};

template <>
class SqlQueryResultMapper<int>
{
public:
    int map(QSqlQuery *query)
    {
        query->next();
        QSqlRecord rec = query->record();
        //Q_ASSERT(rec.count() == 1);
        return rec.value(0).toInt();
    }
};

template <>
class SqlQueryResultMapper<QList<int>>
{
public:
    QList<int> map(QSqlQuery *query)
    {
        QList<int> result;
        //Q_ASSERT(query->record().count() == 1);
        while (query->next())
        {
            result << query->value(0).toInt();
        }
        return result;
    }
};

template <typename T>
class SqlQueryResultMapper<QList<T*>>
{
public:
    SqlQueryResultMapper() {
        metaObject = T::staticMetaObject;
    }

    QList<T*> map(QSqlQuery *query)
    {
        QList<T*> resultList;
        while (query->next()) {
            T *result = new T();
            QSqlRecord rec = query->record();

            mapRecordToQObject(rec, result);
            resultList.append(result);
        }
        return resultList;
    }

private:
    QMetaObject metaObject;
};

#endif // QUERYRESULT_H

