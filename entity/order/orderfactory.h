#ifndef ORDERFACTORY_H
#define ORDERFACTORY_H

#include "order.h"
#include <QObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

typedef QMap<int, Order> OrderMap;

class OrderFactory : public QObject
{
    Q_OBJECT
public:
    explicit OrderFactory(QObject *parent = nullptr);
    static OrderMap makeOrders(const QList<QVariantMap> &vMap1, const QList<QVariantMap> &vMap2);
    static OrderMap addToOrders(const OrderMap &oMap, const QVariantMap &vMap);
private:
    static OrderMap makeOrderFromVarMap(const QVariantMap &vMap);
    static void reduce(OrderMap &result, const OrderMap &oMap);

signals:

public slots:
};

#endif // ORDERFACTORY_H
