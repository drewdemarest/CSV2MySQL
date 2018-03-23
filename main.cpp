#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/orderfactory.h"
#include "entity/order/order.h"
#include <QApplication>

int getInvoiceNumbers(QPair<int, QVariantMap> pair)
{
    return pair.first;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QList<QList<QPair<int, QVariantMap>>> importResults;
    QList<QList<int>> keys;
    QList<QList<int>> deleteFromKeys;
    QList<int> sharedKeys;
    QList<int> tempKeys;


    CharliesCSVImporter importer;

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderHistorySummary0.csv");
    importResults.append(importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::OrderTrackingHistory));

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderHistorySummary1.csv");
    importResults.append(importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::OrderTrackingHistory));

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderHistorySummary2.csv");
    importResults.append(importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::OrderTrackingHistory));

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail0.csv");
    importResults.append(importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::RouteProfitability));

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail1.csv");
    importResults.append(importer.importCharliesCSVAsPair(CharliesCSVImporter::reportTypes::RouteProfitability));

    for(auto importResult: importResults)
        keys.append(QtConcurrent::blockingMapped(importResult, getInvoiceNumbers));

    for(int i = 0; i < keys.length(); ++i)
        std::sort(keys[i].begin(), keys[i].end());

    sharedKeys = *keys.begin();
    for(int i = 0; i < keys.length() - 1 ; ++i)
    {
        std::set_intersection(sharedKeys.begin(), sharedKeys.end(), keys[i+1].begin(), keys[i+1].end(), std::back_inserter(tempKeys));
        sharedKeys = tempKeys;
        tempKeys.clear();
    }
    std::sort(sharedKeys.begin(), sharedKeys.end());
    qDebug() << sharedKeys.size();
    for(int i = 0; i < keys.length(); ++i)
    {
        deleteFromKeys.append(QList<int>());
        std::set_difference(keys[i].begin(), keys[i].end(), sharedKeys.begin(), sharedKeys.end(), std::back_inserter(deleteFromKeys[i]));
    }

    QList<QPair<int, QVariantMap>> completeImport;
    QList<int> completeDelete;
    for(auto importResult: importResults)
        completeImport.append(importResult);

    for(auto deleteKeys:deleteFromKeys)
        completeDelete.append(deleteKeys);


    QMap<int, Order> orderMap;
    for(auto pair: completeImport)
        orderMap[pair.first].addInformation(pair.second);

    std::sort(completeDelete.begin(), completeDelete.end());

    for(auto key:completeDelete)
        orderMap.remove(key);

    qDebug() << orderMap.size();
    for(auto order:orderMap)
        qDebug() << order.extractInformation();


    return a.exec();
}
