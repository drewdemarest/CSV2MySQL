#include "sqlitedbthread.h"

SQLiteDBThread::SQLiteDBThread(const QString &dbPath, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QMap<QString,DataInfo>>();

    dbPath_ = dbPath;
    sqliteDB = new OrderSQLite(dbPath_);
    sqliteDB->moveToThread(&sqliteThread);
    connect(this, &SQLiteDBThread::operate, sqliteDB, &OrderSQLite::importCSVtoSQLite);
    connect(sqliteDB, &OrderSQLite::importFinished, this, &SQLiteDBThread::handleResult);
    connect(&sqliteThread, &QThread::finished, this, &SQLiteDBThread::dinger);
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
    qDebug() << "ding";

    sqliteThread.start();
    emit operate(csvFormat, tableName, hasHeaders, filePath, chunkSize);
}

void SQLiteDBThread::handleResult(const bool &success)
{
    qDebug() << "result is " << success;
    sqliteThread.exit();
    sqliteThread.wait();
    //malloc_trim(0);
}

void SQLiteDBThread::dinger()
{
    qDebug() << "ding";
}
