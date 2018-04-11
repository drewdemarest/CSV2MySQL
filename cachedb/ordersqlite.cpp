#include "ordersqlite.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

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

    QVector<QString> sqliteTypes(csvFormat.size());
    for(auto key: csvFormat.keys())
        sqliteTypes[csvFormat[key].csvColumn_] = csvFormat[key].sqliteType_;

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


//    int rc;
//    sqlite3 *db;
//    char *zErrMsg = 0;
//    char *str;

//    QByteArray temp = queryString.toLatin1();
//    str = temp.data();
//    /* Open database */
//    rc = sqlite3_open("potato.db", &db);

//    if( rc ) {
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//       return(0);
//    } else {
//       fprintf(stdout, "Opened database successfully\n");
//    }

//    /* Create SQL statement */
//    //sql = queryString.toStdString().c_str();

//    /* Execute SQL statement */
//    rc = sqlite3_exec(db, str, callback, 0, &zErrMsg);

//    if( rc != SQLITE_OK ){
//    fprintf(stderr, "SQL error: %s\n", zErrMsg);
//       sqlite3_free(zErrMsg);
//    } else {
//       fprintf(stdout, "Table created successfully\n");
//    }
//    sqlite3_close(db);

//------------------------------------
//THIS IS THE DEMON
    bool derp = true;
    if(derp)
    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        qDebug() << database.isOpen();
        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }
        database.transaction();
        QSqlQuery query(database);

        query.prepare("PRAGMA journal_mode = OFF");
        success = query.exec();
        if(!success)
        {
            qDebug() << query.lastError();
            //qDebug() << queryString;
        }

        query.prepare("PRAGMA temp_store = MEMORY");
        success = query.exec();
        if(!success)
        {
            qDebug() << query.lastError();
            //qDebug() << queryString;
        }
       query.clear();



        query.prepare(queryString);
        success = query.exec();

        if(!success)
        {
            //qDebug() << query.lastError();
            //qDebug() << queryString;
        }
        database.commit();
        query.finish();
        query.clear();
        database.close();
    }
//------------------------------------
//Still has a memory leak?!
    QSqlDatabase::removeDatabase("orders");

    csvColumns.clear();
    csvColumns.squeeze();
    sqliteTypes.clear();
    sqliteTypes.squeeze();
    queryString.clear();
    queryString.squeeze();
    return success;
}

QString OrderSQLite::insertOrIgnoreInto(const QString &tableName, const DataInfoMap &format, const QVector<QVector<QString>> &linesIn)
{
    //Initialize a list to size of format.
    QVector<QString> valueTuples;
    qDebug() << linesIn.size();
    //QStringList csvColumns = QStringList::fromVector(QVector<QString>(format.size()));
    QVector<QString> csvColumns(format.size());
    //QStringList sqliteTypes = QStringList::fromVector(QVector<QString>(format.size()));
    QVector<QString> sqliteTypes(format.size());
    QVector<QString> tempVec;

    QString queryString;
    queryString.reserve(20000000);

    queryString.append("INSERT OR IGNORE INTO "+tableName+"(");

    for(auto key: format.keys())
    {
        csvColumns[format[key].csvColumn_] = key;
        sqliteTypes[format[key].csvColumn_] = format[key].sqliteType_;
    }

    queryString.append(QStringList::fromVector(csvColumns).join(", ") + ") VALUES ");

    auto createValueTupleFunc = std::bind(OrderSQLite::createValueTuple, std::placeholders::_1, sqliteTypes);

    valueTuples = QtConcurrent::blockingMapped(linesIn, createValueTupleFunc);
    //valueTuples = QStringList::fromVector(tempVec);

    for(int i = 0; i < valueTuples.size(); ++i)
    {
        if(i == valueTuples.size() - 1)
        {
            queryString.append(valueTuples[i]);
            continue;
        }
        queryString.append(valueTuples[i] + ", ");
    }

   //queryString.append(valueTuples.join(", "));

    qDebug() << queryString.size();
    csvColumns.clear();
    csvColumns.squeeze();
    sqliteTypes.clear();
    sqliteTypes.squeeze();
    valueTuples.clear();
    valueTuples.squeeze();
    return queryString;
}

QString OrderSQLite::createValueTuple(QVector<QString> csvLine, const QVector<QString> &sqliteTypes)
{
//    if(sqliteTypes.size() != csvLine.size())
//    {
//        qDebug() << "Parse error, the CSV row is not in compliance with the SQLite table. Dropping this entry.";
//        qDebug() << sqliteTypes << sqliteTypes.size();
//        qDebug() << csvLine << csvLine.size();
//        csvLine.clear();
//        for(int i = 0; i < sqliteTypes.size(); ++i)
//            csvLine << "NULL";
//        qDebug() << csvLine.join(", ");
//        return csvLine.join(", ");
//    }


    for(int i = 0; i < csvLine.size(); ++i)
    {
        if(csvLine[i].isEmpty())
        {
            csvLine[i] = "NULL";
            continue;
        }
        else
        {
            if(sqliteTypes.at(i).contains("TEXT") || sqliteTypes.at(i).contains("BLOB"))
                csvLine[i] = "\"" + csvLine[i] + "\"";
            if(sqliteTypes.at(i).contains("INTEGER") || sqliteTypes.at(i).contains("REAL"))
            {
                csvLine[i] = csvLine[i].remove(QRegularExpression("[a-zA-Z,]"));
            }
        }

    }
    QString rtnString = "(";
    for(int i = 0; i < csvLine.size(); ++i)
    {
        if(i == csvLine.size() - 1)
        {
            rtnString.append(csvLine[i] + ")");
            continue;
        }
        rtnString.append(csvLine[i] + ", ");
    }

     //= QString("(" + csvLine.join(", ") + ")");
    csvLine.clear();
    csvLine.shrink_to_fit();
    return rtnString;
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

QString OrderSQLite::csvLineToValueString(const QString &csvLine, const QVector<QString> &sqliteTypes)
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
            if(sqliteTypes.at(matchCounter) == QLatin1String("TEXT") || sqliteTypes.at(matchCounter) == QLatin1String("BLOB"))
                matchVec[matchCounter] = "\"" + word + "\"";

            if(sqliteTypes.at(matchCounter).contains("INTEGER") || sqliteTypes.at(matchCounter) == QLatin1String("REAL"))
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

    matchVec.clear();
    matchVec.squeeze();
    return valueString;
}
