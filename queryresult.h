#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMetaProperty>

#include <tuple>

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

template<typename T>
inline std::tuple<T> mapRecordToTuple(const QSqlRecord &record, int position)
{
    return std::make_tuple<T>(record.value(position).value<T>());
}

template<typename T, typename... Args>
inline
typename std::enable_if<sizeof...(Args), std::tuple<T, Args...>>::type
 mapRecordToTuple(const QSqlRecord &record, int position)
{
    return std::tuple_cat(std::make_tuple<T>(record.value(position).value<T>()), mapRecordToTuple<Args...>(record, position + 1));
}

template <typename T>
class SqlQueryResultMapper
{
public:
    SqlQueryResultMapper() {
    }

    template <typename R = typename std::remove_pointer<T>::type>
    typename std::enable_if<std::is_base_of<QObject, R>::value, R*>::type
    map(QSqlQuery *query)
    {
        T result = new R();
        query->next();
        QSqlRecord rec = query->record();

        mapRecordToQObject(rec, result);
        return result;
    }
};

template <>
class SqlQueryResultMapper<QDateTime>
{
public:
    QDateTime map(QSqlQuery *query)
    {
        query->next();
        QSqlRecord rec = query->record();
        //Q_ASSERT(rec.count() == 1);
        return rec.value(0).toDateTime();
    }
};

template <typename ...Args>
class SqlQueryResultMapper<std::tuple<Args...>>
{
public:
    std::tuple<Args...> map(QSqlQuery *query)
    {
        query->next();
        QSqlRecord rec = query->record();
        //Q_ASSERT(rec.count() == 1);
        return mapRecordToTuple<Args...>(rec, 0);
    }
};

template <typename ...Args>
class SqlQueryResultMapper<QList<std::tuple<Args...>>>
{
public:
    QList<std::tuple<Args...>> map(QSqlQuery *query)
    {
        QList<std::tuple<Args...>> result;
        while (query->next())
        {
            QSqlRecord rec = query->record();
            result << mapRecordToTuple<Args...>(rec, 0);
        }
        return result;
    }
};

template <>
class SqlQueryResultMapper<QList<QDateTime>>
{
public:
    QList<QDateTime> map(QSqlQuery *query)
    {
        QList<QDateTime> result;
        //Q_ASSERT(query->record().count() == 1);
        while (query->next())
        {
            result << query->value(0).toDateTime();
        }
        return result;
    }
};

template <>
class SqlQueryResultMapper<void>
{
public:
    void map(QSqlQuery *query)
    {
        query->next();
    }
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

