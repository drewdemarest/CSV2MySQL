#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"

#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CharliesCSVImporter importer;
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.csv");
    QList<QVariantMap> vMapImport1 = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::OrderTrackingHistory);
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail.csv");
    QList<QVariantMap> vMapImport2 = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::RouteProfitability);
    OrderMap oMap = OrderFactory::makeOrders(vMapImport1, vMapImport2);
    qDebug() << oMap.size();

    return a.exec();
}
