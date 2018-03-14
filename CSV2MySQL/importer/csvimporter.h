#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QObject>
#include <QtCore>
#include <QDebug>
#include <QRegularExpression>

class CSVImporter : public QObject
{
    Q_OBJECT
public:
    explicit CSVImporter(QObject *parent = nullptr);
    void setCSVPath(const QString &csvPath);
    QString getCSVPath();

    QVector<QVariantMap> import(int fileType, const QString &filePath);
    static QVector<QVariantMap> import(const QVariantMap &fileMap, const QString &filePath);

    enum reportTypes {OrderTrackingHistory, RouteProfitability};

private:
    QString csvPath_;

    QVariantMap orderTrackingCSVMap_ {{"cdl", QVariant(0)},
                                       {"orderType",QVariant(1)},
                                       {"orderDate",QVariant(2)},
                                       {"invoiceDate",QVariant(3)},
                                       {"orderTime",QVariant(4)},
                                       {"rep",QVariant(5)},
                                       {"terms",QVariant(6)},
                                       {"customerNumber",QVariant(7)},
                                       {"customerName",QVariant(8)},
                                       {"customerBillingName",QVariant(9)},
                                       {"routeKey",QVariant(10)},
                                       {"stopNumber",QVariant(11)},
                                       {"specialInstructions",QVariant(12)},
                                       {"poNumber",QVariant(13)},
                                       {"invoiceNumber",QVariant(14)},
                                       {"quantityShipped",QVariant(15)},
                                       {"extSales",QVariant(16)},
                                       {"invoiceAmmount",QVariant(17)},
                                       {"user",QVariant(18)},
                                       {"user2",QVariant(19)}};

    QVariantMap routeProfitabilityCSVMap_ {{"routeKey", QVariant(0)},
                                           {"customerNumber",QVariant(1)},
                                           {"customerName",QVariant(2)},
                                           {"masterCustomerNumber",QVariant(3)},
                                           {"invoiceDate",QVariant(4)},
                                           {"invoiceNumber",QVariant(5)},
                                           {"numberOfOrders",QVariant(6)},
                                           {"pieces",QVariant(7)},
                                           {"weight",QVariant(8)},
                                           {"cube",QVariant(9)},
                                           {"netSales",QVariant(10)},
                                           {"cost",QVariant(11)},
                                           {"profit",QVariant(12)},
                                           {"profitPercent",QVariant(13)}};

signals:
    int percentComplete(int progress);



public slots:
};

#endif // CSVIMPORTER_H
