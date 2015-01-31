#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QJsonDocument>
#include <tuple>

#include "queryresult.h"
#include "pg_types.h"

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

template <std::size_t Idx = 0, typename... Args>
inline
typename std::enable_if<((Idx + 1) == sizeof...(Args)), void>::type
_queryBind(QSqlQuery *query, const std::tuple<Args...> &value)
{
    _queryBind(query, std::get<Idx>(value));
}

template <std::size_t Idx = 0, typename... Args>
inline typename std::enable_if<((Idx + 1) != sizeof...(Args)), void>::type
_queryBind(QSqlQuery *query, const std::tuple<Args...> &value)
{
    _queryBind(query, std::get<Idx>(value));
    _queryBind<Idx+1>(query, value);
}

template<typename T, typename... Args>
inline void _queryBind(QSqlQuery *query, T value, Args... args)
{
    _queryBind(query, value);
    _queryBind(query, args...);
}

template <typename T, std::size_t Idx = 0>
struct placeHolderBuilder
{
    static inline QString build ()
    {
        if (pg_types<T>::known)
            return QString("?::%1").arg(pg_types<T>::name());
        else
            return QStringLiteral("?");
        return QStringLiteral("?");
    }
};

template <>
struct placeHolderBuilder<int>
{
    static inline QString build ()
    {
        return QStringLiteral("?::integer");
    }
};

template <typename... Args, std::size_t Idx>
struct placeHolderBuilder<std::tuple<Args...>, Idx>
{

    // Non-final element of tuple...
    template <std::size_t Idx2=Idx>
    static inline typename std::enable_if<((Idx2 + 1) != sizeof...(Args)), QString>::type
    build ()
    {
        QString base = QString::null;
        if (Idx > 0)
            base = QString(", %1").arg(placeHolderBuilder<typename std::tuple_element<Idx, std::tuple<Args...>>::type>::build());
        else
            base = QString("(%1").arg(placeHolderBuilder<typename std::tuple_element<Idx, std::tuple<Args...>>::type>::build());
        base += placeHolderBuilder<std::tuple<Args...>, Idx + 1>::build();
        return base;
    }

    // Final element of tuple...
    template <std::size_t Idx2=Idx>
    static inline typename std::enable_if<((Idx2 + 1) == sizeof...(Args)), QString>::type
    build ()
    {
        if (Idx > 0)
            return QString(", %1)").arg(placeHolderBuilder<typename std::tuple_element<Idx, std::tuple<Args...>>::type>::build());
        else if (Idx == 0)
            return QString("(%1)").arg(placeHolderBuilder<typename std::tuple_element<Idx, std::tuple<Args...>>::type>::build());
        else
            return QString("%1)").arg(placeHolderBuilder<typename std::tuple_element<Idx, std::tuple<Args...>>::type>::build());
    }
};

template<typename T>
inline QString _buildPlaceholders() {
    return placeHolderBuilder<T>::build();
}

template<typename T, typename... Args>
inline
typename std::enable_if<sizeof...(Args), QString>::type
_buildPlaceholders()
{
    return placeHolderBuilder<T>::build() + ", " + _buildPlaceholders<Args...>();
}

template<typename... Args>
inline QString _buildQuery(const QString &functionName)
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
            m_preparedQuery.prepare(_buildQuery<Arguments...>(m_functionName));
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
