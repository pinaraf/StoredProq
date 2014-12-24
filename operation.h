#ifndef OPERATION_H
#define OPERATION_H

#include <QObject>
#include <QDate>

template <class T> QList<T> list_all();

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
