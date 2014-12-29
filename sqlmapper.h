#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "queryresult.h"

template<typename T>
inline void _queryBind(QSqlQuery *query, const T &value)
{
    query->addBindValue(value);
}

template<typename T, typename... Args>
inline void _queryBind(QSqlQuery *query, T value, Args... args)
{
    _queryBind(query, value);
    _queryBind(query, args...);
}

template<typename T>
inline QString _buildPlaceholder()
{
    return QStringLiteral("?");
}

template<>
inline QString _buildPlaceholder<int> ()
{
    return QStringLiteral("?::integer");
}

template<typename T>
inline QString _buildPlaceholders() {
    return _buildPlaceholder<T>();
}

template<typename T, typename... Args>
inline
typename std::enable_if<sizeof...(Args), QString>::type
_buildPlaceholders()
{
    return _buildPlaceholder<T>() + ", " + _buildPlaceholders<Args...>();
}

template<typename... Args>
inline QString _buildQuery(const QString &functionName, Args...)
{
    if (sizeof...(Args) == 0) {
        return QString("SELECT * FROM %1();").arg(functionName);
    } else {
        QString placeHolders = _buildPlaceholders<Args...>();
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
    typename std::enable_if<(sizeof...(Arguments) != 0), R>::type
    operator() (Arguments... params) {
        QSqlQuery *q = preparedQuery();
        if (!q)
            q = setQuery(_buildQuery(m_functionName, params...));
        _queryBind(q, params...);
        q->exec();

        return m_mapper.map(q);
    }

    template<typename R=T>
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
    SqlQueryResultMapper<T> m_mapper;
};


#endif // SQLMAPPER_H
