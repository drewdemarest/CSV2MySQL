#include "charliescsvimporter.h"

CharliesCSVImporter::CharliesCSVImporter() : CSVImporter(), Json2Sqlite()
{

}

QList<QVariantMap> CharliesCSVImporter::importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes reportType)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsQVariantMap(orderTrackingCSVMap_, csvPath_);
    case RouteProfitability:
        return importAsQVariantMap(routeProfitabilityCSVMap_, csvPath_);
    }

    return QList<QVariantMap>();
}

QList<QVariantMap> CharliesCSVImporter::importCharliesCSVAsQVariantMap(CharliesCSVImporter::reportTypes reportType, const QString &csvPath)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsQVariantMap(orderTrackingCSVMap_, csvPath);
    case RouteProfitability:
        return importAsQVariantMap(routeProfitabilityCSVMap_, csvPath);
    }

    return QList<QVariantMap>();
}

QList<QPair<int, QVariantMap> > CharliesCSVImporter::importCharliesCSVAsPair(CharliesCSVImporter::reportTypes reportType)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsPair(orderTrackingCSVMap_, csvPath_, "invoiceNumber");
    case RouteProfitability:
        return importAsPair(routeProfitabilityCSVMap_, csvPath_,  "invoiceNumber");
    }

    return QList<QPair<int, QVariantMap>>();
}

QList<QPair<int, QVariantMap>> CharliesCSVImporter::importCharliesCSVAsPair(CharliesCSVImporter::reportTypes reportType, const QString &csvPath)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsPair(orderTrackingCSVMap_, csvPath,  "invoiceNumber");
    case RouteProfitability:
        return importAsPair(routeProfitabilityCSVMap_, csvPath,  "invoiceNumber");
    }

    return QList<QPair<int, QVariantMap>>();
}

QJsonArray CharliesCSVImporter::importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes reportType)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsQJsonArray(orderTrackingCSVMap_, csvPath_);
    case RouteProfitability:
        return importAsQJsonArray(routeProfitabilityCSVMap_, csvPath_);
    }

    return QJsonArray();
}

QJsonArray CharliesCSVImporter::importCharliesCSVAsQJsonArray(CharliesCSVImporter::reportTypes reportType, const QString &csvPath)
{
    switch(reportType){
    case OrderTrackingHistory:
        return importAsQJsonArray(orderTrackingCSVMap_, csvPath);;
    case RouteProfitability:
        return importAsQJsonArray(routeProfitabilityCSVMap_, csvPath);
    }

    return QJsonArray();
}


