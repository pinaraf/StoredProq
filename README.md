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


Supported datatypes
===================

Most data types should be handled immediately : so far, the code does not look at the data types returned by PostgreSQL (and I doubt it's doable with that template-based solution), so it relies on the programmer for the types mapping to be correct.

The basic types (string, integer, double, QDateTime) work.
More advanced types like QObject using QMetaObject based introspection, QList for functions returnings several lines, std::tuple for both multiple columns return and passing a composite type in function parameter work.
Being exhaustive, considering the PostgreSQL type collection, is not possible. Instead, it shall be easy to define new mappings if any new type was to be needed with a specific treatment.



Important disclaimer
====================

This is a for fun, educational project. I may use it seriously later, depending on its outcome, but I do not plan to maintain any backward compatibility until then. If you decide to use it in a real project, I'm open to discuss stabilisation and not breaking your work :)
