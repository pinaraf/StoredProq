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

