#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"
#include "entity/order/order.h"
//#include "cachedb/ordersqlite.h"
#include "cachedb/sqlitedbthread.h"
#include <QApplication>
#include <malloc.h>
#include <unistd.h>

int getInvoiceNumbers(QPair<int, QVariantMap> pair)
{
    return pair.first;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qRegisterMetaType<DataInfo>();
    const QMap<QString, DataInfo> formatOrderTrackingCSV_
    {{"cdl",                DataInfo(0,1,"INTEGER")},
    {"orderType",           DataInfo(1,2,"TEXT")},
    {"orderDate",           DataInfo(2,3,"TEXT", true, false, "MM/dd/yy", 0, 100)},
    {"invoiceDate",         DataInfo(3,4,"TEXT", true, false, "MM/dd/yy", 0, 100)},
    {"orderTime",           DataInfo(4,5,"TEXT", false, true, "HH:mm:ss", 0, 0)},
    {"rep",                 DataInfo(5,6,"TEXT")},
    {"terms",               DataInfo(6,7,"TEXT")},
    {"customerNumber",      DataInfo(7,8,"INTEGER")},
    {"customerName",        DataInfo(8,9,"TEXT")},
    {"customerBillingName", DataInfo(9,10,"TEXT")},
    {"routeKey",            DataInfo(10,11,"TEXT")},
    {"stopNumber",          DataInfo(11,12,"INTEGER")},
    {"specialInstructions", DataInfo(12,13,"TEXT")},
    {"poNumber",            DataInfo(13,14,"TEXT")},
    {"invoiceNumber",       DataInfo(14,0,"INTEGER PRIMARY KEY")},
    {"quantityShipped",     DataInfo(15,15,"REAL")},
    {"extSales",            DataInfo(16,16,"REAL")},
    {"invoiceAmmount",      DataInfo(17,17,"REAL")},
    {"user",                DataInfo(18,18,"TEXT")},
    {"user2",               DataInfo(19,19,"TEXT")}};

    SQLiteDBThread dbThread(QCoreApplication::applicationDirPath() + "/potato.db");
    dbThread.operate(formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 200000);
//    OrderSQLite sqliteDB(QCoreApplication::applicationDirPath() + "/potato.db");
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatRouteProfitabilityCSV_, "RouteProfitabilityCSV", true, QCoreApplication::applicationDirPath() + "/routeProfitabilityDetailYear.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatTruckDriverAssignCSV_, "TruckDriverAssignCSV", true, QCoreApplication::applicationDirPath() + "/truckDriverAssign0.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatCustRoutesTimeWindowCSV_, "CustRoutesTimeWindowCSV", true, QCoreApplication::applicationDirPath() + "/custWin0.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatCustomerChainGroupCSV_, "CustomerChainGroupCSV", true, QCoreApplication::applicationDirPath() + "/customerChainGroup0.csv", 200000);

    //sqliteDB.populateOrders();

    return a.exec();
}
