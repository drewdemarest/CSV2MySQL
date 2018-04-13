#include "customer.h"

Customer::Customer(const QVariantMap &routingInformation, QObject *parent) : QObject(parent)
{
    addRouting(routingInformation);
}

void Customer::addRouting(const QVariantMap &routingInformation)
{
    QString keyRoute; keyRoute.reserve(100);
    QString keyStop; keyStop.reserve(100);

    for(int dayInt = 0; dayInt < daysOfWeek_.size(); ++dayInt)
    {
        keyRoute = daysOfWeek_[dayInt];
        keyStop = daysOfWeek_[dayInt] + "Stop";

        if(routingInformation.contains(keyRoute))
            routing_[dayInt].first = routingInformation.value(keyRoute).toString();

        if(routingInformation.contains(keyStop))
            routing_[dayInt].second = routingInformation.value(keyStop).toInt();
    }
}

void Customer::addOrder(Order order)
{

    //    QVariantMap information_ {{"invoiceNumber", QVariant()},
    //                              {"customerNumber", QVariant()},
    //                              {"customerName", QVariant()},
    //                              {"masterCustomerNumber",QVariant()},
    //                              {"routeKey", QVariant()},
    //                              {"stopNumber", QVariant()},
    //                              {"orderDateTime", QVariant()},
    //                              {"invoiceDate", QVariant()},
    //                              {"pieces", QVariant()},
    //                              {"weight", QVariant()},
    //                              {"cube", QVariant()},
    //                              {"netSales", QVariant()},
    //                              {"cost", QVariant()},
    //                              {"profit", QVariant()}};

    QVariantMap orderInfo = order.extractInformation();
    int dayOfWeek = -1;
    QDate orderDate = orderInfo["shipDate"].toDate();
    if(orderDate.isValid())
        dayOfWeek = orderDate.dayOfWeek() - 1;

    if(routing_[dayOfWeek].first == orderInfo["routeKey"].toString())
        order.routed_ = true;
    if(routing_[dayOfWeek].first.isEmpty() && !orderInfo["routeKey"].toString().isEmpty())
        order.offday_ = true;
    if(!routing_[dayOfWeek].first.isEmpty() && routing_[dayOfWeek].first != orderInfo["routeKey"].toString())
        order.fixer_ = true;
}
