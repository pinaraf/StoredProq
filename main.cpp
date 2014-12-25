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
    qDebug() << _buildPlaceholder<QString>(0);
    qDebug() << _buildPlaceholders<QString, int, QString, int>(0, true);

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("test");
    db.setUserName("snoopy");
    bool ok = db.open();
    if (!ok)
        qFatal("Could not open db");
    else
        qDebug() << "DB opened";

    Operation *op;

    SqlBindingMapper<Operation> mapper("list_all");
    op = mapper();
    qDebug() << op->description();

    SqlBindingMapper<Operation, int> mapper2("list_all");
    op = mapper2(1);
    qDebug() << op->bookingDate();

    SqlBindingMapper<QList<Operation*>> mapper3("list_all");
    QList<Operation *>ops = mapper3();
    for (Operation *op: ops) {
        qDebug() << op->id();
    }

    SqlBindingMapper<int, int, int> mapper4("mymax");
    qDebug() << mapper4(42, 73);

    SqlBindingMapper<QList<int>, int, int> generateSeries("generate_series");
    for (int i: generateSeries(1, 10))
        qDebug() << i;
    return a.exec();
}
