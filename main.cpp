#include "mainwindow.h"
#include "importer/charliescsvimporter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CharliesCSVImporter importer;
    importer.setCSVPath(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.csv");

    QJsonArray bok = importer.importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes::OrderTrackingHistory);
    importer.saveArray(QCoreApplication::applicationDirPath() + "/orderTrackingHistory.db", "orderTrackingHistory", "invoiceNumber", bok);
    qDebug() << bok.size() << bok[10];
    bok = QJsonArray();

    return a.exec();
}
