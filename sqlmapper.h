#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QJsonDocument>
#include <tuple>

#include "queryresult.h"

template<typename T>
inline void _queryBind(QSqlQuery *query, const T &value)
{
    query->addBindValue(value);
}

template <>
inline void _queryBind(QSqlQuery *query, const QJsonDocument &value)
{
    query->addBindValue(QString::fromUtf8(value.toJson()));
}
/*
template <>
inline void _queryBind(QSqlQuery *query, const std::tuple &value)
{
    query->addBindValue(QString::fromUtf8(value.toJson()));
}
*/
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

template <typename T, typename... Arguments>
class SqlBindingMapper
{
public:
    SqlBindingMapper(const QString &functionName) : m_functionName(functionName) {}
    ~SqlBindingMapper() { }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) != 0), R>::type
    operator() (Arguments... params) {
        if (!m_preparedQuery.isValid())
            m_preparedQuery.prepare(_buildQuery(m_functionName, params...));
        _queryBind(&m_preparedQuery, params...);
        m_preparedQuery.exec();

        return m_mapper.map(&m_preparedQuery);
    }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) == 0), R>::type
    operator() () {
        if (!m_preparedQuery.isValid())
            m_preparedQuery.prepare(_buildQuery(m_functionName));
        m_preparedQuery.exec();

        return m_mapper.map(&m_preparedQuery);
    }

private:

    QString m_functionName;
    SqlQueryResultMapper<T> m_mapper;
    QSqlQuery m_preparedQuery;
};


#endif // SQLMAPPER_H
