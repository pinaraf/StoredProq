#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMetaProperty>

#include "queryresult.h"

template<typename T>
void _queryBind(QSqlQuery *query, int position, T value)
{
    query->bindValue(position, value);
}

template<typename T, typename... Args>
void _queryBind(QSqlQuery *query, int position, T value, Args... args)
{
    query->bindValue(position, value);
    _queryBind(query, position + 1, args...);
}

//TODO: use variadic template here too to dynamically build the query string and cast
inline QString _buildQuery(const QString &functionName, int parameterCount)
{
    if (parameterCount == 0) {
        return QString("SELECT * FROM %1();").arg(functionName);
    } else {
        QString placeHolders;
        for (int i = 0 ; i < parameterCount ; i++)
        {
            if (i > 0)
                placeHolders += ",";
            //TODO : casts !
            placeHolders += QString(":param%1").arg(i);
        }
        qDebug() << QString("SELECT * FROM %1(%2);").arg(functionName).arg(placeHolders);
        return QString("SELECT * FROM %1(%2);").arg(functionName).arg(placeHolders);
    }
}

template <typename T, typename... Arguments>
class SqlBindingMapper
{
public:
    SqlBindingMapper(const QString &functionName) : m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) != 0), R*>::type
    operator() (Arguments... params) {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, sizeof...(Arguments)));
        _queryBind(&q, 0, params...);
        q.exec();

        SqlQueryResultMapper<T> mapper;
        return mapper.map(&q);
    }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) == 0), R*>::type
    operator() () {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, 0));
        q.exec();

        SqlQueryResultMapper<T> mapper;
        return mapper.map(&q);
    }

private:
    QString m_functionName;
};


template <typename... Arguments>
class SqlBindingMapper<int, Arguments...>
{
public:
    SqlBindingMapper(const QString &functionName) : m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) != 0), R>::type
    operator() (Arguments... params) {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, sizeof...(Arguments)));
        _queryBind(&q, 0, params...);
        q.exec();

        SqlQueryResultMapper<int> mapper;
        return mapper.map(&q);
    }

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) == 0), R>::type
    operator() () {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, 0));
        q.exec();

        SqlQueryResultMapper<int> mapper;
        return mapper.map(&q);
    }

private:
    QString m_functionName;
};

template <typename... Arguments>
class SqlBindingMapper<QList<int>, Arguments...>
{
public:
    SqlBindingMapper(const QString &functionName) : m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) != 0), QList<R>>::type
    operator() (Arguments... params) {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, sizeof...(Arguments)));
        _queryBind(&q, 0, params...);
        q.exec();

        SqlQueryResultMapper<QList<int>> mapper;
        return mapper.map(&q);
    }

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) == 0), QList<R>>::type
    operator() () {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, 0));
        q.exec();

        SqlQueryResultMapper<QList<int>> mapper;
        return mapper.map(&q);
    }

private:
    QString m_functionName;
};

template <typename T, typename... Arguments>
class SqlBindingMapper<QList<T*>, Arguments...>
{
public:
    SqlBindingMapper(const QString &functionName) : m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=T>
    typename std::enable_if<sizeof...(Arguments) != 0, QList<R *> >::type
    operator() (Arguments... params) {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, sizeof...(Arguments)));
        _queryBind(&q, 0, params...);
        q.exec();

        SqlQueryResultMapper<QList<T*>> mapper;
        return mapper.map(&q);
    }

    template<typename R=T>
    typename std::enable_if<sizeof...(Arguments) == 0, QList<R *> >::type
    operator() () {
        QSqlQuery q;
        q.prepare(_buildQuery(m_functionName, 0));
        q.exec();

        SqlQueryResultMapper<QList<T*>> mapper;
        return mapper.map(&q);
    }

private:
    QString m_functionName;
};

#endif // SQLMAPPER_H
