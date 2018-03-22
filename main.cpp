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
    QList<QPair<int, QVariantMap>> pairList = importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::OrderTrackingHistory);

    QMap<int, Order> orderMap;
    for(auto pair: pairList)
    {
        orderMap[pair.first] = pair.second;
    }


//    CharliesCSVImporter importer;
//    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.csv");

//    OrderMap oMap = OrderFactory::makeOrders(importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::OrderTrackingHistory));

//    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail.csv");

//    oMap = OrderFactory::addToOrders(oMap, importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::RouteProfitability));

    qDebug() << orderMap.size();

    return a.exec();
}
