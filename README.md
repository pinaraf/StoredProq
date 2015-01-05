StoredProq
==========

Simple modern C++ stored procedures wrapper

This project was started for me to learn modern C++ techniques like variadic templates.
It allows you to easily call a PostgreSQL stored procedure, wrapping both the parameters and the result in simple datatypes.
It currently depends on Qt5 for QMetaObject introspection and QSql, but this dependency will likely be removed or lifted in a separate file.
