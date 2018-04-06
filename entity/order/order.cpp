#include "order.h"

Order::Order()
{
}

Order::Order(const QVariantMap &information)
{
    addInformation(information);
}

 int Order::getInvoiceNumber() const
{
    return information_["invoiceNumber"].toInt();
}

void Order::addInformation(const QVariantMap &information)
{
    for(auto key:information.keys())
        if(information_.contains(key) && !information[key].isNull())
            information_[key] = information[key];

    if(information.contains("orderDate") && information.contains("orderTime"))
        information_["orderDateTime"] = QDateTime::fromString(information["orderDate"].toString() + information["orderTime"].toString(), dateTimeFormat_).addYears(100);
}

 QVariantMap Order::extractInformation() const
{
    return information_;
}

QString Order::toCSVString()
{
    QStringList csvStringList;
    for(auto dataKey: Order::getCSVStringListOrder())
        if(information_.contains(dataKey))
            csvStringList.append(stripQuotes(information_[dataKey].toString()));

    for(auto dataKey: Order::getCSVStringListOrder())
        if(analysisInformation_.contains(dataKey))
            csvStringList.append(stripQuotes(analysisInformation_[dataKey].toString()));


    return csvStringList.join(",");
}


bool Order::informationValid()
{

    for(auto key:information_.keys())
        if(information_[key].isNull())
            return false;

    if(!information_["invoiceNumber"].canConvert<int>())
        return false;

    if(!information_["customerNumber"].canConvert<int>())
        return false;

    if(!information_["masterCustomerNumber"].canConvert<int>())
        return false;

    if(!information_["routeKey"].canConvert<QString>())
        return false;

    if(!information_["stopNumber"].canConvert<int>())
        return false;

    if(!information_["invoiceDate"].canConvert<QString>())
        return false;
    else
    {
        QDate orderDate = QDate::fromString(information_["invoiceDate"].toString(), dateFormat_);
        if(!orderDate.isValid())
            return false;
        information_["invoiceDate"] = QVariant(orderDate);
    }

    if(!information_["orderDateTime"].canConvert<QString>())
    {
        qDebug() << "failed order datetime" << information_["orderDateTime"];
        qDebug() << information_["orderDateTime"];
        return false;
    }
    else
    {
        QDateTime orderDateTime = QDateTime::fromString(information_["orderDateTime"].toString(), dateTimeFormat_);
        if(!orderDateTime.isNull())
        {
            qDebug() << "failed datetime" << information_["orderDateTime"];
            return false;
        }
        qDebug() << orderDateTime;
        information_["orderDateTime"] = orderDateTime;

    }

    if(!information_["pieces"].canConvert<int>())
        return false;

    if(!information_["weight"].canConvert<float>())
        return false;

    if(!information_["cube"].canConvert<float>())
        return false;

    if(!information_["netSales"].canConvert<float>())
        return false;

    if(!information_["cost"].canConvert<float>())
        return false;

    if(!information_["profit"].canConvert<float>())
        return false;

    return true;
}

void Order::calculateAnalysisData()
{
    analysisInformation_["dayOfWeekInt"] = information_["invoiceDate"].toDate().dayOfWeek();
}

QStringList Order::getCSVStringListOrder()
{     
        QStringList csvStringListOrder_
        { "invoiceNumber",
        "customerNumber",
        "customerName",
        "masterCustomerNumber",
        "routeKey",
        "stopNumber",
        "invoiceDate",
        "orderDateTime",
        "pieces",
        "weight",
        "cube",
        "netSales",
        "cost",
        "profit"};
        return csvStringListOrder_;
}

QStringList Order::getKeyListInOrder()
{
    return Order::getCSVStringListOrder();
}

QMap<QString, QString> Order::getDBTableInfo()
{
    return QMap<QString, QString> {
        {"invoiceNumber", "INTEGER PRIMARY KEY"},
        {"customerNumber", "INTEGER"},
        {"customerName", "TEXT"},
        {"masterCustomerNumber", "INTEGER"},
        {"routeKey", "TEXT"},
        {"stopNumber", "INTEGER"},
        {"invoiceDate", "TEXT"},
        {"orderDateTime", "TEXT"},
        {"pieces", "REAL"},
        {"weight", "REAL"},
        {"cube", "REAL"},
        {"netSales", "REAL"},
        {"cost", "REAL"},
        {"profit", "REAL"}
    };
}

QString Order::stripQuotes(const QString &data)
{
    QString quotelessData = QString(data).remove("\"");
    return QString("\"" + quotelessData + "\"");
}

QStringList Order::neededKeys()
{
    QStringList needed;
    for(auto key:information_.keys())
        if(information_[key].isNull())
            needed << key;

    return needed;
}

QStringList Order::completedKeys()
{
    QStringList needed;
    for(auto key:information_.keys())
        if(!information_[key].isNull())
            needed << key;

    return needed;
}
