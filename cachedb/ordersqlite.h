#ifndef ORDERSQLITE_H
#define ORDERSQLITE_H

#include <QtConcurrent/QtConcurrent>
#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QApplication>
#include <QSqlError>
#include "entity/customer/customer.h"
#include <malloc.h>

struct DataInfo
{
    DataInfo(){}
    DataInfo(int csvColumn, int sqliteColumn, QString sqliteType,
             bool isDate=false, bool isTime=false, QString dateTimeFormat = QString(), qint64 modSec = 0, int modYear = 0)
    {
        csvColumn_ = csvColumn;
        sqliteColumn_ = sqliteColumn;
        sqliteType_ = sqliteType;
        date_ = isDate;
        time_ = isTime;
        dateTimeFormat_ = dateTimeFormat;
        modSec_  = modSec;
        modYear_ = modYear;

        if(date_ && time_)
            dateTime_ = true;

    }
    DataInfo(const DataInfo &other){
        csvColumn_ = other.csvColumn_;
        sqliteColumn_ = other.sqliteColumn_;
        sqliteType_ = other.sqliteType_;
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

    bool date_ = false;
    bool time_ = false;
    bool dateTime_ = false;

    qint64 modSec_ = 0;
    int modYear_ = 0;

    int csvColumn_ = 0;
    int sqliteColumn_ = 0;
    QString sqliteType_ = QString();
    QString dateTimeFormat_ = QString();

    bool isDate()const{return date_;}
    bool isTime()const{return time_;}
    bool isDateTime()const{return dateTime_;}
    int getCSVColumn()const{return csvColumn_;}
    int getSQLiteColumn()const{return sqliteColumn_;}
    QString getSQLiteType()const{return sqliteType_;}
    QString getDateTimeFormat()const{return dateTimeFormat_;}
    qint64 getModSec()const{return modSec_;}
    int getModYear()const{return modYear_;}

};
Q_DECLARE_METATYPE(DataInfo)

typedef QMap<QString, DataInfo> DataInfoMap;
typedef QMap<QString, DataInfoMap> TableInfoMap;


class OrderSQLite : public QObject
{
    Q_OBJECT
public:
    explicit OrderSQLite(const QString &dbPath, QObject *parent = nullptr);
    bool doesDatabaseExist() const;

    QList<QVariantMap> getOrdersFromDB
    (const int minInvoiceNum, const int maxInvoiceNum) const;

    QList<QVariantMap> getOrdersFromDB
    (const QDate &minOrderDate, const QDate &maxOrderDate) const;

    TableInfoMap getKnownFormats() const;
    void addFormat(const QString &tableName, const DataInfoMap &dim);
    bool populateOrders() const;

    QVector<Order> getOrdersForRoutes(const QDate &min,const QDate &max,const QVector<QString> &routes);

    const DataInfoMap formatOrderTrackingCSV_
    {{"cdl",                DataInfo(0,1,"INTEGER")},
        {"orderType",           DataInfo(1,2,"TEXT")},
        {"orderDate",           DataInfo(2,3,"TEXT", true, false, "MM/dd/yy", 0, 100)},
        {"invoiceDate",         DataInfo(3,4,"TEXT", true, false, "MM/dd/yy", 0, 100)},
        {"orderTime",           DataInfo(4,5,"TEXT", false, true, "HH:mm:ss", 0, 0)},
        {"rep",                 DataInfo(5,6,"TEXT")},
        {"terms",               DataInfo(6,7,"TEXT")},
        {"customerNumber",      DataInfo(7,8,"INTEGER")},
        {"customerName",        DataInfo(8,9,"TEXT")},
        {"customerBillingName", DataInfo(9,10,"TEXT")},
        {"routeKey",            DataInfo(10,11,"TEXT")},
        {"stopNumber",          DataInfo(11,12,"INTEGER")},
        {"specialInstructions", DataInfo(12,13,"TEXT")},
        {"poNumber",            DataInfo(13,14,"TEXT")},
        {"invoiceNumber",       DataInfo(14,0,"INTEGER PRIMARY KEY")},
        {"quantityShipped",     DataInfo(15,15,"REAL")},
        {"extSales",            DataInfo(16,16,"REAL")},
        {"invoiceAmmount",      DataInfo(17,17,"REAL")},
        {"user",                DataInfo(18,18,"TEXT")},
        {"user2",               DataInfo(19,19,"TEXT")}};

    const DataInfoMap formatRouteProfitabilityCSV_
    {{"routeKey",               DataInfo(0,1, "TEXT")},
        {"customerNumber",          DataInfo(1,2, "INTEGER")},
        {"customerName",            DataInfo(2,3, "TEXT")},
        {"masterCustomerNumber",    DataInfo(3,4, "INTEGER")},
        {"invoiceDate",             DataInfo(4,5, "TEXT", true, false, "MM/dd/yy", 0, 100)},
        {"invoiceNumber",           DataInfo(5,0, "INTEGER PRIMARY KEY")},
        {"numberOfOrders",          DataInfo(6,6, "INTEGER")},
        {"pieces",                  DataInfo(7,7, "REAL")},
        {"weight",                  DataInfo(8,8, "REAL")},
        {"cube",                    DataInfo(9,9, "REAL")},
        {"netSales",                DataInfo(10,10,"REAL")},
        {"cost",                    DataInfo(11,11,"REAL")},
        {"profit",                  DataInfo(12,12,"REAL")},
        {"profitPercent",           DataInfo(13,13,"REAL")}};

    const DataInfoMap formatOrderObj_
    {{"invoiceNumber",        DataInfo(0,0,"INTEGER PRIMARY KEY")},
        {"customerNumber",        DataInfo(1,1, "INTEGER")},
        {"customerName",          DataInfo(2,2,"TEXT")},
        {"masterCustomerNumber",  DataInfo(3,3,"INTEGER")},
        {"routeKey",              DataInfo(4,4,"TEXT")},
        {"stopNumber",            DataInfo(5,5,"INTEGER")},
        {"orderDateTime",         DataInfo(6,6,"TEXT")},
        {"invoiceDate",           DataInfo(7,7,"TEXT", true, false, "MM/dd/yy", 0, 100)},
        {"pieces",                DataInfo(8,8,"REAL")},
        {"weight",                DataInfo(9,9,"REAL")},
        {"cube",                  DataInfo(10,10,"REAL")},
        {"netSales",              DataInfo(11,11,"REAL")},
        {"cost",                  DataInfo(12,12,"REAL")},
        {"profit",                DataInfo(13,13,"REAL")}};

    const DataInfoMap formatTruckDriverAssignCSV_
    {{"division",  DataInfo(0,1, "TEXT")},
        {"routeKey",   DataInfo(1,0, "TEXT")},
        {"routeName",  DataInfo(2,2, "TEXT")},
        {"whs1Cutoff", DataInfo(3,3, "TEXT", false, true, "HH:mm", 0, 0)},
        {"whs2Cutoff", DataInfo(4,4, "TEXT", false, true, "HH:mm", 0, 0)},
        {"truck",      DataInfo(5,5, "TEXT")},
        {"driverID",   DataInfo(6,6,"INTEGER")},
        {"driverName", DataInfo(7,7,"TEXT")}};

    const DataInfoMap formatCustRoutesTimeWindowCSV_
    {{"division",           DataInfo(0, 1, "TEXT")},
        {"rep",                 DataInfo(1, 2, "TEXT")},
        {"customerNumber",      DataInfo(2, 0, "INTEGER PRIMARY KEY")},
        {"customerName",        DataInfo(3, 3, "TEXT")},
        {"customerAddr1",       DataInfo(4, 4, "TEXT")},
        {"customerAddr2",       DataInfo(5, 5, "TEXT")},
        {"city",                DataInfo(6, 6, "TEXT")},
        {"state",               DataInfo(7, 7, "TEXT")},
        {"lastOrder",           DataInfo(8, 8, "TEXT", true, false, "MM/dd/yy", 0, 100)},
        {"customerChain",       DataInfo(9, 9, "TEXT")},
        {"cstuomerGroup",       DataInfo(10, 10, "TEXT")},
        {"timeWindow1Open",     DataInfo(11, 11, "TEXT", false, true, "hmm")},
        {"timeWindow1Close",    DataInfo(12, 12, "TEXT", false, true, "hmm")},
        {"mondayRoute",         DataInfo(13, 13, "TEXT")},
        {"mondayStop",          DataInfo(14, 14, "INTEGER")},
        {"tuesdayRoute",        DataInfo(15, 15, "TEXT")},
        {"tuesdayStop",         DataInfo(16, 16, "INTEGER")},
        {"wednesdayRoute",      DataInfo(17, 17, "TEXT")},
        {"wednesdayStop",       DataInfo(18, 18, "INTEGER")},
        {"thursdayRoute",       DataInfo(19, 19, "TEXT")},
        {"thursdayStop",        DataInfo(20, 20, "INTEGER")},
        {"fridayRoute",         DataInfo(21, 21, "TEXT")},
        {"fridayStop",          DataInfo(22, 22, "INTEGER")},
        {"saturdayRoute",       DataInfo(23, 23, "TEXT")},
        {"saturdayStop",        DataInfo(24, 24, "INTEGER")},
        {"sundayRoute",         DataInfo(25, 25, "TEXT")},
        {"sundayStop",          DataInfo(26, 26, "INTEGER")},
        {"timeWindow2Open",     DataInfo(27, 27, "TEXT", false, true, "hmm")},
        {"timeWindow2Close",    DataInfo(28, 28, "TEXT", false, true, "hmm")},
        {"openTime",            DataInfo(29, 29, "TEXT", false, true, "hmm")},
        {"closeTime",           DataInfo(30, 30, "TEXT", false, true, "hmm")}};

    const DataInfoMap formatCustomerChainGroupCSV_
    {
        {"company",                 DataInfo(0,1, "INTEGER")},
        {"division",                DataInfo(1,2,"INTEGER")},
        {"location",                DataInfo(2,3,"INTEGER")},
        {"warehouse",               DataInfo(3,4,"INTEGER")},
        {"customerNumber",          DataInfo(4,0,"INTEGER PRIMARY KEY")},
        {"storeNumber",             DataInfo(5,5,"TEXT")},
        {"stopNumber",              DataInfo(6,6,"INTEGER")},
        {"phoneNumber",             DataInfo(7,7,"TEXT")},
        {"billToName",              DataInfo(8,8,"TEXT")},
        {"chainID",                   DataInfo(9,9, "TEXT")},
        {"chainDescription",        DataInfo(10,10, "TEXT")},
        {"groupID",                 DataInfo(11,11, "TEXT")},
        {"groupDescription",        DataInfo(12,12, "TEXT")},
        {"type",                    DataInfo(13,13,"INTEGER")},
        {"ediFlag",                 DataInfo(14,14,"TEXT")},
        {"itemMiscTax",             DataInfo(15,15, "TEXT")},
        {"poRequired",              DataInfo(16,16,"TEXT")},
        {"salesLoc",                DataInfo(17,17, "TEXT")},
        {"territory",               DataInfo(18,18,"TEXT")},
        {"rep",                     DataInfo(19,19,"TEXT")},
        {"merchaniser",             DataInfo(20,20,"TEXT")},
        {"exclusionNumber",        DataInfo(21,21,"TEXT")},
        {"proprietaryDesc",         DataInfo(22,22,"TEXT")},
        {"arFeeCode",               DataInfo(23,23,"TEXT")},
        {"arFeeAmount",             DataInfo(24,24,"TEXT")},
        {"routeInvoicng",           DataInfo(25,25,"TEXT")},
        {"zone",                    DataInfo(26,26,"TEXT")},
        {"guide",                   DataInfo(27,27,"TEXT")},
        {"authorized",              DataInfo(28,28,"TEXT")},
        {"subs",                    DataInfo(29,29,"TEXT")},
        {"autoUpdateGuide",         DataInfo(30,30,"TEXT")},
        {"cool",                    DataInfo(31,31,"TEXT")},
        {"distributionFeeCode",     DataInfo(32,32,"TEXT")},
        {"firstInvoiceDate",        DataInfo(33,33,"TEXT", true,false,"yyyyMMdd")},
        {"lastInvoiceDate",          DataInfo(34,34,"TEXT", true,false,"yyyyMMdd")},
        {"terms",                   DataInfo(35,35,"TEXT")},
        {"shipToName",              DataInfo(36,36,"TEXT")},
        {"customerAddress1",        DataInfo(37,37,"TEXT")},
        {"customerAddress2",        DataInfo(38,38,"TEXT")},
        {"city",                    DataInfo(39,39,"TEXT")},
        {"state",                   DataInfo(40,40,"TEXT")},
        {"zipCode",                 DataInfo(41,41,"TEXT")},
        {"mondayRoute",             DataInfo(42, 42, "TEXT")},
        {"mondayStop",              DataInfo(43, 43, "INTEGER")},
        {"tuesdayRoute",            DataInfo(44, 44, "TEXT")},
        {"tuesdayStop",             DataInfo(45, 45, "INTEGER")},
        {"wednesdayRoute",          DataInfo(46, 46, "TEXT")},
        {"wednesdayStop",           DataInfo(47, 47, "INTEGER")},
        {"thursdayRoute",           DataInfo(48, 48, "TEXT")},
        {"thursdayStop",            DataInfo(49, 49, "INTEGER")},
        {"fridayRoute",             DataInfo(50, 50, "TEXT")},
        {"fridayStop",              DataInfo(51, 51, "INTEGER")},
        {"saturdayRoute",           DataInfo(52, 52, "TEXT")},
        {"saturdayStop",            DataInfo(53, 53, "INTEGER")},
        {"sundayRoute",             DataInfo(54, 54, "TEXT")},
        {"sundayStop",              DataInfo(55, 55, "INTEGER")},
        {"salesCurrent",            DataInfo(56,56,"REAL")},
        {"custCurrent",             DataInfo(57,57,"REAL")},
        {"profitCurrent",           DataInfo(58,58,"REAL")},
        {"profitPercentCurrent",    DataInfo(59,59,"REAL")},
        {"salesPrevious",           DataInfo(60,60,"REAL")},
        {"costPrevious",            DataInfo(61,61,"REAL")},
        {"profitPrevious",          DataInfo(62,62,"REAL")},
        {"profitPercentPrevious",   DataInfo(63,63,"REAL")}};

public slots:
    bool importCSVtoSQLite(const DataInfoMap  &csvFormat,
                           const QString      &tableName,
                           const bool         hasHeaders,
                           const QString      &filePath,
                           const int          &chunkSize);

private:
    QString dbPath_;

    bool makeOrderDB();
    bool saveToDB(const QString &tableName, const DataInfoMap &format, const QVector<QString> &linesIn);
    QString createNewTable(const QString &tableName, const DataInfoMap &format);
    QString insertOrIgnoreInto(const QString &tableName, const DataInfoMap &format, const QVector<QString> &csvRows);

    //Static functions for threading.
    static QString createValueTuple(QVector<QString> csvLine, const QVector<QString> &sqliteTypes);
    static QVector<QString> parseCSVLine(const QString &csvLine);
    static QString csvLineToValueString(const QString &csvLine, const QVector<DataInfo> &sqliteTypes);

    TableInfoMap knownFormats_ = {{"OrderTrackingCSV",          formatOrderTrackingCSV_},
                                  {"RouteProfitabilityCSV",     formatRouteProfitabilityCSV_},
                                  {"OrderObj",                  formatOrderObj_},
                                  {"TruckDriverAssignCSV",      formatTruckDriverAssignCSV_},
                                  {"CustRoutesTimeWindowCSV",   formatCustRoutesTimeWindowCSV_},
                                  {"CustomerChainGroupCSV",         formatCustomerChainGroupCSV_}};


signals:
    void importFinished(const bool &success);

public slots:
};

#endif // ORDERSQLITE_H
