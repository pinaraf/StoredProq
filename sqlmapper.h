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

template<typename T>
inline QString _buildPlaceholder(int position)
{
    return QString(":param%1").arg(position);
}

template<>
inline QString _buildPlaceholder<int> (int position)
{
    return QString(":param%1::integer").arg(position);
}

template<typename T>
QString _buildPlaceholders(int position, T) {
    return _buildPlaceholder<T>(position);
}

template<typename T, typename... Args>
QString _buildPlaceholders(int position, T, Args... args)
{
    if (sizeof...(Args) == 0)
        return _buildPlaceholder<T>(position);
    else
        return _buildPlaceholder<T>(position) + ", " + _buildPlaceholders<Args...>(position + 1, args...);
}

template<typename... Args>
inline QString _buildQuery(const QString &functionName, Args... args)
{
    if (sizeof...(Args) == 0) {
        return QString("SELECT * FROM %1();").arg(functionName);
    } else {
        QString placeHolders = _buildPlaceholders(0, args...);
        qDebug() << QString("SELECT * FROM %1(%2);").arg(functionName).arg(placeHolders);
        return QString("SELECT * FROM %1(%2);").arg(functionName).arg(placeHolders);
    }
}

inline QString _buildQuery(const QString &functionName)
{
    return QString("SELECT * FROM %1();").arg(functionName);
}


class BaseMapper
{
public:
    BaseMapper() : m_preparedQuery(nullptr) {}
    ~BaseMapper() { if (m_preparedQuery) delete(m_preparedQuery); }

    QSqlQuery *preparedQuery() const { return m_preparedQuery; }
    QSqlQuery *setQuery(const QString &query) {
        if (m_preparedQuery)
            delete(m_preparedQuery);
        m_preparedQuery = new QSqlQuery();
        m_preparedQuery->prepare(query);
        return m_preparedQuery;
    }
private:
    QSqlQuery *m_preparedQuery;
};

template <typename T, typename... Arguments>
class SqlBindingMapper : BaseMapper
{
public:
    SqlBindingMapper(const QString &functionName) : BaseMapper(), m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) != 0), R*>::type
    operator() (Arguments... params) {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName, params...));
        _queryBind(q, 0, params...);
        q->exec();

        return m_mapper.map(q);
    }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) == 0), R*>::type
    operator() () {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName));
        q->exec();

        return m_mapper.map(q);
    }

private:
    QString m_functionName;
    SqlQueryResultMapper<T> m_mapper;
};


template <typename... Arguments>
class SqlBindingMapper<int, Arguments...> : BaseMapper
{
public:
    SqlBindingMapper(const QString &functionName) : BaseMapper(), m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) != 0), R>::type
    operator() (Arguments... params) {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName, params...));
        _queryBind(q, 0, params...);
        q->exec();

        return m_mapper.map(q);
    }

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) == 0), R>::type
    operator() () {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName));
        q->exec();

        return m_mapper.map(q);
    }

private:
    QString m_functionName;
    SqlQueryResultMapper<int> m_mapper;
};

template <typename... Arguments>
class SqlBindingMapper<QList<int>, Arguments...> : BaseMapper
{
public:
    SqlBindingMapper(const QString &functionName) : BaseMapper(), m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) != 0), QList<R>>::type
    operator() (Arguments... params) {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName, params...));
        _queryBind(q, 0, params...);
        q->exec();

        return m_mapper.map(q);
    }

    template<typename R=int>
    typename std::enable_if<(sizeof...(Arguments) == 0), QList<R>>::type
    operator() () {
      QSqlQuery *q = preparedQuery();
      if (!q)
          q = setQuery(_buildQuery(m_functionName));
      q->exec();

      return m_mapper.map(q);
    }

private:
    QString m_functionName;
    SqlQueryResultMapper<QList<int>> m_mapper;
};

template <typename T, typename... Arguments>
class SqlBindingMapper<QList<T*>, Arguments...> : BaseMapper
{
public:
    SqlBindingMapper(const QString &functionName) : BaseMapper(), m_functionName(functionName) {}
    ~SqlBindingMapper() {}

    template<typename R=T>
    typename std::enable_if<sizeof...(Arguments) != 0, QList<R *> >::type
    operator() (Arguments... params) {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName, params...));
        _queryBind(q, 0, params...);
        q->exec();

        return m_mapper.map(q);
    }

    template<typename R=T>
    typename std::enable_if<sizeof...(Arguments) == 0, QList<R *> >::type
    operator() () {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName));
        q->exec();

        return m_mapper.map(q);
    }

private:
    QString m_functionName;
    SqlQueryResultMapper<QList<T*>> m_mapper;
};

#endif // SQLMAPPER_H
