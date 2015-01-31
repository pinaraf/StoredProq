#ifndef PG_TYPES_H
#define PG_TYPES_H

#include <QString>

template<typename T>
struct pg_types
{
    static constexpr bool known = false;
    static constexpr const char *name() { return ""; }
    static QString quoteValue (T & value) {
        return QString("?").arg(value);
    }
};

template<>
struct pg_types<int>
{
    static constexpr bool known = true;
    static constexpr const char *name() { return "integer"; }
    static QString quoteValue (int value) {
        return QString::number(value);
    }
};

template<>
struct pg_types<double>
{
    static constexpr bool known = true;
    static constexpr const char *name() { return "double"; }
    static QString quoteValue (double value) {
        return QString::number(value);
    }
};

template<>
struct pg_types<QString>
{
    static constexpr bool known = true;
    static constexpr const char *name() { return "text"; }
    static QString quoteValue (QString value) {
        return value.replace("'", "''");
    }
};

#endif // PG_TYPES_H

