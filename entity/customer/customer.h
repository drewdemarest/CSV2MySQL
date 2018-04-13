#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QObject>
#include "entity/order/order.h"

class Customer : public QObject
{
    Q_OBJECT
public:
    explicit Customer(const QVariantMap &routingInformation, QObject *parent = nullptr);


    void addOrder(Order order);

private:
    void addRouting(const QVariantMap &routingInformation);
    const QVector<QString> daysOfWeek_ = {"mon",
                                          "tue",
                                          "wed",
                                          "thu",
                                          "fri",
                                          "sat",
                                          "sun"};



    QVector<QPair<QString, int>> routing_ = QVector<QPair<QString, int>>(7);
    QVector<Order> orders_;

signals:

public slots:
};

#endif // CUSTOMER_H
