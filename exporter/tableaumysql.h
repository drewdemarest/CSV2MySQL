#ifndef TABLEAUMYSQL_H
#define TABLEAUMYSQL_H

#include <QObject>

class TableauMySQL : public QObject
{
    Q_OBJECT
public:
    explicit TableauMySQL(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TABLEAUMYSQL_H