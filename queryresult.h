#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMetaProperty>
#include <QJsonDocument>

#include <tuple>

template <typename T>
inline T mapRecordFieldToValue(const QSqlRecord &record, int field)
{
    return record.value(field).value<T>();
}

template <>
inline QJsonDocument mapRecordFieldToValue(const QSqlRecord &record, int field)
{
    return QJsonDocument::fromJson(record.value(field).toString().toUtf8());
}

void mapRecordToQObject(const QSqlRecord &record, QObject *target)
{
    const QMetaObject *metaObject = target->metaObject();

    for (int i = 0 ; i < record.count() ; i++)
    {
        int propIdx = metaObject->indexOfProperty(record.fieldName(i).toLocal8Bit());
        if (propIdx >= 0)
        {
            QMetaProperty prop = metaObject->property(propIdx);
            // Hum, without real language level introspection, this is not possible to call mapRecordFieldToValue easily
            prop.write(target, record.value(i));
        }
    }
}

template<typename T>
inline std::tuple<T> mapRecordToTuple(const QSqlRecord &record, int position)
{
    return std::make_tuple<T>(mapRecordFieldToValue<T>(record, position));
}

template<typename T, typename... Args>
inline
typename std::enable_if<sizeof...(Args), std::tuple<T, Args...>>::type
 mapRecordToTuple(const QSqlRecord &record, int position)
{
    return std::tuple_cat(std::make_tuple<T>(mapRecordFieldToValue<T>(record, position)), mapRecordToTuple<Args...>(record, position + 1));
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

    template <typename R = typename std::remove_pointer<T>::type>
    typename std::enable_if<!std::is_base_of<QObject, R>::value, R>::type
    map(QSqlQuery *query)
    {
        query->next();
        QSqlRecord rec = query->record();

        return mapRecordFieldToValue<R>(rec, 0);
    }
};


template <typename T>
class SqlQueryResultMapper<QList<T>>
{
public:
    template <typename R = typename std::remove_pointer<T>::type>
    typename std::enable_if<std::is_base_of<QObject, R>::value, QList<R*>>::type
    map(QSqlQuery *query)
    {
        QList<R*> resultList;
        //Q_ASSERT(query->record().count() == 1);
        while (query->next())
        {
            T result = new R();
            QSqlRecord rec = query->record();
            mapRecordToQObject(rec, result);
            resultList << result;
        }
        return resultList;
    }

    template <typename R = typename std::remove_pointer<T>::type>
    typename std::enable_if<!std::is_base_of<QObject, R>::value, QList<R>>::type
    map(QSqlQuery *query)
    {
        QList<R> resultList;
        while (query->next())
        {
            QSqlRecord rec = query->record();
            resultList << mapRecordFieldToValue<R>(rec, 0);
        }
        return resultList;
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

#endif // QUERYRESULT_H
