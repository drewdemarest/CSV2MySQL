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
    {{"cdl",                    DataInfo(0,1, SQLiteType::INTEGER)},
        {"orderType",           DataInfo(1,2,SQLiteType::TEXT)},
        {"orderDate",           DataInfo(2,3,SQLiteType::TEXT_DATE,false, "MM/dd/yy", 100)},
        {"invoiceDate",         DataInfo(3,4,SQLiteType::TEXT_DATE,false, "MM/dd/yy", 100)},
        {"orderTime",           DataInfo(4,5,SQLiteType::TEXT_TIME,false, "HH:mm:ss", 0, 0)},
        {"rep",                 DataInfo(5,6,SQLiteType::TEXT)},
        {"terms",               DataInfo(6,7,SQLiteType::TEXT)},
        {"customerNumber",      DataInfo(7,8,SQLiteType::INTEGER)},
        {"customerName",        DataInfo(8,9,SQLiteType::TEXT)},
        {"customerBillingName", DataInfo(9,10,SQLiteType::TEXT)},
        {"routeKey",            DataInfo(10,11,SQLiteType::TEXT)},
        {"stopNumber",          DataInfo(11,12,SQLiteType::INTEGER)},
        {"specialInstructions", DataInfo(12,13,SQLiteType::TEXT)},
        {"poNumber",            DataInfo(13,14,SQLiteType::TEXT)},
        {"invoiceNumber",       DataInfo(14,0,SQLiteType::INTEGER, true)},
        {"quantityShipped",     DataInfo(15,15,SQLiteType::REAL)},
        {"extSales",            DataInfo(16,16,SQLiteType::REAL)},
        {"invoiceAmmount",      DataInfo(17,17,SQLiteType::REAL)},
        {"user",                DataInfo(18,18,SQLiteType::TEXT)},
        {"user2",               DataInfo(19,19,SQLiteType::TEXT)}};

    SQLiteDBThread dbThread(QCoreApplication::applicationDirPath() + "/potato.db");
    dbThread.operate(formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 60000);
//    OrderSQLite sqliteDB(QCoreApplication::applicationDirPath() + "/potato.db");
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatRouteProfitabilityCSV_, "RouteProfitabilityCSV", true, QCoreApplication::applicationDirPath() + "/routeProfitabilityDetailYear.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatTruckDriverAssignCSV_, "TruckDriverAssignCSV", true, QCoreApplication::applicationDirPath() + "/truckDriverAssign0.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatCustRoutesTimeWindowCSV_, "CustRoutesTimeWindowCSV", true, QCoreApplication::applicationDirPath() + "/custWin0.csv", 200000);
//    sqliteDB.importCSVtoSQLite(sqliteDB.formatCustomerChainGroupCSV_, "CustomerChainGroupCSV", true, QCoreApplication::applicationDirPath() + "/customerChainGroup0.csv", 200000);

    //sqliteDB.populateOrders();

    return a.exec();
}
