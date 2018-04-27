#ifndef MYSQLEXPORTER_H
#define MYSQLEXPORTER_H

#include <QtCore>
#include <QObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

class mysqlExporter : public QObject
{
    Q_OBJECT
public:
    explicit mysqlExporter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MYSQLEXPORTER_H
