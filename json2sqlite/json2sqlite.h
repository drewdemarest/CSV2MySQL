#ifndef JSON2SQLITE_H
#define JSON2SQLITE_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>
#include <QSqlError>

class Json2Sqlite
{
public:
    Json2Sqlite();
    bool saveSettings(QString dbPath, QJsonObject jsonSettings);
    bool saveArray(QString dbPath, QString table, QString primaryKey, QJsonArray jsonArray);
    QJsonObject loadSettings(QString dbPath, QJsonObject jsonSettings);
    QJsonArray loadArray(QString dbPath, QString table);
    bool doesDatabaseExist(QString dbPath);
    int numberOfTypesInArray(QJsonArray jsonArray);

private:
    bool makeInitalSettingsDatabase(QString dbPath);
    bool makeInitialArrayDatabase(QString dbPath, QString table, QString primaryKey, QJsonArray jsonArray);
};

#endif // JSON2SQLITE_H
