#include "sqlitedbthread.h"

SQLiteDBThread::SQLiteDBThread(const QString &dbPath, QObject *parent) : QObject(parent)
{
    OrderSQLite *sqliteDB = new OrderSQLite(dbPath);

    qRegisterMetaType<QMap<QString,DataInfo>>();
    sqliteDB->moveToThread(&sqliteThread);
    connect(&sqliteThread, &QThread::finished, sqliteDB, &QObject::deleteLater);
    connect(this, &SQLiteDBThread::operate, sqliteDB, &OrderSQLite::importCSVtoSQLite);
    connect(sqliteDB, &OrderSQLite::importFinished, this, &SQLiteDBThread::handleResult);
    sqliteThread.start();
}

void SQLiteDBThread::handleResult(const bool &success)
{
    qDebug() << "result is " << success;
    //malloc_trim(0);
}
