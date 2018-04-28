#include "mysqlexporter.h"
#include <unistd.h>
mysqlExporter::mysqlExporter(QObject *parent) : QObject(parent)
{
    //load the mysql settings from a db.

}

bool mysqlExporter::exportQueryString(const QString &queryString)
{
    int lastError = 0;
    bool success;
    {
        QSqlDatabase sslDB = QSqlDatabase::addDatabase("QMYSQL", "test");
        QString workingDir = QCoreApplication::applicationDirPath() + "/";
        QString caStr = ("SSL_CA=" + workingDir + "ca.pem;");
        QString clientKeyStr = ("SSL_KEY=" + workingDir + "client-key.pem;");
        QString clientCertStr = ("SSL_CERT=" + workingDir + "client-cert.pem;");
        sslDB.setConnectOptions(caStr + clientKeyStr + clientCertStr);
        if(sslDB.open())
        {
            qDebug() << "dbOpen";
            QSqlQuery query(sslDB);
            //query.prepare(queryString);
            //qDebug() << queryString;
            //QString queryString = "REPLACE INTO custVerbose (customerNumber, customerName) VALUES (100, \"windows is a hoooooooooooor\")";
            //query.prepare(queryString);
            success = query.exec(queryString);
            //qDebug() << queryString;

            if(!success)
            {
                qDebug() << query.lastError();
                lastError = query.lastError().number();
            }
//            QString queryResult;

//            while (query.next())
//                for(int j = 0; j < query.record().count(); ++j)
//                    queryResult.append(query.value(j).toString());

//            qDebug() << queryResult;

            sslDB.commit();
            query.finish();
            query.clear();
        }
        else
        {
            qDebug() << "reee";
        }
        sslDB.close();
        qDebug() << "done";
    }
    QSqlDatabase::removeDatabase("test");


    if(lastError == 2006)
    {
        qDebug() << "ree?";
        usleep(60000000);
        qDebug() << "ree2?";
        exportQueryString(queryString);
    }

    return success;
}



bool mysqlExporter::exportToMySQL(int chunkSize)
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

        /*
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
        */


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

        /*
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
        */


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
