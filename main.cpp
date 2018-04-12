#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"
#include "entity/order/order.h"
#include "cachedb/ordersqlite.h"
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

    OrderSQLite sqliteDB(QCoreApplication::applicationDirPath() + "/potato.db");
    sqliteDB.importCSVtoSQLite(sqliteDB.formatOrderTrackingCSV_, "OrderTrackingCSV", false, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 100000);
    sqliteDB.importCSVtoSQLite(sqliteDB.formatRouteProfitabilityCSV_, "RouteProfitabilityCSV", true, QCoreApplication::applicationDirPath() + "/routeProfitabilityDetailYear.csv", 100000);
    sqliteDB.importCSVtoSQLite(sqliteDB.formatTruckDriverAssignCSV_, "TruckDriverAssignCSV", true, QCoreApplication::applicationDirPath() + "/truckDriverAssign0.csv", 100000);
    sqliteDB.importCSVtoSQLite(sqliteDB.formatCustRoutesTimeWindowCSV_, "CustRoutesTimeWindowCSV", true, QCoreApplication::applicationDirPath() + "/custWin0.csv", 100000);
    malloc_trim(0);
    //sqliteDB.populateOrders();

    return a.exec();
}
