#ifndef CHARLIESCSVIMPORTER_H
#define CHARLIESCSVIMPORTER_H

#include "csvimporter.h"
#include "json2sqlite/json2sqlite.h"
#include <QtCore>
#include <QObject>

class CharliesCSVImporter : public CSVImporter,public Json2Sqlite
{
    Q_OBJECT
public:
    enum reportTypes {OrderTrackingHistory, RouteProfitability};

    explicit CharliesCSVImporter();

    QJsonArray importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes);
    QJsonArray importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes, const QString &csvPath);

    QList<QVariantMap> importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes);
    QList<QVariantMap> importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes, const QString &csvPath);

    QList<QPair<int, QVariantMap> > importCharliesCSVAsPair(CharliesCSVImporter::reportTypes);
    QList<QPair<int, QVariantMap> > importCharliesCSVAsPair(CharliesCSVImporter::reportTypes, const QString &csvPath);

private:
    QMap<QString, int> orderTrackingCSVMap_ {{"cdl", int(0)},
                                       {"orderType",int(1)},
                                       {"orderDate",int(2)},
                                       {"invoiceDate",int(3)},
                                       {"orderTime",int(4)},
                                       {"rep",int(5)},
                                       {"terms",int(6)},
                                       {"customerNumber",int(7)},
                                       {"customerName",int(8)},
                                       {"customerBillingName",int(9)},
                                       {"routeKey",int(10)},
                                       {"stopNumber",int(11)},
                                       {"specialInstructions",int(12)},
                                       {"poNumber",int(13)},
                                       {"invoiceNumber",int(14)},
                                       {"quantityShipped",int(15)},
                                       {"extSales",int(16)},
                                       {"invoiceAmmount",int(17)},
                                       {"user",int(18)},
                                       {"user2",int(19)}};

    QMap<QString, int> routeProfitabilityCSVMap_ {{"routeKey", int(0)},
                                           {"customerNumber",int(1)},
                                           {"customerName",int(2)},
                                           {"masterCustomerNumber",int(3)},
                                           {"invoiceDate",int(4)},
                                           {"invoiceNumber",int(5)},
                                           {"numberOfOrders",int(6)},
                                           {"pieces",int(7)},
                                           {"weight",int(8)},
                                           {"cube",int(9)},
                                           {"netSales",int(10)},
                                           {"cost",int(11)},
                                           {"profit",int(12)},
                                           {"profitPercent",int(13)}};
signals:

public slots:
};

#endif // CHARLIESCSVIMPORTER_H
