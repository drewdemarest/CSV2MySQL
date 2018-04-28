#ifndef MYSQLEXPORTER_H
#define MYSQLEXPORTER_H

#include <QtCore>
#include <QObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "datainfo/datainfo.hpp"

class mysqlExporter : public QObject
{
    Q_OBJECT
public:
    explicit mysqlExporter(QObject *parent = nullptr);

    bool exportQueryString(const QString &queryString);
    bool exportToMySQL(int chunkSize);
    bool exportInvoiceToMySQL(int chunkSize);

    const QMap<QString, QString> formatCustomerChainGroupCSV_
    {
        {"CustomerChainGroupCSV.company",                 "company"},
        {"CustomerChainGroupCSV.division",                "division"},
        {"CustomerChainGroupCSV.location",                "location"},
        {"CustomerChainGroupCSV.warehouse",               "warehouse"},
        {"CustomerChainGroupCSV.customerNumber",          "customerNumber"},
        {"CustomerChainGroupCSV.storeNumber",             "storeNumber"},
        {"CustomerChainGroupCSV.stopNumber",              "stopNumber"},
        {"CustomerChainGroupCSV.phoneNumber",             "phoneNumber"},
        {"CustomerChainGroupCSV.billToName",              "billToName"},
        {"CustomerChainGroupCSV.chainID",                 "chain"},
        {"CustomerChainGroupCSV.chainDescription",        "chainDescription"},
        {"CustomerChainGroupCSV.groupID",                 "groupID"},
        {"CustomerChainGroupCSV.groupDescription",        "groupDescription"},
        {"CustomerChainGroupCSV.type",                    "type"},
        {"CustomerChainGroupCSV.ediFlag",                 "ediFlag"},
        {"CustomerChainGroupCSV.itemMiscTax",             "itemMiscTax"},
        {"CustomerChainGroupCSV.salesLoc",                "salesLocation"},
        {"CustomerChainGroupCSV.territory",               "territory"},
        {"CustomerChainGroupCSV.rep",                     "rep"},
        {"CustomerChainGroupCSV.merchaniser",             "merchandiser"},
        {"CustomerChainGroupCSV.exclusionNumber",         "exclusionNumber"},
        {"CustomerChainGroupCSV.proprietaryDesc",         "proprietaryDesc"},
        {"CustomerChainGroupCSV.arFeeCode",               "arFeeCode"},
        {"CustomerChainGroupCSV.arFeeAmount",             "arFeeAmount"},
        {"CustomerChainGroupCSV.routeInvoicng",           "routeInvoicing"},
        {"CustomerChainGroupCSV.zone",                    "zone"},
        {"CustomerChainGroupCSV.firstInvoiceDate",        "firstInvoiceDate"},
        {"CustomerChainGroupCSV.lastInvoiceDate",         "lastInvoiceDate"},
        {"CustomerChainGroupCSV.terms",                   "terms"},
        {"CustomerChainGroupCSV.shipToName",              "customerName"},
        {"CustomerChainGroupCSV.customerAddress1",        "customerAddress1"},
        {"CustomerChainGroupCSV.customerAddress2",        "customerAddress2"},
        {"CustomerChainGroupCSV.city",                    "city"},
        {"CustomerChainGroupCSV.state",                   "State"},
        {"CustomerChainGroupCSV.zipCode",                 "zipCode"},
        {"CustomerChainGroupCSV.mondayRoute",             "mondayRoute"},
        {"CustomerChainGroupCSV.mondayStop",              "mondayStop"},
        {"CustomerChainGroupCSV.tuesdayRoute",            "tuesdayRoute"},
        {"CustomerChainGroupCSV.tuesdayStop",             "tuesdayStop"},
        {"CustomerChainGroupCSV.wednesdayRoute",          "wednesdayRoute"},
        {"CustomerChainGroupCSV.wednesdayStop",           "wednesdayStop"},
        {"CustomerChainGroupCSV.thursdayRoute",           "thursdayRoute"},
        {"CustomerChainGroupCSV.thursdayStop",            "thursdayStop"},
        {"CustomerChainGroupCSV.fridayRoute",             "fridayRoute"},
        {"CustomerChainGroupCSV.fridayStop",              "fridayStop"},
        {"CustomerChainGroupCSV.saturdayRoute",           "saturdayRoute"},
        {"CustomerChainGroupCSV.saturdayStop",            "saturdayStop"},
        {"CustomerChainGroupCSV.sundayRoute",             "sundayRoute"},
        {"CustomerChainGroupCSV.sundayStop",              "sundayStop"},
        {"CustRoutesTimeWindowCSV.timeWindow1Open",       "win1Start"},
        {"CustRoutesTimeWindowCSV.timeWindow1Close",      "win1Stop"},
        {"CustRoutesTimeWindowCSV.timeWindow2Open",       "win2Start"},
        {"CustRoutesTimeWindowCSV.timeWindow2Close",      "win2Stop"},
        {"CustRoutesTimeWindowCSV.openTime",              "open"},
        {"CustRoutesTimeWindowCSV.closeTime",             "close"}};


    /*
    QString queryString = ("SELECT OrderTrackingCSV.invoiceNumber, "
                           "OrderTrackingCSV.orderTime, "
                           "OrderTrackingCSV.orderDate, "
                           "OrderTrackingCSV.invoiceDate, "
                           "OrderTrackingCSV.rep, "
                           "OrderTrackingCSV.routeKey, "
                           "OrderTrackingCSV.stopNumber, "
                           "RouteProfitabilityCSV.customerNumber, "
                           "RouteProfitabilityCSV.customerName, "
                           "RouteProfitabilityCSV.masterCustomerNumber, "
                           "RouteProfitabilityCSV.pieces, "
                           "RouteProfitabilityCSV.weight, "
                           "RouteProfitabilityCSV.cube, "
                           "RouteProfitabilityCSV.netSales, "
                           "RouteProfitabilityCSV.cost, "
                           "RouteProfitabilityCSV.profit "
                           "FROM OrderTrackingCSV "
                           "INNER JOIN RouteProfitabilityCSV "
                           "ON OrderTrackingCSV.invoiceNumber = RouteProfitabilityCSV.invoiceNumber");
    */

    const QMap<QString, QString> formatInvoice_
    {
        {"OrderTrackingCSV.invoiceNumber",                 "invoiceNumber"},
        {"OrderTrackingCSV.orderTime",                "orderTime"},
        {"OrderTrackingCSV.orderDate",                "orderDate"},
        {"OrderTrackingCSV.invoiceDate",               "invoiceDate"},
        {"OrderTrackingCSV.rep",          "salesRep"},
        {"OrderTrackingCSV.routeKey",             "route"},
        {"OrderTrackingCSV.stopNumber",              "stopNumber"},
        {"RouteProfitabilityCSV.customerNumber",              "customerNumber"},
        {"RouteProfitabilityCSV.masterCustomerNumber",        "masterCustomerNumber"},
        {"RouteProfitabilityCSV.pieces",                 "caseCount"},
        {"RouteProfitabilityCSV.weight",        "weight"},
        {"RouteProfitabilityCSV.cube",                    "caseCube"},
        {"RouteProfitabilityCSV.netSales",                 "netSales"},
        {"RouteProfitabilityCSV.cost",             "productCost"},
        {"RouteProfitabilityCSV.profit",                "profit"}};


signals:

public slots:
};

#endif // MYSQLEXPORTER_H
