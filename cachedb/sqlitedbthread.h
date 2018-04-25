#ifndef SQLITEDBTHREAD_H
#define SQLITEDBTHREAD_H

#include "cachedb/ordersqlite.h"
#include <QObject>
#include <QThread>


class SQLiteDBThread : public QObject
{
    Q_OBJECT
public:
    explicit SQLiteDBThread(const QString &dbPath, QObject *parent = nullptr);

    ~SQLiteDBThread()
    {
        sqliteThread.quit();
        sqliteThread.wait();
    }

    void importFromThread(const DataInfoMap &csvFormat,
                          const QString &tableName,
                          const bool hasHeaders,
                          const QString &filePath,
                          const int &chunkSize);

    QThread sqliteThread;
    OrderSQLite *sqliteDB;

signals:
    void operate(const QMap<QString,DataInfo> &csvFormat,
                 const QString &tableName,
                 const bool hasHeaders,
                 const QString &filePath,
                 const int &chunkSize);

public slots:
    void handleResult(const bool &success);
    void dinger();

private:
    QString dbPath_;
};

#endif // SQLITEDBTHREAD_H
