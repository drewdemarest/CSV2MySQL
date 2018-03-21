#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/order.h"
#include <QApplication>

Order makeOrderFromVarMap(const QVariantMap &vMap)
{
    return Order(vMap);
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

    //QJsonArray bok = importer.importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes::OrderTrackingHistory);

    //importer.saveArray(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.db", "orderTrackingHistory", "invoiceNumber", bok);
    qDebug() << bok.size() << bok[10];
    // bok = QJsonArray();

    return a.exec();
}
