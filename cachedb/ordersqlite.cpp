#include "ordersqlite.h"

//static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
//    int i;
//    for(i = 0; i<argc; i++) {
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
//}

OrderSQLite::OrderSQLite(const QString &dbPath, QObject *parent) : QObject(parent)
{
    dbPath_ = dbPath;

    if(!doesDatabaseExist())
        makeOrderDB();
}

bool OrderSQLite::doesDatabaseExist() const
{
    QFileInfo checkFile(dbPath_);
    if(checkFile.exists() && checkFile.isFile())
        return true;
    else
        return false;
}

//QList<QVariantMap> OrderSQLite::getOrdersFromDB(const int minInvoiceNum, const int maxInvoiceNum) const
//{
//    return QList<QVariantMap>();
//}

//QList<QVariantMap> OrderSQLite::getOrdersFromDB(const QDate &minOrderDate, const QDate &maxOrderDate) const
//{
//    return QList<QVariantMap>();
//}

TableInfoMap OrderSQLite::getKnownFormats() const
{
    return knownFormats_;
}

void OrderSQLite::addFormat(const QString &tableName, const DataInfoMap &dim)
{
    knownFormats_[tableName] = dim;
}

bool OrderSQLite::populateOrders() const
{    //Scope to make sure everything is clean regarding DB connections.

    bool success;
    QSqlDatabase database;
    database = database.addDatabase("QSQLITE", "orders");
    database.setDatabaseName(dbPath_);

    if(!database.open())
    {
        qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
        return false;
    }


    QString queryString = ("SELECT OrderTrackingCSV.invoiceNumber, "
                           "OrderTrackingCSV.orderTime, "
                           "OrderTrackingCSV.orderDate, "
                           "OrderTrackingCSV.invoiceDate, "
                           "OrderTrackingCSV.rep, "
                           "OrderTrackingCSV.routeKey, "
                           "OrderTrackingCSV.stopNumber, "
                           "RouteProfitabilityCSV.customerNumber, "
                           "RouteProfitabilityCSV.customerName, "
                           "RouteProfitabilityCSV.masterCustomerNumber, "
                           "RouteProfitabilityCSV.pieces, "
                           "RouteProfitabilityCSV.weight, "
                           "RouteProfitabilityCSV.cube, "
                           "RouteProfitabilityCSV.netSales, "
                           "RouteProfitabilityCSV.cost, "
                           "RouteProfitabilityCSV.profit "
                           "FROM OrderTrackingCSV "
                           "INNER JOIN RouteProfitabilityCSV "
                           "ON OrderTrackingCSV.invoiceNumber = RouteProfitabilityCSV.invoiceNumber");


    QSqlQuery query(database);
    qDebug() << queryString;
    query.prepare(queryString);
    success = query.exec();
    if(!success)
        qDebug() << query.lastError();

    //    while(query.next()){
    //        //qDebug() << query.record().count();
    //        QStringList queryResult;
    //        for(int j = 0; j < query.record().count(); ++j)
    //            queryResult.append(query.value(j).toString());
    //        //qDebug() << queryResult;
    //    }

    query.clear();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase("orders");
    return success;
}



bool OrderSQLite::makeOrderDB()
{
    bool success;
    QStringList pragmas;
    pragmas << "PRAGMA synchronous = OFF"
            << "PRAGMA journal_mode = MEMORY"
            << "PRAGMA cache_Size=10000";

    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }

        QSqlQuery query(database);
        for(auto key: knownFormats_.keys())
        {
            query.clear();
            query.prepare(createNewTable(key, knownFormats_[key]));
            success = query.exec();


            if(!success)
                qDebug() << query.lastError();
        }

        for(auto pragma: pragmas)
        {
            query.clear();
            query.prepare(pragma);
            success = query.exec();
        }

        query.clear();
        database.close();
    }
    QSqlDatabase::removeDatabase("orders");
    return success;
}

QString OrderSQLite::createNewTable(const QString &tableName, const DataInfoMap &format)
{
    QString queryString; queryString.reserve(1000);
    queryString.append("CREATE TABLE "+tableName+"(");
    QStringList sqliteColumns = QStringList::fromVector(QVector<QString>(format.size()));

    //Use format keys to ensure that columns are in the correct sequence.
    for(auto key: format.keys())
        sqliteColumns[format[key].sqliteColumn_] = key + " " + format[key].sqliteType_;

    queryString.append(sqliteColumns.join(", "));
    queryString.append(")");

    qDebug() << "Query String" << queryString;

    return queryString;
}


QVector<QString> OrderSQLite::importCSVtoSQLite(const DataInfoMap &csvFormat,
                                                const QString &tableName,
                                                const bool hasHeaders,
                                                const QString &filePath,
                                                const int &chunkSize)
{
    QFile *csvFile = new QFile(filePath);
    QTextStream csvFileStream(csvFile);
    QVector<QString> lineVec(chunkSize);
    QVector<QString> csvStringVecs;
    QVector<DataInfo> sqliteTypes(csvFormat.size());

    for(auto key: csvFormat.keys())
    {
        sqliteTypes[csvFormat.value(key).getCSVColumn()] = csvFormat.value(key);
        qDebug() << csvFormat.value(key).getCSVColumn();
    }


    auto csvLineToValueStringFunc = std::bind(OrderSQLite::csvLineToValueString, std::placeholders::_1, sqliteTypes);

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << csvFile->errorString();
        return QVector<QString>();
    }

    //Skip headers
    if(hasHeaders && !csvFileStream.atEnd())
        csvFileStream.readLine();


    // Chunks
    int i = 0;
    while(!csvFileStream.atEnd())
    {
        if(i == chunkSize)
        {
            //qDebug() << "Bing";
            csvStringVecs = QtConcurrent::blockingMapped(lineVec, csvLineToValueStringFunc);
            saveToDB(tableName, csvFormat, csvStringVecs);
            csvStringVecs.clear();
            csvStringVecs.shrink_to_fit();
            i = 0;
        }
        lineVec.replace(i, csvFileStream.readLine());
        ++i;
    }

    // Computes remaining lineVec.
    qDebug() << i;
    csvStringVecs = QtConcurrent::blockingMapped(lineVec.mid(0, i), csvLineToValueStringFunc);
    lineVec.clear();
    lineVec.shrink_to_fit();
    saveToDB(tableName, csvFormat, csvStringVecs);
    csvStringVecs.clear();
    csvStringVecs.shrink_to_fit();

    // End Chunks

    lineVec.clear();
    lineVec.shrink_to_fit();

    csvFile->flush();
    csvFile->close();
    delete csvFile;

    return csvStringVecs;
}

bool OrderSQLite::saveToDB(const QString &tableName, const DataInfoMap &format, const QVector<QString> &linesIn)
{
    bool success;
    QString queryString; queryString.reserve(20000000);
    QVector<QString> csvColumns(format.size());
    QVector<QString> sqliteTypes(format.size());

    queryString.append("INSERT OR IGNORE INTO "+tableName+"(");

    for(auto key: format.keys())
    {
        csvColumns[format[key].csvColumn_] = key;
        sqliteTypes[format[key].csvColumn_] = format[key].sqliteType_;
    }

    queryString.append(QStringList::fromVector(csvColumns).join(", ") + ") VALUES ");

    for(int i = 0; i < linesIn.size(); ++i)
    {
        if(i == linesIn.size() - 1)
        {
            queryString.append(linesIn.at(i));
            continue;
        }
        queryString.append(linesIn.at(i) + ", ");
    }

    //Scope to make sure everything is clean regarding DB connections.
    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }
        database.transaction();
        QSqlQuery query(database);
        query.prepare(queryString);
        success = query.exec();

        if(!success)
        {
            qDebug() << query.lastError();
            //qDebug() << queryString;
        }
        database.commit();
        query.finish();
        query.clear();
        database.close();
    }
    QSqlDatabase::removeDatabase("orders");

    csvColumns.clear();
    csvColumns.squeeze();
    sqliteTypes.clear();
    sqliteTypes.squeeze();
    queryString.clear();
    queryString.squeeze();
    return success;
}

QVector<QString> OrderSQLite::parseCSVLine(const QString &csvLine)
{
    QRegularExpression csvParse("(?:,|\\n|^)(\"(?:(?:\"\")*[^\"]*)*\"|[^\",\\n]*|(?:\\n|$))");
    QRegularExpressionMatchIterator i = csvParse.globalMatch(csvLine.simplified());
    QRegularExpressionMatch match;
    QString word;
    word.reserve(100);
    qDebug() << sizeof(word);
    QVector<QString> matchVec;

    if(csvLine.at(0) == ',')
        matchVec.append(QString());

    while (i.hasNext()) {
        match = i.next();
        word = match.captured(1);
        word = word.remove("\"");
        matchVec.append(word.trimmed());
    }
    matchVec.shrink_to_fit();


    return matchVec;
}

QString OrderSQLite::csvLineToValueString(const QString &csvLine, const QVector<DataInfo> &sqliteTypes)
{
    QRegularExpression csvParse("(?:,|\\n|^)(\"(?:(?:\"\")*[^\"]*)*\"|[^\",\\n]*|(?:\\n|$))");
    QRegularExpressionMatchIterator i = csvParse.globalMatch(csvLine.simplified());
    QRegularExpressionMatch match;
    QVector<QString> matchVec(sqliteTypes.size());
    QString word; word.reserve(100);
    QString valueString; valueString.reserve(2000);
    int matchCounter = 0;

    if(csvLine.at(0) == ',')
    {
        matchVec[0] = "NULL";
        ++matchCounter;
    }

    while (i.hasNext()) {
        match = i.next();
        word = match.captured(1);
        word = word.remove("\"");
        word = word.trimmed();
        if(word.isEmpty())
            matchVec[matchCounter] = "NULL";
        else
        {
            //qDebug() << sqliteTypes.at(matchCounter).getSQLiteType();
            if(sqliteTypes.at(matchCounter).isDate())
            {
                word = QDate::fromString(word, sqliteTypes.at(matchCounter).getDateTimeFormat()).addYears(sqliteTypes.at(matchCounter).getModYear()).toString("yyyy-MM-dd");
            }

            if(sqliteTypes.at(matchCounter).isTime())
            {
                word = QTime::fromString(word, sqliteTypes.at(matchCounter).getDateTimeFormat()).addSecs(sqliteTypes.at(matchCounter).getModSec()).toString("HH:mm:ss");
            }

            if(sqliteTypes.at(matchCounter).isDateTime())
            {
                word = QDateTime::fromString(word, sqliteTypes.at(matchCounter).getDateTimeFormat()).addSecs(sqliteTypes.at(matchCounter).getModSec()).toString("yyyy-MM-dd HH:MM:ss");
            }

            if(sqliteTypes.at(matchCounter).getSQLiteType() == QLatin1String("TEXT") || sqliteTypes.at(matchCounter).getSQLiteType() == QLatin1String("BLOB"))
                matchVec[matchCounter] = "\"" + word + "\"";

            if(sqliteTypes.at(matchCounter).getSQLiteType().contains("INTEGER") || sqliteTypes.at(matchCounter).getSQLiteType() == QLatin1String("REAL"))
                matchVec[matchCounter] = word.remove(QRegularExpression("[a-zA-Z,]"));
        }
        ++matchCounter;
    }

    //process the matchVec...
    valueString.append("(");
    for(int i = 0; i < matchVec.size(); ++i)
    {
        if(i == matchVec.size() - 1)
        {
            valueString.append(matchVec[i] + ")");
            continue;
        }
        valueString.append(matchVec[i] + ", ");
    }

    //qDebug() << valueString;
    matchVec.clear();
    matchVec.squeeze();
    return valueString;
}

void OrderSQLite::generateRandyReport()
{

}
