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

QList<QStringList> CSVImporter::importAsQStringList(const QString &filePath)
{
    QList<QByteArray> lineList;
    QFile csvFile(filePath);

    if(!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << csvFile.errorString();
        return QList<QStringList>();
    }

    while(!csvFile.atEnd())
        lineList.append(csvFile.readLine());

    QList<QStringList> csvStringLists = QtConcurrent::blockingMapped(lineList, CSVImporter::parseCSVLine);
    lineList.clear();
    return csvStringLists;
}


QList<QVariantMap> CSVImporter::importAsQVariantMap(const QMap<QString, int> &fileMap, const QString &filePath)
{
    QList<QStringList> csvStringLists = CSVImporter::importAsQStringList(filePath);

    //std::bind is very handy here as QtConcurrent can otherwise only use one arg.
    auto csvStringListToCSVVariantMapFunc = std::bind(CSVImporter::csvStringListToCSVVariantMap, std::placeholders::_1, fileMap);
    QList<QVariantMap> variantList = QtConcurrent::blockingMapped(csvStringLists, csvStringListToCSVVariantMapFunc);
    csvStringLists.clear();
    return variantList;
}

QJsonArray CSVImporter::importAsQJsonArray(const QMap<QString, int> &fileMap, const QString &filePath)
{
    QList<QStringList> csvStringList = importAsQStringList(filePath);
    auto csvStringListToQJsonArrayFunc = std::bind(CSVImporter::csvStringListToQJsonArray, std::placeholders::_1, fileMap);
    QJsonArray csvJsonArray = QtConcurrent::blockingMappedReduced(csvStringList, csvStringListToQJsonArrayFunc, &QJsonArray::append);
    csvStringList.clear();

    return csvJsonArray;
}

QList<QPair<int, QVariantMap>> CSVImporter::importAsPair(const QMap<QString, int> &fileMap, const QString &filePath, const QString &keyIdentifier)
{
    QList<QStringList> csvStringList = importAsQStringList(filePath);
    auto csvStringListToPair = std::bind(CSVImporter::csvStringListToCSVPair, std::placeholders::_1, fileMap, keyIdentifier);
    QList<QPair<int, QVariantMap>> csvPairList = QtConcurrent::blockingMapped(csvStringList, csvStringListToPair);
    csvStringList.clear();

    return csvPairList;
}

QStringList CSVImporter::parseCSVLine(const QByteArray &csvLine)
{
    QRegularExpression csvParse("(\"([^\"]*)\"|[^,]*)(,|$)");
    QRegularExpressionMatchIterator i = csvParse.globalMatch(csvLine);
    QStringList matchList;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        word = word.remove("\"");
        matchList << word.trimmed();
    }
    return matchList;
}

QVariantMap CSVImporter::csvStringListToCSVVariantMap(const QStringList &csvStringList, const QMap<QString, int> &columnMap)
{
    QVariantMap csvMappedData;

    if(columnMap.keys().size() > csvStringList.size())
    {
        qDebug() << "Error, more keys in csv column map than there are strings"
                    " in string list. Returning empty QVariantMap";
        return QVariantMap();
    }

    for(auto key:columnMap.keys())
        csvMappedData[key] = csvStringList[columnMap[key]];
    return csvMappedData;
}

QPair<int, QVariantMap> CSVImporter::csvStringListToCSVPair(const QStringList &csvStringList, const QMap<QString, int> &columnMap, const QString &keyIdentifier)
{
    QVariantMap csvMappedData;

    if(columnMap.keys().size() > csvStringList.size())
    {
        qDebug() << "Error, more keys in csv column map than there are strings"
                    " in string list. Returning empty QVariantMap";
        return QPair<int, QVariantMap>();
    }

    for(auto key:columnMap.keys())
        csvMappedData[key] = csvStringList[columnMap[key]];

    return QPair<int, QVariantMap>(csvMappedData[keyIdentifier].toInt(), csvMappedData);
}


QJsonObject CSVImporter::csvStringListToQJsonArray(const QStringList &csvStringList, const QMap<QString, int> &columnMap)
{
    QJsonObject csvJsonObject;
    if(columnMap.keys().size() > csvStringList.size())
    {
        qDebug() << "Error, more keys in csv column map than there are strings"
                    " in string list. Returning empty QVariantMap";
        return QJsonObject();
    }

    for(auto key:columnMap.keys())
        csvJsonObject[key] = csvStringList[columnMap[key]];

    return csvJsonObject;
}
