#include "mysqlexporter.h"
#include <unistd.h>
mysqlExporter::mysqlExporter(QObject *parent) : QObject(parent)
{
    //load the mysql settings from a db.
    mySQLSettings_ = settings_.loadSettings(settingsDBPAth_, mySQLSettings_);
    DBTransferFormat dbxfer;
    qDebug() << dbxfer.makeSelectFromDB1();
    //TEST CODE

}

bool mysqlExporter::exportQueryString(const QString &queryString)
{
    int lastError = 0;
    bool success;
    {
        QSqlDatabase sslDB = QSqlDatabase::addDatabase("QMYSQL", "test");

        QString caStr = mySQLSettings_["caStr"].toString();
        QString clientKeyStr = mySQLSettings_["clientKeyStr"].toString();
        QString clientCertStr = mySQLSettings_["clientCertStr"].toString();
        sslDB.setHostName(mySQLSettings_["hostName"].toString());
        sslDB.setDatabaseName(mySQLSettings_["databaseName"].toString());
        sslDB.setUserName(mySQLSettings_["userName"].toString());
        sslDB.setPassword(mySQLSettings_["password"].toString());
        sslDB.setConnectOptions(caStr + clientKeyStr + clientCertStr);

        if(sslDB.open())
        {
            qDebug() << "dbOpen";
            QSqlQuery query(sslDB);
            success = query.exec(queryString);

            if(!success)
            {
                qDebug() << query.lastError();
                lastError = query.lastError().number();
            }

            sslDB.commit();
            query.finish();
            query.clear();
        }
        else
        {
            qDebug() << sslDB.lastError();
        }
        sslDB.close();
        qDebug() << "Completed MySQL";
    }
    QSqlDatabase::removeDatabase("test");


    if(lastError == 2006 || lastError == 2013)
    {
        qDebug() << "Failed due to " << lastError << " waiting 9secs.";
        usleep(9000000);
        qDebug() << "Done waitng";
        exportQueryString(queryString);
    }

    return success;
}

bool mysqlExporter::exportToMySQL(int chunkSize, QVector<QString>tables)
{

    QString queryExportString;
    QStringList valueTuples;
    QStringList queryResult;
    QStringList keys = formatCustomerChainGroupCSV_.keys();
    QStringList values = formatCustomerChainGroupCSV_.values();
    int rowCount = 0;

    {    //Scope to make sure everything is clean regarding DB connections.
        QString dbPath_ = QCoreApplication::applicationDirPath() + "/potato.db";
        bool success;
        QSqlDatabase database;
        database = database.addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }

        QString queryString = "SELECT " + keys.join(" , ") + " FROM CustomerChainGroupCSV"
                                                             " INNER JOIN CustRoutesTimeWindowCSV"
                                                             " ON CustomerChainGroupCSV.customerNumber = CustRoutesTimeWindowCSV.customerNumber";
        qDebug() << queryString;

        QSqlQuery query(database);
        //qDebug() << queryString;
        query.prepare(queryString);
        success = query.exec();
        if(!success)
            qDebug() << query.lastError();

        while(query.next())
        {
            if(rowCount == chunkSize)
            {
                queryExportString.clear();
                queryExportString.shrink_to_fit();
                queryExportString = "REPLACE INTO custVerbose (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
                qDebug() << queryExportString;
                exportQueryString(queryExportString);
                rowCount = 0;
            }

            queryResult.clear();
            valueTuples.clear();
            //qDebug() << query.record().count();
            for(int j = 0; j < query.record().count(); ++j)
            {
                switch(query.value(j).type()) {

                case QVariant::Type::LongLong:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::Double:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::String:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append("\"" + query.value(j).toString().toLatin1() + "\"");
                    break;

                default:
                    qDebug() << "unsupported data type from sqlite database " << query.value(j);
                    break;
                }
            }
            valueTuples.append("(" + queryResult.join(", ") + ")");
            ++rowCount;
        }

        queryExportString.clear();
        queryExportString.shrink_to_fit();
        queryExportString = "REPLACE INTO custVerbose (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
        exportQueryString(queryExportString);
        valueTuples.clear();
        query.clear();
        database.close();
        return success;
    }
}


bool mysqlExporter::exportInvoiceToMySQL(int chunkSize)
{

    QString queryExportString;
    QStringList valueTuples;
    QStringList queryResult;
    QStringList keys = formatInvoice_.keys();
    QStringList values = formatInvoice_.values();
    int rowCount = 0;

    {    //Scope to make sure everything is clean regarding DB connections.
        QString dbPath_ = QCoreApplication::applicationDirPath() + "/potato.db";
        bool success;
        QSqlDatabase database;
        database = database.addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }

        QString queryString = "SELECT " + keys.join(" , ") + " FROM OrderTrackingCSV"
                                                             " INNER JOIN RouteProfitabilityCSV"
                                                             " ON OrderTrackingCSV.invoiceNumber = RouteProfitabilityCSV.invoiceNumber";
        qDebug() << queryString;

        QSqlQuery query(database);
        //qDebug() << queryString;
        query.prepare(queryString);
        success = query.exec();
        if(!success)
            qDebug() << query.lastError();

        while(query.next())
        {
            if(rowCount == chunkSize)
            {
                queryExportString.clear();
                queryExportString.shrink_to_fit();
                queryExportString = "REPLACE INTO invoice (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
                valueTuples.clear();

                //qDebug() << queryExportString;
                exportQueryString(queryExportString);
                rowCount = 0;
            }

            queryResult.clear();
            //qDebug() << query.record().count();
            for(int j = 0; j < query.record().count(); ++j)
            {
                //qDebug() << query.value(j).type();
                switch(query.value(j).type()) {

                case QVariant::Type::LongLong:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::Double:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::String:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append("\"" + query.value(j).toString().toLatin1() + "\"");
                    break;

                default:
                    break;
                }


            }
            valueTuples.append("(" + queryResult.join(", ") + ")");
            ++rowCount;
        }


        queryExportString.clear();
        queryExportString.shrink_to_fit();
        queryExportString = "REPLACE INTO invoice (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
        exportQueryString(queryExportString);
        valueTuples.clear();


        query.clear();
        database.close();
        return success;
    }
}

bool mysqlExporter::exportCustomInvoiceToMySQL(int chunkSize)
{

    QString queryExportString;
    QStringList valueTuples;
    QStringList queryResult;
    QStringList keys = formatInvoiceCustom_.keys();
    QStringList values = formatInvoiceCustom_.values();
    int rowCount = 0;

    {    //Scope to make sure everything is clean regarding DB connections.
        QString dbPath_ = QCoreApplication::applicationDirPath() + "/potato.db";
        bool success;
        QSqlDatabase database;
        database = database.addDatabase("QSQLITE", "orders");
        database.setDatabaseName(dbPath_);

        if(!database.open())
        {
            qDebug() << "db failed to load in OrderSQLite::makeInitalDatabase";
            return false;
        }

        QString queryString = "SELECT " + keys.join(" , ") + " FROM InvoiceCustomCSV";
        qDebug() << queryString;

        QSqlQuery query(database);
        //qDebug() << queryString;
        query.prepare(queryString);
        success = query.exec();
        if(!success)
            qDebug() << query.lastError();

        while(query.next())
        {
            if(rowCount == chunkSize)
            {
                queryExportString.clear();
                queryExportString.shrink_to_fit();
                queryExportString = "REPLACE INTO invoice (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
                valueTuples.clear();

                //qDebug() << queryExportString;
                exportQueryString(queryExportString);
                rowCount = 0;
            }

            queryResult.clear();
            //qDebug() << query.record().count();
            for(int j = 0; j < query.record().count(); ++j)
            {
                //qDebug() << query.value(j).type();
                switch(query.value(j).type()) {

                case QVariant::Type::LongLong:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::Double:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append(query.value(j).toString());
                    break;

                case QVariant::Type::String:
                    if(query.value(j).isNull())
                        queryResult.append("NULL");
                    else
                        queryResult.append("\"" + query.value(j).toString().toLatin1() + "\"");
                    break;

                default:
                    break;
                }


            }
            valueTuples.append("(" + queryResult.join(", ") + ")");
            ++rowCount;
        }


        queryExportString.clear();
        queryExportString.shrink_to_fit();
        queryExportString = "REPLACE INTO invoice (" + values.join(", ") + ") VALUES " + valueTuples.join(", ");
        exportQueryString(queryExportString);
        valueTuples.clear();


        query.clear();
        database.close();
        return success;
    }
}
