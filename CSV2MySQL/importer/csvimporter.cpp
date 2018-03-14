#include "csvimporter.h"

CSVImporter::CSVImporter(QObject *parent) : QObject(parent)
{

}

void CSVImporter::setCSVPath(const QString &filePath)
{
    csvPath_ = filePath;
}

QString CSVImporter::getCSVPath()
{
    return csvPath_;
}

QVector<QVariantMap> CSVImporter::import(int fileType, const QString &filePath)
{
    switch(fileType){
    case OrderTrackingHistory:
        return import(orderTrackingCSVMap_, filePath);
    case RouteProfitability:
        return import(routeProfitabilityCSVMap_, filePath);
    }

    return QVector<QVariantMap>();
}

QVector<QVariantMap> CSVImporter::import(const QVariantMap &fileMap, const QString &filePath)
{
    QVector<QVariantMap> csvAsVariantMap;
    QRegularExpression csvParse("(\"([^\"]*)\"|[^,]*)(,|$)");

    QFile csvFile(filePath);

    if(!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << csvFile.errorString();
        return QVector<QVariantMap>();
    }

    while(!csvFile.atEnd())
    {
        QVariantMap copyFileMap = fileMap;
        QRegularExpressionMatchIterator i = csvParse.globalMatch(csvFile.readLine());
        QStringList list;

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString word = match.captured(1);
            list << word.trimmed();
        }
        for(auto key:copyFileMap.keys())
            copyFileMap[key] = list[copyFileMap[key].toInt()];

        csvAsVariantMap.append(copyFileMap);
    }

    return csvAsVariantMap;
}
