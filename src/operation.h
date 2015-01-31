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


#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QDate>

class Operation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate booking_date MEMBER m_bookingDate)
    Q_PROPERTY(QString description MEMBER m_description)
    Q_PROPERTY(int id MEMBER m_id)
    Q_PROPERTY(int amount_in_cents MEMBER m_amountInCents)

public:
    explicit Operation(QObject *parent = 0);
    ~Operation();

    int id() const { return m_id; }
    QString description() const { return m_description; }
    QDate bookingDate() const { return m_bookingDate; }
    int amountInCents() const { return m_amountInCents; }
signals:

public slots:

private:
    QDate m_bookingDate;
    QString m_description;
    int m_id;
    int m_amountInCents;
};

#endif // OPERATION_H
