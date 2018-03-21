#ifndef ORDER_H
#define ORDER_H

#include <QtCore>
#include <QDebug>

class Order
{
public:
    Order();
    Order(const QVariantMap &information);
    int getInvoiceNumber();

    void addInformation(const QVariantMap &information);
    void setDateFormat(const QString &dateFormat);
    void setDateTimeFormat(const QString &dateTimeFormat);

    QString toCSVString();

    QStringList necessaryKeys();
    QStringList neededKeys();
    bool informationValid();
    void calculateAnalysisData();

    static QStringList getCSVStringListOrder();

private:
    QString stripQuotes(const QString &data);

    QString dateFormat_ = "M/d/yyyy";
    QString dateTimeFormat_ = "M/d/yyyyh:mm:ss";

    QVariantMap information_ {{"invoiceNumber", QVariant()},
                              {"customerNumber", QVariant()},
                              {"customerName", QVariant()},
                              {"masterCustomerNumber",QVariant()},
                              {"routeKey", QVariant()},
                              {"stopNumber", QVariant()},
                              {"orderDateTime", QVariant()},
                              {"invoiceDate", QVariant()},
                              {"pieces", QVariant()},
                              {"weight", QVariant()},
                              {"cube", QVariant()},
                              {"netSales", QVariant()},
                              {"cost", QVariant()},
                              {"profit", QVariant()}};

    QVariantMap analysisInformation_ = {{"dayOfWeekInt", QVariant()}};
};

#endif // ORDER_H
