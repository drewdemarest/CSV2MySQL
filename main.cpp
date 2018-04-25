#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"
#include "entity/order/order.h"
//#include "cachedb/ordersqlite.h"
#include "cachedb/sqlitedbthread.h"
#include <QApplication>
#include <malloc.h>
//#include <unistd.h>

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
    const DataInfoMap formatOrderTrackingCSV_
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

    const DataInfoMap formatRouteProfitabilityCSV_
    {{"routeKey",               DataInfo(0,1, SQLiteType::TEXT)},
        {"customerNumber",          DataInfo(1,2, SQLiteType::INTEGER)},
        {"customerName",            DataInfo(2,3, SQLiteType::TEXT)},
        {"masterCustomerNumber",    DataInfo(3,4, SQLiteType::INTEGER)},
        {"invoiceDate",             DataInfo(4,5, SQLiteType::TEXT_DATE, false, "MM/dd/yy", 100)},
        {"invoiceNumber",           DataInfo(5,0, SQLiteType::INTEGER, true)},
        {"numberOfOrders",          DataInfo(6,6, SQLiteType::INTEGER)},
        {"pieces",                  DataInfo(7,7, SQLiteType::REAL)},
        {"weight",                  DataInfo(8,8, SQLiteType::REAL)},
        {"cube",                    DataInfo(9,9, SQLiteType::REAL)},
        {"netSales",                DataInfo(10,10,SQLiteType::REAL)},
        {"cost",                    DataInfo(11,11,SQLiteType::REAL)},
        {"profit",                  DataInfo(12,12,SQLiteType::REAL)},
        {"profitPercent",           DataInfo(13,13,SQLiteType::REAL)}};

    const DataInfoMap formatOrderObj_
    {{"invoiceNumber",        DataInfo(0,0,SQLiteType::INTEGER, true)},
        {"customerNumber",        DataInfo(1,1, SQLiteType::INTEGER)},
        {"customerName",          DataInfo(2,2,SQLiteType::TEXT)},
        {"masterCustomerNumber",  DataInfo(3,3,SQLiteType::INTEGER)},
        {"routeKey",              DataInfo(4,4,SQLiteType::TEXT)},
        {"stopNumber",            DataInfo(5,5,SQLiteType::INTEGER)},
        {"orderDateTime",         DataInfo(6,6,SQLiteType::TEXT)},
        {"invoiceDate",           DataInfo(7,7,SQLiteType::TEXT_DATE,false, "MM/dd/yy", 100)},
        {"pieces",                DataInfo(8,8,SQLiteType::REAL)},
        {"weight",                DataInfo(9,9,SQLiteType::REAL)},
        {"cube",                  DataInfo(10,10,SQLiteType::REAL)},
        {"netSales",              DataInfo(11,11,SQLiteType::REAL)},
        {"cost",                  DataInfo(12,12,SQLiteType::REAL)},
        {"profit",                DataInfo(13,13,SQLiteType::REAL)}};

    const DataInfoMap formatTruckDriverAssignCSV_
    {{"division",  DataInfo(0,1, SQLiteType::TEXT)},
        {"routeKey",   DataInfo(1,0, SQLiteType::TEXT)},
        {"routeName",  DataInfo(2,2, SQLiteType::TEXT)},
        {"whs1Cutoff", DataInfo(3,3, SQLiteType::TEXT_TIME,false, "HH:mm")},
        {"whs2Cutoff", DataInfo(4,4, SQLiteType::TEXT_TIME,false, "HH:mm")},
        {"truck",      DataInfo(5,5, SQLiteType::TEXT)},
        {"driverID",   DataInfo(6,6,SQLiteType::INTEGER)},
        {"driverName", DataInfo(7,7,SQLiteType::TEXT)}};

    const DataInfoMap formatCustRoutesTimeWindowCSV_
    {{"division",           DataInfo(0, 1, SQLiteType::TEXT)},
        {"rep",                 DataInfo(1, 2, SQLiteType::TEXT)},
        {"customerNumber",      DataInfo(2, 0, SQLiteType::INTEGER, true)},
        {"customerName",        DataInfo(3, 3, SQLiteType::TEXT)},
        {"customerAddr1",       DataInfo(4, 4, SQLiteType::TEXT)},
        {"customerAddr2",       DataInfo(5, 5, SQLiteType::TEXT)},
        {"city",                DataInfo(6, 6, SQLiteType::TEXT)},
        {"state",               DataInfo(7, 7, SQLiteType::TEXT)},
        {"lastOrder",           DataInfo(8, 8, SQLiteType::TEXT_DATE,false,"MM/dd/yy", 100)},
        {"customerChain",       DataInfo(9, 9, SQLiteType::TEXT)},
        {"cstuomerGroup",       DataInfo(10, 10, SQLiteType::TEXT)},
        {"timeWindow1Open",     DataInfo(11, 11, SQLiteType::TEXT_TIME,false,"hmm")},
        {"timeWindow1Close",    DataInfo(12, 12, SQLiteType::TEXT_TIME,false,"hmm")},
        {"mondayRoute",         DataInfo(13, 13, SQLiteType::TEXT)},
        {"mondayStop",          DataInfo(14, 14, SQLiteType::INTEGER)},
        {"tuesdayRoute",        DataInfo(15, 15, SQLiteType::TEXT)},
        {"tuesdayStop",         DataInfo(16, 16, SQLiteType::INTEGER)},
        {"wednesdayRoute",      DataInfo(17, 17, SQLiteType::TEXT)},
        {"wednesdayStop",       DataInfo(18, 18, SQLiteType::INTEGER)},
        {"thursdayRoute",       DataInfo(19, 19, SQLiteType::TEXT)},
        {"thursdayStop",        DataInfo(20, 20, SQLiteType::INTEGER)},
        {"fridayRoute",         DataInfo(21, 21, SQLiteType::TEXT)},
        {"fridayStop",          DataInfo(22, 22, SQLiteType::INTEGER)},
        {"saturdayRoute",       DataInfo(23, 23, SQLiteType::TEXT)},
        {"saturdayStop",        DataInfo(24, 24, SQLiteType::INTEGER)},
        {"sundayRoute",         DataInfo(25, 25, SQLiteType::TEXT)},
        {"sundayStop",          DataInfo(26, 26, SQLiteType::INTEGER)},
        {"timeWindow2Open",     DataInfo(27, 27, SQLiteType::TEXT_TIME,false, "hmm")},
        {"timeWindow2Close",    DataInfo(28, 28, SQLiteType::TEXT_TIME,false, "hmm")},
        {"openTime",            DataInfo(29, 29, SQLiteType::TEXT_TIME,false, "hmm")},
        {"closeTime",           DataInfo(30, 30, SQLiteType::TEXT_TIME,false, "hmm")}};

    const DataInfoMap formatCustomerChainGroupCSV_
    {
        {"company",                 DataInfo(0,1, SQLiteType::INTEGER)},
        {"division",                DataInfo(1,2,SQLiteType::INTEGER)},
        {"location",                DataInfo(2,3,SQLiteType::INTEGER)},
        {"warehouse",               DataInfo(3,4,SQLiteType::INTEGER)},
        {"customerNumber",          DataInfo(4,0,SQLiteType::INTEGER, true)},
        {"storeNumber",             DataInfo(5,5,SQLiteType::TEXT)},
        {"stopNumber",              DataInfo(6,6,SQLiteType::INTEGER)},
        {"phoneNumber",             DataInfo(7,7,SQLiteType::TEXT)},
        {"billToName",              DataInfo(8,8,SQLiteType::TEXT)},
        {"chainID",                 DataInfo(9,9, SQLiteType::TEXT)},
        {"chainDescription",        DataInfo(10,10, SQLiteType::TEXT)},
        {"groupID",                 DataInfo(11,11, SQLiteType::TEXT)},
        {"groupDescription",        DataInfo(12,12, SQLiteType::TEXT)},
        {"type",                    DataInfo(13,13,SQLiteType::INTEGER)},
        {"ediFlag",                 DataInfo(14,14,SQLiteType::TEXT)},
        {"itemMiscTax",             DataInfo(15,15, SQLiteType::TEXT)},
        {"poRequired",              DataInfo(16,16,SQLiteType::TEXT)},
        {"salesLoc",                DataInfo(17,17, SQLiteType::TEXT)},
        {"territory",               DataInfo(18,18,SQLiteType::TEXT)},
        {"rep",                     DataInfo(19,19,SQLiteType::TEXT)},
        {"merchaniser",             DataInfo(20,20,SQLiteType::TEXT)},
        {"exclusionNumber",         DataInfo(21,21,SQLiteType::TEXT)},
        {"proprietaryDesc",         DataInfo(22,22,SQLiteType::TEXT)},
        {"arFeeCode",               DataInfo(23,23,SQLiteType::TEXT)},
        {"arFeeAmount",             DataInfo(24,24,SQLiteType::TEXT)},
        {"routeInvoicng",           DataInfo(25,25,SQLiteType::TEXT)},
        {"zone",                    DataInfo(26,26,SQLiteType::TEXT)},
        {"guide",                   DataInfo(27,27,SQLiteType::TEXT)},
        {"authorized",              DataInfo(28,28,SQLiteType::TEXT)},
        {"subs",                    DataInfo(29,29,SQLiteType::TEXT)},
        {"autoUpdateGuide",         DataInfo(30,30,SQLiteType::TEXT)},
        {"cool",                    DataInfo(31,31,SQLiteType::TEXT)},
        {"distributionFeeCode",     DataInfo(32,32,SQLiteType::TEXT)},
        {"firstInvoiceDate",        DataInfo(33,33,SQLiteType::TEXT_DATE,false, "yyyyMMdd")},
        {"lastInvoiceDate",         DataInfo(34,34,SQLiteType::TEXT_DATE,false, "yyyyMMdd")},
        {"terms",                   DataInfo(35,35,SQLiteType::TEXT)},
        {"shipToName",              DataInfo(36,36,SQLiteType::TEXT)},
        {"customerAddress1",        DataInfo(37,37,SQLiteType::TEXT)},
        {"customerAddress2",        DataInfo(38,38,SQLiteType::TEXT)},
        {"city",                    DataInfo(39,39,SQLiteType::TEXT)},
        {"state",                   DataInfo(40,40,SQLiteType::TEXT)},
        {"zipCode",                 DataInfo(41,41,SQLiteType::TEXT)},
        {"mondayRoute",             DataInfo(42, 42, SQLiteType::TEXT)},
        {"mondayStop",              DataInfo(43, 43, SQLiteType::INTEGER)},
        {"tuesdayRoute",            DataInfo(44, 44, SQLiteType::TEXT)},
        {"tuesdayStop",             DataInfo(45, 45, SQLiteType::INTEGER)},
        {"wednesdayRoute",          DataInfo(46, 46, SQLiteType::TEXT)},
        {"wednesdayStop",           DataInfo(47, 47, SQLiteType::INTEGER)},
        {"thursdayRoute",           DataInfo(48, 48, SQLiteType::TEXT)},
        {"thursdayStop",            DataInfo(49, 49, SQLiteType::INTEGER)},
        {"fridayRoute",             DataInfo(50, 50, SQLiteType::TEXT)},
        {"fridayStop",              DataInfo(51, 51, SQLiteType::INTEGER)},
        {"saturdayRoute",           DataInfo(52, 52, SQLiteType::TEXT)},
        {"saturdayStop",            DataInfo(53, 53, SQLiteType::INTEGER)},
        {"sundayRoute",             DataInfo(54, 54, SQLiteType::TEXT)},
        {"sundayStop",              DataInfo(55, 55, SQLiteType::INTEGER)},
        {"salesCurrent",            DataInfo(56,56,SQLiteType::REAL)},
        {"custCurrent",             DataInfo(57,57,SQLiteType::REAL)},
        {"profitCurrent",           DataInfo(58,58,SQLiteType::REAL)},
        {"profitPercentCurrent",    DataInfo(59,59,SQLiteType::REAL)},
        {"salesPrevious",           DataInfo(60,60,SQLiteType::REAL)},
        {"costPrevious",            DataInfo(61,61,SQLiteType::REAL)},
        {"profitPrevious",          DataInfo(62,62,SQLiteType::REAL)},
        {"profitPercentPrevious",   DataInfo(63,63,SQLiteType::REAL)}};

    SQLiteDBThread dbThread(QCoreApplication::applicationDirPath() + "/potato.db");
    //dbThread.importFromThread(formats.formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistoryYear.csv", 60000);
    dbThread.importFromThread(formatOrderTrackingCSV_, "OrderTrackingCSV", true, QCoreApplication::applicationDirPath() + "/orderTrackingHistory1.csv", 60000);
    dbThread.importFromThread(formatRouteProfitabilityCSV_, "RouteProfitabilityCSV", true, QCoreApplication::applicationDirPath() + "/routeProfitabilityDetailYear.csv", 60000);
    dbThread.importFromThread(formatTruckDriverAssignCSV_, "TruckDriverAssignCSV", true, QCoreApplication::applicationDirPath() + "/truckDriverAssign0.csv", 60000);
    dbThread.importFromThread(formatCustRoutesTimeWindowCSV_, "CustRoutesTimeWindowCSV", true, QCoreApplication::applicationDirPath() + "/custWin0.csv", 60000);
    dbThread.importFromThread(formatCustomerChainGroupCSV_, "CustomerChainGroupCSV", true, QCoreApplication::applicationDirPath() + "/customerChainGroup0.csv", 60000);

    //sqliteDB.populateOrders();

    return a.exec();
}
