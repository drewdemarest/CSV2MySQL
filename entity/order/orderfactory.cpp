#include "orderfactory.h"

OrderFactory::OrderFactory(QObject *parent) : QObject(parent)
{

}

OrderMap OrderFactory::makeOrders(const QList<QVariantMap> &vMap)
{
    return QtConcurrent::blockingMappedReduced(vMap, OrderFactory::makeOrderFromVarMap, OrderFactory::reduce);
}

OrderMap OrderFactory::addToOrders(const OrderMap &oMap, const QList<QVariantMap> &vMap)
{
    OrderMap oMap1 = oMap;
    OrderMap oMap2 = QtConcurrent::blockingMappedReduced(vMap, OrderFactory::makeOrderFromVarMap, OrderFactory::reduce);

    QList<int> keys = oMap1.keys();
    keys.append(oMap2.keys());
    QSet<int> sharedKeys = keys.toSet();

    for(auto key: sharedKeys)
    {
        oMap1[key].addInformation(oMap2[key].extractInformation());
        oMap2.remove(key);
    }

    oMap2.clear();

    for(auto key:oMap1.keys())
        if(!oMap1[key].neededKeys().isEmpty())
            oMap1.remove(key);

    return oMap1;
}

OrderMap OrderFactory::makeOrderFromVarMap(const QVariantMap &vMap)
{
    OrderMap oMap;
    Order order = Order(vMap);
    oMap[order.getInvoiceNumber()] = order;
    return oMap;
}

void OrderFactory::reduce(OrderMap &result, const OrderMap &oMap)
{
    QMapIterator<int, Order> i(oMap);
    while(i.hasNext())
    {
        i.next();
        result[i.key()] = i.value();
    }
}


