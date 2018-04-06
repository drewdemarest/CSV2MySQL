#include "ordersqlite.h"

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

QList<QVariantMap> OrderSQLite::getOrdersFromDB(const int minInvoiceNum, const int maxInvoiceNum) const
{
    return QList<QVariantMap>();
}

QList<QVariantMap> OrderSQLite::getOrdersFromDB(const QDate &minOrderDate, const QDate &maxOrderDate) const
{
    return QList<QVariantMap>();
}

TableInfoMap OrderSQLite::getKnownFormats() const
{
    return knownFormats_;
}

bool OrderSQLite::addFormat(const QString &tableName, const DataInfoMap &dim)
{
    knownFormats_[tableName] = dim;
}

bool OrderSQLite::populateOrders() const
{
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
    QSqlDatabase database;
    database = database.addDatabase("QSQLITE", "orders");
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


    query.clear();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase("orders");
    return success;
}

QString OrderSQLite::createNewTable(const QString &tableName, const DataInfoMap &format)
{
    QString queryString = "CREATE TABLE "+tableName+"(";
    QStringList sqliteColumns = QStringList::fromVector(QVector<QString>(format.size()));

    //Use format keys to ensure that columns are in the correct sequence.
    for(auto key: format.keys())
        sqliteColumns[format[key].sqliteColumn_] = key + " " + format[key].sqliteType_;

    queryString.append(sqliteColumns.join(", "));
    queryString.append(")");

    qDebug() << "Query String" << queryString;

    return queryString;
}


QList<QStringList> OrderSQLite::importCSVtoSQLite(const DataInfoMap &csvFormat,
                                                  const QString &tableName,
                                                  const bool hasHeaders,
                                                  const QString &filePath,
                                                  const int &chunkSize)
{
    QFile csvFile(filePath);
    QList<QByteArray> lineList;
    QList<QStringList> csvStringLists;

    if(!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << csvFile.errorString();
        return QList<QStringList>();
    }

//Skip headers
    if(hasHeaders && !csvFile.atEnd())
        csvFile.readLine();

// Chunks
    int i = 0;
    while(!csvFile.atEnd())
    {
        lineList.append(csvFile.readLine());
        if(i % chunkSize == 0)
        {
            //qDebug() << "Bing";
            csvStringLists = QtConcurrent::blockingMapped(lineList, OrderSQLite::parseCSVLine);
            lineList.clear();
            saveToDB(tableName, csvFormat, csvStringLists);
            csvStringLists.clear();
        }
        ++i;

    }
// Computes remaining lineList.
    csvStringLists = QtConcurrent::blockingMapped(lineList, OrderSQLite::parseCSVLine);
    lineList.clear();
    saveToDB(tableName, csvFormat, csvStringLists);
    csvStringLists.clear();

// End Chunks

    lineList.clear();
    return csvStringLists;
}

bool OrderSQLite::saveToDB(const QString &tableName, const DataInfoMap &format, const QList<QStringList> &linesIn)
{
    bool success;
    QSqlDatabase database;
    database = database.addDatabase("QSQLITE", "orders");
    database.setDatabaseName(dbPath_);

    if(!database.open())
    {
        qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
        return false;
    }

    QSqlQuery query(database);

    query.prepare(insertOrIgnoreInto(tableName, format, linesIn));
    success = query.exec();

    if(!success)
    {
        qDebug() << query.lastError();
        qDebug() << insertOrIgnoreInto(tableName, format, linesIn);
    }

    query.clear();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase("orders");
    return success;
}

QString OrderSQLite::insertOrIgnoreInto(const QString &tableName, const DataInfoMap &format, const QList<QStringList> &linesIn)
{
    //Initialize a list to size of format.
    QStringList valueTuples;
    QStringList csvColumns = QStringList::fromVector(QVector<QString>(format.size()));
    QStringList sqliteTypes = QStringList::fromVector(QVector<QString>(format.size()));

    QString queryString = "INSERT OR IGNORE INTO "+tableName+"(";

    for(auto key: format.keys())
    {
        csvColumns[format[key].csvColumn_] = key;
        sqliteTypes[format[key].csvColumn_] = format[key].sqliteType_;
    }

    queryString.append(csvColumns.join(", ") + ") VALUES ");

    auto createValueTupleFunc = std::bind(OrderSQLite::createValueTuple, std::placeholders::_1, sqliteTypes);
    valueTuples = QtConcurrent::blockingMapped(linesIn, createValueTupleFunc);
    queryString.append(valueTuples.join(", "));
    return queryString;
}

QString OrderSQLite::createValueTuple(QStringList csvRow, const QStringList &sqliteTypes)
{
    if(sqliteTypes.size() != csvRow.size())
    {
        qDebug() << "Parse error, the CSV row is not in compliance with the SQLite table. Dropping this entry.";
        qDebug() << sqliteTypes << sqliteTypes.size();
        qDebug() << csvRow << csvRow.size();
        csvRow.clear();
        for(int i = 0; i < sqliteTypes.size(); ++i)
            csvRow << "NULL";
        qDebug() << csvRow.join(", ");
        return csvRow.join(", ");
    }


    for(int i = 0; i < csvRow.size(); ++i)
    {
        if(csvRow[i].isEmpty())
        {
            csvRow[i] = "NULL";
            continue;
        }
        else
        {
            if(sqliteTypes.at(i) == "TEXT" || sqliteTypes.at(i) == "BLOB")
                csvRow[i] = "\"" + csvRow[i] + "\"";
            if(sqliteTypes.at(i).contains("INTEGER") || sqliteTypes.at(i).contains("REAL"))
            {
                csvRow[i] = csvRow[i].remove(QRegularExpression("[a-zA-Z,]"));
            }
        }

    }

    return QString("(" + csvRow.join(", ") + ")");
}

QStringList OrderSQLite::parseCSVLine(const QByteArray &csvLine)
{
    QByteArray csvLineSimpl = csvLine.simplified();
    QRegularExpression csvParse("(?:,|\\n|^)(\"(?:(?:\"\")*[^\"]*)*\"|[^\",\\n]*|(?:\\n|$))");
    QRegularExpressionMatchIterator i = csvParse.globalMatch(csvLineSimpl);
    QStringList matchList;

    if(csvLine.at(0) == ',')
        matchList.append(QString());

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        word = word.remove("\"");
        matchList << word.trimmed();
    }
    return matchList;
}
