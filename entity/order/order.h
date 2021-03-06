#ifndef ORDER_H
#define ORDER_H

#include <QtCore>
#include <QDebug>

class Order
{
public:
    Order();
    Order(const QVariantMap &information);
    int getInvoiceNumber() const;
    QVariantMap extractInformation() const;
    void addInformation(const QVariantMap &information);
    void setDateFormat(const QString &dateFormat);
    void setDateTimeFormat(const QString &dateTimeFormat);

    QString toCSVString();

    QStringList necessaryKeys();
    QStringList neededKeys();
    QStringList completedKeys();
    bool informationValid();
//    void calculateAnalysisData();

    static QStringList getCSVStringListOrder();
    static QStringList getKeyListInOrder();
    static QMap<QString,QString> getDBTableInfo();

private:
    friend class Customer;
    QString stripQuotes(const QString &data);

    QString dateFormat_ = "M/d/yyyy";
    QString dateTimeFormat_ = "MM/dd/yyhh:mm:ss";

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

    bool offday_;
    bool fixer_;
    bool routed_;

    bool isFixer()const{return fixer_;}
    bool isOffday()const{return offday_;}
    bool isRouted()const{return routed_;}

    //QVariantMap analysisInformation_ = {{"dayOfWeekInt", QVariant()}};
};

#endif // ORDER_H
