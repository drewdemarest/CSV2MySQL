#ifndef DATAINFO_HPP
#define DATAINFO_HPP

#include <QtCore>
enum SQLiteType{INTEGER, REAL, TEXT, TEXT_DATE, TEXT_TIME, TEXT_DATETIME, BLOB, NULL_SQL};

struct DataInfo
{
    DataInfo(){}
    DataInfo(int csvColumn, int sqliteColumn, SQLiteType sqliteType, bool primaryKey = false, QString dateTimeFormat = QString(), int modYear = 0, qint64 modSec = 0)
    {
        csvColumn_ = csvColumn;
        sqliteColumn_ = sqliteColumn;
        sqliteType_ = sqliteType;
        primaryKey_ = primaryKey;
        dateTimeFormat_ = dateTimeFormat;
        modSec_  = modSec;
        modYear_ = modYear;

        switch(sqliteType)
        {
        case SQLiteType::TEXT_DATE :
            date_ = true;
            break;

        case SQLiteType::TEXT_TIME :
            time_ = true;
            break;

        case SQLiteType::TEXT_DATETIME :
            time_ = true;
            date_ = true;
            break;

        default: break;

        }
    }

    DataInfo(const DataInfo &other){
        csvColumn_ = other.csvColumn_;
        sqliteColumn_ = other.sqliteColumn_;
        sqliteType_ = other.sqliteType_;
        primaryKey_ = other.primaryKey_;
        date_ = other.date_;
        time_ = other.time_;
        dateTime_ = other.dateTime_;
        dateTimeFormat_ = other.dateTimeFormat_;
        modSec_ = other.modSec_;
        modYear_ = other.modYear_;
    }
    ~DataInfo(){}

    //    DataInfo operator=(const DataInfo &dbi) const
    //    {
    //        return DataInfo(dbi.csvColumn_, this->sqliteColumn_, this->sqliteType_, this->date_, this->time_, this->dateTimeFormat_, this->modSec_, this->modYear_);
    //    }

    bool operator==(const DataInfo &dbi) const
    {
        if(this->csvColumn_ == dbi.csvColumn_
                && this->sqliteColumn_ == dbi.sqliteColumn_
                && this->primaryKey_ == dbi.primaryKey_
                && this->sqliteType_ == dbi.sqliteType_
                && this->date_ == dbi.date_
                && this->time_ == dbi.time_
                && this->dateTime_ == dbi.dateTime_
                && this->dateTimeFormat_ == dbi.dateTimeFormat_
                && this->modSec_ == dbi.modSec_
                && this->modYear_ == dbi.modYear_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool primaryKey_ = false;
    bool date_ = false;
    bool time_ = false;
    bool dateTime_ = false;

    qint64 modSec_ = 0;
    int modYear_ = 0;

    int csvColumn_ = 0;
    int sqliteColumn_ = 0;
    SQLiteType sqliteType_ = SQLiteType::NULL_SQL;
    QString dateTimeFormat_ = QString();

    bool isDate()const{return date_;}
    bool isTime()const{return time_;}
    bool isDateTime()const{return dateTime_;}
    bool isPrimaryKey()const{return primaryKey_;}
    int getCSVColumn()const{return csvColumn_;}
    int getSQLiteColumn()const{return sqliteColumn_;}
    SQLiteType getSQLiteType()const{return sqliteType_;}
    QString getDateTimeFormat()const{return dateTimeFormat_;}
    qint64 getModSec()const{return modSec_;}
    int getModYear()const{return modYear_;}

};
Q_DECLARE_METATYPE(DataInfo)

typedef QMap<QString, DataInfo> DataInfoMap;
typedef QMap<QString, DataInfoMap> TableInfoMap;
#endif // DATAINFO_HPP
