#include "sqlitedbthread.h"

SQLiteDBThread::SQLiteDBThread(const QString &dbPath, QObject *parent) : QObject(parent)
{
    dbPath_ = dbPath;
    qRegisterMetaType<QMap<QString,DataInfo>>();
}

void SQLiteDBThread::importFromThread(const DataInfoMap &csvFormat,
                                       const QString &tableName,
                                       const bool hasHeaders,
                                       const QString &filePath,
                                       const int &chunkSize)
{
    while(sqliteThread.isRunning())
    {
        qApp->processEvents();
    }

    OrderSQLite *sqliteDB = new OrderSQLite(dbPath_);
    sqliteDB->moveToThread(&sqliteThread);
    connect(&sqliteThread, &QThread::finished, sqliteDB, &QObject::deleteLater);
    connect(this, &SQLiteDBThread::operate, sqliteDB, &OrderSQLite::importCSVtoSQLite);
    connect(sqliteDB, &OrderSQLite::importFinished, this, &SQLiteDBThread::handleResult);

    sqliteThread.start();
    emit operate(csvFormat, tableName, hasHeaders, filePath, chunkSize);
}

void SQLiteDBThread::handleResult(const bool &success)
{
    qDebug() << "result is " << success;
    sqliteThread.exit();
    //malloc_trim(0);
}
