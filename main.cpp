#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include "entity/order/order.h"
#include <QApplication>
#include <QMutex>

typedef QMap<int, Order> OrderMap;

void combineOrders(const Order &o, QMap<int, Order> &oMap)
{
        if(oMap.contains(o.getInvoiceNumber()))
        {
            qDebug() << o.getInvoiceNumber();
            oMap[o.getInvoiceNumber()].addInformation(o.extractInformation());
        }
}


OrderMap makeOrderFromVarMap(const QVariantMap &vMap)
{
    OrderMap oMap;
    Order order = Order(vMap);
    oMap[order.getInvoiceNumber()] = order;
    return oMap;
}

void reduce(OrderMap &result, const OrderMap &oMap)
{
    QMapIterator<int, Order> i(oMap);
    while(i.hasNext())
    {
        i.next();
        result[i.key()] = i.value();
    }
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

    QList<QVariantMap> vMapImport = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::OrderTrackingHistory);
    OrderMap oMap = QtConcurrent::blockingMappedReduced(vMapImport, makeOrderFromVarMap, reduce);

    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/routeProfitabilityDetail.csv");
    vMapImport.clear();
    vMapImport = importer.importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes::RouteProfitability);
    OrderMap oMap2 = QtConcurrent::blockingMappedReduced(vMapImport, makeOrderFromVarMap, reduce);

    QList<int> keys = oMap.keys();
    keys.append(oMap2.keys());
    QSet<int> sharedKeys = keys.toSet();

    for(auto key: sharedKeys)
    {
        oMap[key].addInformation(oMap2[key].extractInformation());
        oMap2.remove(key);
    }

    oMap2.clear();

    for(auto key:oMap.keys())
        if(!oMap[key].neededKeys().isEmpty())
            oMap.remove(key);

    qDebug() << oMap.size();
    qDebug() << oMap.begin().value().neededKeys();
    qDebug() << oMap2.size();
    return a.exec();
}
