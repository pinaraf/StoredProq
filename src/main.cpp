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


#include <QCoreApplication>
#include "operation.h"
#include <QMetaProperty>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "sqlmapper.h"

int main(int, char *[])
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("test");
    db.setUserName("moi");
    db.setPassword("moi");
    bool ok = db.open();
    if (!ok)
        qFatal("Could not open db");
    else
        qDebug() << "DB opened";

    QVector<QString> data;
    data << "Hello world";
    data << "Hello ' '' ''' world !";
    qDebug() << "data:" << data;
    SqlBindingMapper<QList<QString>, QVector<QString>> unnest("unnest");
    qDebug() << unnest(data);

    SqlBindingMapper<int, std::tuple<int, QString>> testCard("test_card");
    int value = testCard(std::make_tuple(1, "test"));
    qDebug() << value;

    SqlBindingMapper<int, QVector<int>, int> array_length("array_length");
    QVector<int> data_int;
    data_int << 1 << 2;
    qDebug() << "Our dims are :" << array_length(data_int, 1);

    Operation *op;

    SqlBindingMapper<Operation*> mapper("list_all");
    op = mapper();
    qDebug() << op->id();

    SqlBindingMapper<std::vector<int>, int, int, int, int, int> build_array("build_array");
    auto test = build_array(1, 2123, 1233, 1, 15);
    for (int i: test) {
        qDebug() << "Got i from array :" << i;
    }

/*
    SqlBindingMapper<Operation, int> mapper2("list_all");
    op = mapper2(1);
    qDebug() << op->bookingDate();
*/
    SqlBindingMapper<QList<Operation*>> mapper3("list_all");
    QList<Operation *>ops = mapper3();
    for (Operation *op: ops) {
        qDebug() << op->id();
    }
/*
    SqlBindingMapper<int, int, int> mapper4("mymax");
    qDebug() << mapper4(42, 73);
*/
    SqlBindingMapper<QList<std::tuple<int>>, int, int> generateSeries("generate_series");
    for (auto i: generateSeries(1, 10))
        qDebug() << std::get<0>(i);
    for (auto i: generateSeries(2, 5))
        qDebug() << std::get<0>(i);
    SqlBindingMapper<QDateTime> get_now("now");
    qDebug() << get_now();

    int x, y;
    x = 42;
    y = 73;
    SqlBindingMapper<std::tuple<int, int>, int, int> swapper("swap");
    qDebug() << "x = " << x << " ; y = " << y;
    std::tuple<int, int> res = swapper(x, y);
    qDebug() << "x = " << std::get<0>(res) << " ; y = " << std::get<1>(res);

    QJsonDocument doc = QJsonDocument::fromJson("{\"hello\": {\"world\": false, \"me\": true}}");
    SqlBindingMapper<QJsonDocument, QJsonDocument, QString> json_extractor("test_json");
    qDebug() << json_extractor(doc, "{hello}").toJson();

#if 0

    SqlBindingMapper<int, std::tuple<int, int>> summer("sum_me");
    qDebug() << summer(std::make_tuple(1, 2));
#endif


    return 0;
}
