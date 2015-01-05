StoredProq
==========

Simple modern C++ stored procedures wrapper

This project was started for me to learn modern C++ techniques like variadic templates.
It allows you to easily call a PostgreSQL stored procedure, wrapping both the parameters and the result in simple datatypes.
It currently depends on Qt5 for QMetaObject introspection and QSql, but this dependency will likely be removed or lifted in a separate file.


Example usage
=============

First step is to connect using QSql (longer term, raw libpq could be used instead)
```
QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
db.setHostName("localhost");
db.setDatabaseName("test");
bool ok = db.open();
```

Then you can declare a stored procedure
```
SqlBindingMapper<QList<int>, int, int> generateSeries("generate_series");
```

And simply call it like any function…
```
for (int i: generateSeries(1, 10))
    qDebug() << i;
```
