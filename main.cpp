#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"

#include <QApplication>


//QMap<int, Order> filter(QMap<int, Order> &result, QList<int> &sharedKeys)
//{
//    qDebug << "potato";
//}

int getInvoiceNumbers(QPair<int, QVariantMap> pair)
{
    return pair.first;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CharliesCSVImporter importer;
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.csv");
    QList<QPair<int, QVariantMap>> pairList = importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::OrderTrackingHistory);
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail.csv");
    QList<QPair<int, QVariantMap>> pairList2 = importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::RouteProfitability);

    //Surely this is a better way to do this.
    QList<int> keys0 = QtConcurrent::blockingMapped(pairList, getInvoiceNumbers);
    QList<int> keys1 = QtConcurrent::blockingMapped(pairList2, getInvoiceNumbers);

    QList<int> sharedKeys;

    QList<int> deleteFromKeys0;
    QList<int> deleteFromKeys1;

    std::sort(keys0.begin(), keys0.end());
    std::sort(keys1.begin(), keys1.end());
    std::set_intersection(keys0.begin(), keys0.end(), keys1.begin(), keys1.end(), std::back_inserter(sharedKeys));
    std::sort(sharedKeys.begin(), sharedKeys.end());
    std::set_difference(keys0.begin(), keys0.end(), sharedKeys.begin(), sharedKeys.end(), std::back_inserter(deleteFromKeys0));
    std::set_difference(keys1.begin(), keys1.end(), sharedKeys.begin(), sharedKeys.end(), std::back_inserter(deleteFromKeys1));
    //End section that is tantalizingly improveable.

    pairList.append(pairList2);

    QMap<int, Order> orderMap;
    for(auto pair: pairList)
        orderMap[pair.first].addInformation(pair.second);
    deleteFromKeys0.append(deleteFromKeys1);
    std::sort(deleteFromKeys0.begin(), deleteFromKeys0.end());
    for(auto key:deleteFromKeys0)
        orderMap.remove(key);

    qDebug() << orderMap.size();

    return a.exec();
}
