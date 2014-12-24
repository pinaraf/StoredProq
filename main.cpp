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
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("test");
    db.setUserName("snoopy");
    bool ok = db.open();
    if (!ok)
        qFatal("Could not open db");
    else
        qDebug() << "DB opened";

    SqlSimpleMapper<Operation> mapper("list_all");
    Operation *op = mapper();
    qDebug() << op->description();

    SqlBindingMapper<Operation, int> mapper2("list_all");
    op = mapper2(1);
    qDebug() << op->bookingDate();

    SqlSimpleMapper<QList<Operation*>> mapper3("list_all");
    QList<Operation *>ops = mapper3();
    for (Operation *op: ops) {
        qDebug() << op->id();
    }

    //SqlBindingMapper<int, int, int> mapper4("mymax");
    //qDebug() << mapper4(42, 73);

    return a.exec();
}
