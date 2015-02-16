/*
 * This file is part of the StoredProq project
 * distributed under the MIT License (MIT)
 *
 * Copyright (c) 2015 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QJsonDocument>
#include <QVector>
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

template <typename T>
inline void _queryBind(QSqlQuery *query, const QVector<T> &value)
{
    QString vectorContent = "{";
    bool first = true;
    for (auto& v: value) {
        if (!first)
            vectorContent.append(",");
        else
            first = false;
        vectorContent.append(pg_types<T>::quoteValue(v));
    }
    vectorContent.append("}");
    query->addBindValue(vectorContent);
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
    }
};

template <typename T>
struct placeHolderBuilder<QVector<T>>
{
    static inline QString build ()
    {
        if (pg_types<T>::known)
            return QString("?::%1[]").arg(pg_types<T>::name());
        else
            return QStringLiteral("?");
    }
};

template <typename... Args, std::size_t Idx>
struct placeHolderBuilder<std::tuple<Args...>, Idx>
{

    // Non-final element of tuple...
    template <std::size_t Idx2 = Idx + 1>
    static inline typename std::enable_if<(Idx2 != sizeof...(Args)), QString>::type
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
    template <std::size_t Idx2 = Idx + 1>
    static inline typename std::enable_if<(Idx2 == sizeof...(Args)), QString>::type
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
typename std::enable_if<sizeof...(Args) != 0, QString>::type
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
    SqlBindingMapper(const QString &functionName) : SqlBindingMapper(QString::null, functionName) {}
    SqlBindingMapper(const QString &schemaName, const QString &functionName) : SqlBindingMapper(QSqlDatabase::defaultConnection, schemaName, functionName) {}

    SqlBindingMapper(const char *connectionName, const QString &schemaName, const QString &functionName)
        : m_schemaName(schemaName),
          m_functionName(functionName),
          m_preparedQuery(QSqlDatabase::database(connectionName))
    {}

    ~SqlBindingMapper() { }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) != 0), R>::type
    operator() (Arguments... params) {
        if (!m_preparedQuery.isValid())
            m_preparedQuery.prepare(_buildQuery<Arguments...>(sqlFunctionName()));
        _queryBind(&m_preparedQuery, params...);

        _exec();

        return m_mapper.map(&m_preparedQuery);
    }

    template<typename R=T>
    typename std::enable_if<(sizeof...(Arguments) == 0), R>::type
    operator() () {
        if (!m_preparedQuery.isValid())
            m_preparedQuery.prepare(_buildQuery(sqlFunctionName()));

        _exec();

        return m_mapper.map(&m_preparedQuery);
    }

    QString sqlFunctionName() const {
        if (!m_schemaName.isEmpty())
            return QString("\"%1\".\"%2\"").arg(m_schemaName).arg(m_functionName);
        else
            return QString("\"%1\"").arg(m_functionName);
    }

private:

    inline void _exec() {
        if (!m_preparedQuery.exec()) {
            qDebug() << "Got a database failure :" << m_preparedQuery.lastError().text();
            qFatal("Stopping for database issue");
        }
    }

    QString m_schemaName;
    QString m_functionName;
    SqlQueryResultMapper<T> m_mapper;
    QSqlQuery m_preparedQuery;
};


#endif // SQLMAPPER_H
