#include "ordersqlite.h"

OrderSQLite::OrderSQLite(const QString &dbPath, QObject *parent) : QObject(parent)
{   qDebug() << dbPath;
    dbPath_ = dbPath;

    if(!doesDatabaseExist())
        makeOrderDB();
//    else
//    {
//        QFile file(dbPath);
//        file.remove();
//        makeOrderDB();
//    }
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
    //qDebug() << queryString;
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
            //qDebug() << createNewTable(key, knownFormats_[key]);
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
    QString primaryKeyString; primaryKeyString.reserve(1000);
    primaryKeyString.append("PRIMARY KEY(");
    queryString.append("CREATE TABLE "+tableName+"(");
    QStringList sqliteColumns = QStringList::fromVector(QVector<QString>(format.size()));
    QStringList primaryKeys;

    //Use format keys to ensure that columns are in the correct sequence.
    for(auto key: format.keys())
    {
        if(format[key].isPrimaryKey())
            primaryKeys.append(key);

        switch (format[key].sqliteType_)
        {
        case SQLiteType::INTEGER:
            sqliteColumns[format[key].sqliteColumn_] = key + " INTEGER";
            break;

        case SQLiteType::REAL:
            sqliteColumns[format[key].sqliteColumn_] = key + " REAL";
            break;

        case SQLiteType::TEXT:
            sqliteColumns[format[key].sqliteColumn_] = key + " TEXT";
            break;

        case SQLiteType::TEXT_DATE:
            sqliteColumns[format[key].sqliteColumn_] = key + " TEXT";
            break;

        case SQLiteType::TEXT_TIME:
            sqliteColumns[format[key].sqliteColumn_] = key + " TEXT";
            break;

        case SQLiteType::TEXT_DATETIME:
            sqliteColumns[format[key].sqliteColumn_] = key + " TEXT";
            break;

        case SQLiteType::BLOB:
            sqliteColumns[format[key].sqliteColumn_] = key + " BLOB";
            break;

        case SQLiteType::NULL_SQL:
            sqliteColumns[format[key].sqliteColumn_] = key + " NULL";
            break;
        }
    }

    if(!primaryKeys.empty())
    {
        primaryKeyString.append(primaryKeys.join(", "));
        primaryKeyString.append(")");
        sqliteColumns.append(primaryKeyString);
    }

    queryString.append(sqliteColumns.join(", "));
    queryString.append(")");

    qDebug() << "Query String" << queryString;
    return queryString;
}


bool OrderSQLite::importCSVtoSQLite(const DataInfoMap &csvFormat,
                                                const QString &tableName,
                                                const bool hasHeaders,
                                                const QString &filePath,
                                                const int &chunkSize)
{

    qDebug() << tableName << hasHeaders << filePath << chunkSize;

    QFile *csvFile = new QFile(filePath);
    QTextStream csvFileStream(csvFile);
    QVector<QString> lineVec(chunkSize);
    QVector<QString> csvStringVecs;
    QVector<DataInfo> sqliteTypes(csvFormat.size());

    for(auto key: csvFormat.keys())
    {
        sqliteTypes[csvFormat.value(key).getCSVColumn()] = csvFormat.value(key);
        //qDebug() << csvFormat.value(key).getCSVColumn();
    }

    auto csvLineToValueStringFunc = std::bind(OrderSQLite::csvLineToValueString, std::placeholders::_1, sqliteTypes);

    if(!csvFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << csvFile->errorString();
        qDebug() << "There was an error";
        emit importFinished(false);
        return false;
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

    emit importFinished(true);
    QDir outDir(QCoreApplication::applicationDirPath() + "/OUT/");
    qDebug() << filePath;
    qDebug() << "I AM HERE " + outDir.path() + "/invoice" + QDateTime::currentDateTime().toString(Qt::ISODate) + ".csv";
    //outDir.rename(filePath, outDir.path() + "/invoice" + QDateTime::currentDateTime().toString(Qt::ISODate) + ".csv");

    csvFile->rename(outDir.path() + "/invoice" + QDateTime::currentDateTime().toString("yyyy-MM-dd-T-HH-mm-ss-zzz") + ".csv");
    csvFile->flush();
    csvFile->close();
    delete csvFile;


    return true;
}

bool OrderSQLite::saveToDB(const QString &tableName, const DataInfoMap &format, const QVector<QString> &linesIn)
{
    bool success;
    QString queryString; queryString.reserve(20000000);
    QVector<QString> csvColumns(format.size());
    QVector<QString> sqliteTypes(format.size());

    queryString.append("INSERT OR REPLACE INTO "+tableName+"(");

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
        //query.prepare(queryString);
        //qDebug() << queryString;
        success = query.exec(queryString);

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
    QString word; //word.reserve(100);
    QString valueString; //valueString.reserve(2000);
    int matchCounter = 0;

    if(csvLine.at(0) == ',')
    {
        matchVec[0] = "NULL";
        ++matchCounter;
    }

    while (i.hasNext())
    {
        match = i.next();
        word = match.captured(1);
        word = word.remove("\"");
        word = word.trimmed();

        //check if null word
        if(word.isEmpty())
        {
            matchVec[matchCounter] = "NULL";
            ++matchCounter;
            continue;
        }

        switch(sqliteTypes.at(matchCounter).getSQLiteType())
        {
        case SQLiteType::INTEGER:
            OrderSQLite::parseInteger(word);
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::REAL:
            OrderSQLite::parseReal(word);
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::TEXT:
            OrderSQLite::parseText(word);
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::TEXT_DATE:
            OrderSQLite::parseDate(word, sqliteTypes.at(matchCounter));
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::TEXT_TIME:
            OrderSQLite::parseTime(word, sqliteTypes.at(matchCounter));
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::TEXT_DATETIME:
            OrderSQLite::parseDateTime(word, sqliteTypes.at(matchCounter));
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::BLOB:
            OrderSQLite::parseBlob(word);
            matchVec[matchCounter] = word;
            break;

        case SQLiteType::NULL_SQL:
            matchVec[matchCounter] = "NULL";
            break;
        }
        ++matchCounter;
    }

    //process the matchVec...
    valueString.append("(");
    for(int i = 0; i < matchVec.size(); ++i)
    {
        if(i == matchVec.size() - 1)
        {
            valueString.append(matchVec[i].append(")"));
            continue;
        }
        valueString.append(matchVec[i].append(", "));
    }

    //qDebug() << valueString;
    matchVec.clear();
    matchVec.squeeze();
    //qDebug() << valueString;
    return valueString;
}

void OrderSQLite::ifEmptyNull(QString &word)
{
    if(word.isEmpty())
        word = "NULL";
}

void OrderSQLite::parseText(QString &text)
{
    text.append("\"");
    text.prepend("\"");
}

void OrderSQLite::parseBlob(QString &blob)
{
    blob.append("\"");
    blob.prepend("\"");
}

void OrderSQLite::parseInteger(QString &integer)
{
    integer = integer.remove(QRegularExpression("[a-zA-Z,]"));
    OrderSQLite::ifEmptyNull(integer);
}

void OrderSQLite::parseReal(QString &real)
{
    if(real.startsWith("."))
        real.prepend("0");

    real = real.remove(QRegularExpression("[a-zA-Z,]"));
    OrderSQLite::ifEmptyNull(real);
}

void OrderSQLite::parseDate(QString &date, const DataInfo &format)
{
    date = QDate::fromString(date, format.getDateTimeFormat()).addYears(format.getModYear()).toString("yyyy-MM-dd");

    if(!date.isEmpty())
    {
        date.append("\"");
        date.prepend("\"");
    }
    OrderSQLite::ifEmptyNull(date);
}

void OrderSQLite::parseTime(QString &time, const DataInfo &format)
{
        if(time == "0")
            time = "NULL";

        if(time == "2400")
            time = "0000";

        while(time.size() < 4)
            time.prepend("0");

        time = QTime::fromString(time, format.getDateTimeFormat()).addSecs(format.getModSec()).toString("HH:mm:ss");

        if(!time.isEmpty())
        {
            time.append("\"");
            time.prepend("\"");
        }

        OrderSQLite::ifEmptyNull(time);
}

void OrderSQLite::parseDateTime(QString &dateTime, const DataInfo &format)
{
    dateTime = QDateTime::fromString(dateTime, format.getDateTimeFormat()).addSecs(format.getModSec()).toString("yyyy-MM-dd HH:MM:ss");

    if(!dateTime.isEmpty())
    {
        dateTime.append("\"");
        dateTime.prepend("\"");
    }
    OrderSQLite::ifEmptyNull(dateTime);
}
