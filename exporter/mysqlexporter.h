#ifndef MYSQLEXPORTER_H
#define MYSQLEXPORTER_H

#include <QObject>

class mysqlExporter : public QObject
{
    Q_OBJECT
public:
    explicit mysqlExporter(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MYSQLEXPORTER_H