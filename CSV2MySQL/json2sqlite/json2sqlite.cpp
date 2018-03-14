#include "json2sqlite.h"

Json2Sqlite::Json2Sqlite()
{
    qDebug() << "json2sqlite ctor";
}

QJsonObject Json2Sqlite::loadSettings(QString dbPath, QJsonObject jsonSettings)
{
    if(!dbPath.isEmpty() && !jsonSettings.isEmpty())
    {
        if(!doesDatabaseExist(dbPath))
            makeInitalSettingsDatabase(dbPath);

        QSqlDatabase database;
        database = database.addDatabase("QSQLITE", "settings");
        database.setDatabaseName(dbPath);

        if(!database.open())
        {
            qDebug() << "db failed to load in OAuth2 loadSettings"
                     << "All settings values are null!";

            return jsonSettings;
        }

        QSqlQuery query(database);
        QString queryString  = "SELECT * FROM settings WHERE key IN (";

        for(auto key: jsonSettings.keys()){
            queryString.append("'" + key + "', ");
        }
        queryString.remove((queryString.length() - 2), 2);
        queryString.append(")");

        query.prepare(queryString);

        if(query.exec())
        {
            qDebug() << "Json2Sqlite::loadSettings query success.";

            while(query.next())
            {
                if(query.value("isJsonArray").toBool())
                    jsonSettings[query.value("key").toString()] = QJsonDocument::fromJson(query.value("value").toString().toUtf8()).array();
                else
                    jsonSettings[query.value("key").toString()] = query.value("value").toJsonValue();
            }
        }
        else
        {
            qDebug() << "Json2Sqlite::loadSettings ERROR: " << query.lastError();
        }
        query.clear();

        database.close();
        database = QSqlDatabase();
        database.removeDatabase("settings");

        return jsonSettings;
    }
    else
    {
        qDebug() << "Json2Sqlite::loadSettings error, dbPath or JsonObject is null";
        return QJsonObject();
    }
}

QJsonArray Json2Sqlite::loadArray(QString dbPath, QString table)
{
    QString dbPathh = dbPath;
    QString tableh = table;
    return QJsonArray();
}

bool Json2Sqlite::saveSettings(QString dbPath, QJsonObject jsonSettings)
{
    bool success = false;

    if(!dbPath.isEmpty()){
        if(!doesDatabaseExist(dbPath))
            makeInitalSettingsDatabase(dbPath);

        QString queryString = "INSERT or REPLACE into settings VALUES ";

        QSqlDatabase database;

        database = database.addDatabase("QSQLITE", "settings");
        database.setDatabaseName(dbPath);

        if(!database.open())
        {
            qDebug() << "db failed to load in OAuth2 loadSettings"
                     << "All settings values are null!";

            return false;
        }

        QSqlQuery query(database);

        for(auto key: jsonSettings.keys())
        {
            if(jsonSettings[key].isArray())
            {
                QJsonDocument arrayToString;
                arrayToString.setArray(jsonSettings[key].toArray());
                queryString.append("('" + key + "', '" + QString(arrayToString.toJson()) + "', '" + QString::number(true) + "'),");
            }
            else{
                queryString.append("('" + key + "', '" + jsonSettings[key].toString() + "', '" + QString::number(false) + "'),");
            }
        }
        queryString.remove(queryString.length() - 1, 1);

        query.prepare(queryString);
        if(query.exec())
        {
            qDebug() << "OAuth2::saveSettings query success.";
            success = true;
        }
        else
        {
            qDebug() << "OAuth2::saveSettings ERROR: " << query.lastError();
            success = false;
        }

        query.clear();
        database.close();
        database = QSqlDatabase();
        database.removeDatabase("settings");
        return success;
    }
    else
    {
        return success;
    }
}

bool Json2Sqlite::saveArray(QString dbPath, QString table, QString primaryKey, QJsonArray jsonArray)
{

    QSet<QString> uniqueKeys;

    QJsonArray typesOfObjects;
    for(auto jsonValue : jsonArray)
    {
        bool knownObject = false;
        QJsonObject jsonObject = jsonValue.toObject();

        for(auto typeOfValue : typesOfObjects)
        {
            QJsonObject typeOfObject = typeOfValue.toObject();
            if(jsonObject.keys() == typeOfObject.keys())
                knownObject = true;
        }
        if(!knownObject)
        {
            typesOfObjects.append(jsonValue);
            for(auto key : jsonObject.keys())
                uniqueKeys.insert(key);
        }
    }

    bool success = false;

    if(!doesDatabaseExist(dbPath))
        makeInitialArrayDatabase(dbPath, table, primaryKey, jsonArray);

    //Build key list...
    QStringList keyList;
    if(!jsonArray.isEmpty())
        keyList = jsonArray.first().toObject().keys();

    //Build base query...
    QString queryString = "INSERT or REPLACE into " + table + "(";
    for(auto key : uniqueKeys)
    {
        queryString.append("'" + key + "'" + ", ");
    }
    queryString.remove(queryString.length() - 2, 2);
    queryString.append(") VALUES ");

    //Appending value tuples
    for(auto arrayValue : jsonArray)
    {
        QString valueTuple = "(";
        QJsonObject arrayObject = arrayValue.toObject();

        for(auto key: uniqueKeys)
        {
            if(arrayObject[key].isArray())
            {
                QJsonDocument arrayToString;
                arrayToString.setArray(arrayObject[key].toArray());
                valueTuple.append("'" + QString(arrayToString.toJson(QJsonDocument::Compact)).replace("'", "''") + "', ");
            }
            else{
                valueTuple.append("'" + arrayObject[key].toVariant().toString().replace("'", "''") + "', ");
            }
        }
        valueTuple.remove(valueTuple.length() - 2, 2);
        valueTuple.append("), ");
        queryString.append(valueTuple);
    }
    queryString.remove(queryString.length() - 2, 2);

    //qDebug() << queryString;

    //Done building query open db connection
    QSqlDatabase database;
    database = database.addDatabase("QSQLITE", "array");
    database.setDatabaseName(dbPath);
    QSqlQuery query(database);

    if(!database.open())
    {
        qDebug() << "db failed to load in Json2Sqlite saveArray"
                 << "All settings values are null!";

        return false;
    }

//    //PREP TABLE FOR NEW ARRAY
//    //An append function would just add to table...
//    query.prepare("DELETE FROM " + table);
//    query.exec();

    query.prepare(queryString);

    if(query.exec())
    {
        qDebug() << "Json2Sqlite::saveArray query success.";
        success = true;
    }
    else
    {
        qDebug() << "Json2Sqlite::saveArray ERROR: " << query.lastError();
        success = false;
    }

    query.clear();
    database.close();
    database = QSqlDatabase();
    database.removeDatabase("array");
    return success;
}

bool Json2Sqlite::doesDatabaseExist(QString dbPath)
{
    QFileInfo checkFile(dbPath);
    if(checkFile.exists() && checkFile.isFile())
        return true;
    else
        return false;
}

int Json2Sqlite::numberOfTypesInArray(QJsonArray jsonArray)
{
    QJsonArray typesOfObjects;
    for(auto jsonValue : jsonArray)
    {
        bool knownObject = false;
        QJsonObject jsonObject = jsonValue.toObject();

        for(auto typeOfValue : typesOfObjects)
        {
            QJsonObject typeOfObject = typeOfValue.toObject();
            if(jsonObject.keys() == typeOfObject.keys())
                knownObject = true;
        }
        if(!knownObject)
            typesOfObjects.append(jsonValue);
    }
    return typesOfObjects.size();
}

bool Json2Sqlite::makeInitialArrayDatabase(QString dbPath, QString table, QString primaryKey, QJsonArray jsonArray)
{
    //-------------------------------------------------------------------------
    // Key Check Subsection
    //-------------------------------------------------------------------------
    //Create a key check for arrays.
    //This will compare the keys of each contained object against each other and determine how many
    //objects that this json array contains.

//    int numOfTypes = numberOfTypesInArray(jsonArray);


//    if(numOfTypes != 1)
//    {
//        qDebug() << "Json2Sqlite::makeInitialArrayDatabase - Does not have one object. It has more or less than one object";
//        qDebug() << "Found " << numOfTypes << " types.";
//        return false;
//    }
    QSet<QString> uniqueKeys;

    QJsonArray typesOfObjects;
    for(auto jsonValue : jsonArray)
    {
        bool knownObject = false;
        QJsonObject jsonObject = jsonValue.toObject();

        for(auto typeOfValue : typesOfObjects)
        {
            QJsonObject typeOfObject = typeOfValue.toObject();
            if(jsonObject.keys() == typeOfObject.keys())
                knownObject = true;
        }
        if(!knownObject)
        {
            typesOfObjects.append(jsonValue);
            for(auto key : jsonObject.keys())
                uniqueKeys.insert(key);
        }
    }

    qDebug() << uniqueKeys;

    //-------------------------------------------------------------------------
    // End Key Check Subsection
    //-------------------------------------------------------------------------

    bool success;
    QSqlDatabase database;
    QString queryString;

    database = database.addDatabase("QSQLITE", "array");
    database.setDatabaseName(dbPath);

    if(!database.open())
    {
        qDebug() << "db failed to load in OAuth2::makeInitalDatabase"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(database);
    queryString = "CREATE TABLE " + table + "(";


    QJsonObject jsonObject = jsonArray.first().toObject();
    QStringList jsonObjectKeys = jsonObject.keys();

    for(auto key : uniqueKeys)
    {
        if(key == primaryKey)
        {
            queryString.append("'" + key + "'" + " text PRIMARY KEY, ");
        }
        else
        {
            queryString.append("'" + key + "'" + " text, ");
        }
    }


    queryString.remove(queryString.length() - 2, 2);
    queryString.append(")");


    query.prepare(queryString);
    success = query.exec();
    query.clear();

    database.close();
    database = QSqlDatabase();
    database.removeDatabase("array");
    return success;
}

bool Json2Sqlite::makeInitalSettingsDatabase(QString dbPath)
{
    bool success;
    QSqlDatabase database;

    database = database.addDatabase("QSQLITE", "settings");
    database.setDatabaseName(dbPath);

    if(!database.open())
    {
        qDebug() << "db failed to load in OAuth2::makeInitalDatabase"
                 << "All settings values are null!";

        return false;
    }

    QSqlQuery query(database);
    query.prepare("CREATE TABLE settings(key text PRIMARY KEY, value text, isJsonArray integer)");
    success = query.exec();
    query.clear();

    database.close();
    database = QSqlDatabase();
    database.removeDatabase("settings");
    return success;
}
