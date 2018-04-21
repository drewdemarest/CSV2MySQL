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

    QThread sqliteThread;

signals:
    void operate(const QMap<QString,DataInfo> &csvFormat,
                 const QString &tableName,
                 const bool hasHeaders,
                 const QString &filePath,
                 const int &chunkSize);

public slots:
    void handleResult(const bool &success);
};

#endif // SQLITEDBTHREAD_H
