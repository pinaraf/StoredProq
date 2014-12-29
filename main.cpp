#include <QCoreApplication>
#include "operation.h"
#include <QMetaProperty>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "sqlmapper.h"

int main(int argc, char *argv[])
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

    Operation *op;

    SqlBindingMapper<Operation*> mapper("list_all");
    op = mapper();
    qDebug() << op->description();
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
}
