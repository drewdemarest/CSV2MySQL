#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/order.h"
#include <QApplication>

Order makeOrderFromVarMap(const QVariantMap &vMap)
{
    return Order(vMap);
}

int getInvoiceNumFromOrder(Order o)
{
    return o.getInvoiceNumber();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CharliesCSVImporter importer;
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.csv");

    QList<QVariantMap> bok = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::OrderTrackingHistory);
    QList<Order> orders = QtConcurrent::blockingMapped(bok, makeOrderFromVarMap);
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail.csv");
    bok.clear();
    bok = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::RouteProfitability);
    QList<Order> orders2 = QtConcurrent::blockingMapped(bok, makeOrderFromVarMap);
    QList<int> orderInvoiceNums = QtConcurrent::blockingMapped(orders, getInvoiceNumFromOrder);
    QList<int> order2InvoiceNums = QtConcurrent::blockingMapped(orders2, getInvoiceNumFromOrder);
    QList<int> commonInvoiceNums;
    std::sort(orderInvoiceNums.begin(), orderInvoiceNums.end());
    std::sort(order2InvoiceNums.begin(), order2InvoiceNums.end());
    std::set_intersection(orderInvoiceNums.begin(), orderInvoiceNums.end(), order2InvoiceNums.begin(), order2InvoiceNums.end(), std::back_inserter(commonInvoiceNums));

    qDebug() << commonInvoiceNums.size();


    //QJsonArray bok = importer.importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes::OrderTrackingHistory);

    //importer.saveArray(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.db", "orderTrackingHistory", "invoiceNumber", bok);
    qDebug() << bok.size() << bok[10];
    qDebug() << orders[10].neededKeys();
    qDebug() << orders2[10].neededKeys();

    // bok = QJsonArray();

    return a.exec();
}
