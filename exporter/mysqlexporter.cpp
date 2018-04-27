#include "mysqlexporter.h"

mysqlExporter::mysqlExporter(QObject *parent) : QObject(parent)
{
    //load the mysql settings from a db.
    {
        QSqlDatabase sslDB = QSqlDatabase::addDatabase("QMYSQL", "test");
        QString workingDir = QCoreApplication::applicationDirPath() + "/";

        sslDB.setConnectOptions(caStr + clientKeyStr + clientCertStr);
        if(sslDB.open())
        {
            qDebug() << "dbOpen";
            QSqlQuery query(sslDB);
            //query.prepare(queryString);
            //qDebug() << queryString;
            QString queryString = "INSERT INTO custVerbose (customerNumber) VALUES (100)";
            qDebug() << queryString;
            query.prepare(queryString);
            bool success = query.exec();

            if(!success)
            {
                qDebug() << query.lastError();
                //qDebug() << queryString;
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
        sslDB.close();
        qDebug() << "done";
    }
}
