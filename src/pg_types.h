#ifndef PG_TYPES_H
#define PG_TYPES_H

#include <QString>

template<typename T>
struct pg_types
{
    static const bool known = false;
    static const char *name() { return ""; }
    static QString quoteValue (T & value) {
        return QString("?").arg(value);
    }
};

template<>
struct pg_types<int>
{
    static const bool known = true;
    static const char *name() { return "integer"; }
    static QString quoteValue (int value) {
        return QString::number(value);
    }
};

#endif // PG_TYPES_H

