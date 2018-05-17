#ifndef MYSQLEXPORTER_H
#define MYSQLEXPORTER_H

#include <QtCore>
#include <QObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "datainfo/datainfo.hpp"
#include "json2sqlite/json2sqlite.h"

enum SQLComp{none, eq, lt, gt, lte, gte};

struct SQLCompare{
    SQLCompare(){}
    SQLCompare(const QString &tableA, const QString &columnA,
               const QString &tableB, const QString &columnB,
               SQLComp sqlOperator)
    {
        tableA_ = tableA;
        tableB_ = tableB;
        columnA_ = columnA;
        columnB_ = columnB;
        sqlOperator_ = sqlOperator;

        if(tableA_.isNull()  ||
           tableB_.isNull()  ||
           columnA_.isNull() ||
           columnB_.isNull())
        {
            valid_ = false;
        }
        else
        {
            valid_ = true;
        }

        switch(sqlOperator)
        {
        case (SQLComp::eq):
            sqlOperatorStr_ = " = ";
            break;
        case (SQLComp::lt):
            sqlOperatorStr_ = " < ";
            break;
        case (SQLComp::gt):
            sqlOperatorStr_ = " > ";
            break;
        case (SQLComp::lte):
            sqlOperatorStr_ = " <= ";
            break;
        case (SQLComp::gte):
            sqlOperatorStr_ = " >= ";
            break;
        default:
            valid_ = false;
            break;
        }

    }

    bool operator<(const SQLCompare& other)
    {
        if(this->tableA_ < other.tableA_)
        {
            return true;
        }

        if(this->tableA_ == other.tableA_ && this->columnA_ < other.columnA_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    QString toString(){return QString(tableA_ + "." + columnA_ + sqlOperatorStr_ +  tableB_ + "." + columnB_);}
    bool isValid()const{return valid_;}

    QString tableA_;
    QString tableB_;
    QString columnA_;
    QString columnB_;
    SQLComp sqlOperator_;
    QString sqlOperatorStr_;

private:
    bool valid_ = false;
};

class DBtoDB{

public:

    DBtoDB(){}
    DBtoDB(const QString &tableFrom, const QString &columnFrom,
           const  QString &tableTo, const QString &columnTo)
    {
        tableFrom_ = tableFrom;
        tableTo_ = tableTo;
        columnFrom_ = columnFrom;
        columnTo_ = columnTo;

        if(tableFrom.isEmpty())
        {
            qDebug() << "Error tableFrom is empty in DBtoDB format";
            valid_ = false;
        }

        if(tableTo.isEmpty())
        {
            qDebug() << "Error tableTo is empty in DBtoDB format";
            valid_ = false;
        }

        if(columnFrom.isEmpty())
        {
            qDebug() << "Error columnFrom is empty in DBtoDB format";
            valid_ = false;

        }

        if(columnTo.isEmpty())
        {
            qDebug() << "Error columnTo is empty in DBtoDB format";
            valid_ = false;

        }
    }

    ~DBtoDB(){}

    bool operator<(const DBtoDB& other)
    {
        if(this->tableFrom_ < other.tableFrom_)
        {
            return true;
        }

        if(this->tableFrom_ == other.tableFrom_ && this->columnFrom_ < other.columnFrom_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool    isValid()const{return valid_;}
    QString getTableFrom()const{return tableFrom_;}
    QString getTableTo()const{return tableTo_;}
    QString getColumnFrom()const{return columnFrom_;}
    QString getColumnTo()const{return columnTo_;}

private:
    bool valid_ = false;
    QString tableFrom_;
    QString tableTo_;
    QString columnFrom_;
    QString columnTo_;

};

class DBTransferFormat
{
public:
    DBTransferFormat(){
        std::sort(sqlCompares_.begin(),sqlCompares_.end());
        std::sort(dbFormat_.begin(),dbFormat_.end());
    }
    ~DBTransferFormat(){}

    QString makeSelectFromDB1(){
        QString queryString = "SELECT ";
        QStringList db2dbList;
        QVector<QString> tables;
        for(auto dbdb:dbFormat_)
        {
            db2dbList.append(dbdb.getTableFrom() + "." + dbdb.getColumnFrom());
            if(!tables.contains(dbdb.getTableFrom()))
                tables.append(dbdb.getTableFrom());
        }

        queryString.append(db2dbList.join(", "));
        queryString.append(" FROM ");
        if(tables.size() == 1)
        {
            for(auto table:tables)
                queryString.append(table);
        }

        if(tables.size()> 1)
        {
            for(int i = 0; i<tables.size();++i)
            {
                if(i == 0)
                {
                    queryString.append(tables[i] + " ");
                    ++i;
                }
                queryString.append("INNER JOIN " + tables[i] + " ON ");
                for(auto compare:sqlCompares_)
                {
                    if(compare.tableA_ == tables[i])
                    {
                        queryString.append(compare.toString() + " ");
                        break;
                    }
                }
            }
        }

        return queryString;

    }

    QString makeReplaceIntoDB2(const QStringList &valueTuples)
    {
        QString queryString; queryString.reserve(10000);
        QStringList values;

        for(auto dbdb:dbFormat_)
        {
            values.append(dbdb.getTableTo() + "." + dbdb.getColumnTo());
        }

        queryString.append("REPLACE INTO (" + values.join(", ") +  ") VALUES " + valueTuples.join(", "));
        qDebug() << queryString;
        values.clear();
        return queryString;
    }

    bool setSQLCompares(const QVector<SQLCompare> sqlCompares)
    {
        for(auto compare:sqlCompares)
            if(!compare.isValid())
            {
                qDebug() << "Failed to import sqlCompares. Not all compares are valid";
                return false;
            }

        sqlCompares_ = sqlCompares;
        return true;
    }

    bool setDBtoDBFormat(const QVector<DBtoDB> dbFormats)
    {
        for(auto format:dbFormats)
            if(!format.isValid())
            {
                qDebug() << "Failed to import dbFormats. Not all DBtoDB formats are valid";
                return false;
            }

        dbFormat_ = dbFormats;
        return true;}

private:

/*  // Test Code
    QVector<SQLCompare> sqlCompares
    {
        SQLCompare("E", "x", "D", "x", SQLComp::eq),
        SQLCompare("D", "x", "B", "x", SQLComp::eq),
        SQLCompare("C", "x", "A", "x", SQLComp::eq),
        SQLCompare("A", "x", "E", "x", SQLComp::eq),
        SQLCompare("B", "x", "E", "x", SQLComp::eq)
    };

    QVector<SQLCompare> sqlCompares
    {
        SQLCompare("C","x","A","x",SQLComp::eq),
        SQLCompare("A","x","B","x",SQLComp::eq),
        SQLCompare("D","x","C","x",SQLComp::eq),
        SQLCompare("B","x","E","x",SQLComp::eq),
        SQLCompare("E","x","A","x",SQLComp::eq)
    };

    QVector<DBtoDB> dbFormat_
    {
        DBtoDB("C","c","Z","z"),
        DBtoDB("A","a","X","x"),
        DBtoDB("D","d","W","w"),
        DBtoDB("B","b","Y","y"),
        DBtoDB("E","e","V","v")
    };
*/

    QVector<SQLCompare> sqlCompares_;
    QVector<DBtoDB> dbFormat_;
};


class mysqlExporter : public QObject
{
    Q_OBJECT
public:
    explicit mysqlExporter(QObject *parent = nullptr);

    bool exportQueryString(const QString &queryString);
    bool exportToMySQL(int chunkSize, const QVector<DBtoDB> &dbFormat, const QVector<SQLCompare> &sqlCompares);
    bool exportInvoiceToMySQL(int chunkSize);
    bool exportCustomInvoiceToMySQL(int chunkSize);




    const QMap<QString, QString> formatCustomerChainGroupCSV_
    {
        {"CustomerChainGroupCSV.company",                 "company"},
        {"CustomerChainGroupCSV.division",                "division"},
        {"CustomerChainGroupCSV.location",                "location"},
        {"CustomerChainGroupCSV.warehouse",               "warehouse"},
        {"CustomerChainGroupCSV.customerNumber",          "customerNumber"},
        {"CustomerChainGroupCSV.storeNumber",             "storeNumber"},
        {"CustomerChainGroupCSV.stopNumber",              "stopNumber"},
        {"CustomerChainGroupCSV.phoneNumber",             "phoneNumber"},
        {"CustomerChainGroupCSV.billToName",              "billToName"},
        {"CustomerChainGroupCSV.chainID",                 "chain"},
        {"CustomerChainGroupCSV.chainDescription",        "chainDescription"},
        {"CustomerChainGroupCSV.groupID",                 "groupID"},
        {"CustomerChainGroupCSV.groupDescription",        "groupDescription"},
        {"CustomerChainGroupCSV.type",                    "type"},
        {"CustomerChainGroupCSV.ediFlag",                 "ediFlag"},
        {"CustomerChainGroupCSV.itemMiscTax",             "itemMiscTax"},
        {"CustomerChainGroupCSV.salesLoc",                "salesLocation"},
        {"CustomerChainGroupCSV.territory",               "territory"},
        {"CustomerChainGroupCSV.rep",                     "rep"},
        {"CustomerChainGroupCSV.merchaniser",             "merchandiser"},
        {"CustomerChainGroupCSV.exclusionNumber",         "exclusionNumber"},
        {"CustomerChainGroupCSV.proprietaryDesc",         "proprietaryDesc"},
        {"CustomerChainGroupCSV.arFeeCode",               "arFeeCode"},
        {"CustomerChainGroupCSV.arFeeAmount",             "arFeeAmount"},
        {"CustomerChainGroupCSV.routeInvoicng",           "routeInvoicing"},
        {"CustomerChainGroupCSV.zone",                    "zone"},
        {"CustomerChainGroupCSV.firstInvoiceDate",        "firstInvoiceDate"},
        {"CustomerChainGroupCSV.lastInvoiceDate",         "lastInvoiceDate"},
        {"CustomerChainGroupCSV.terms",                   "terms"},
        {"CustomerChainGroupCSV.shipToName",              "customerName"},
        {"CustomerChainGroupCSV.customerAddress1",        "customerAddress1"},
        {"CustomerChainGroupCSV.customerAddress2",        "customerAddress2"},
        {"CustomerChainGroupCSV.city",                    "city"},
        {"CustomerChainGroupCSV.state",                   "State"},
        {"CustomerChainGroupCSV.zipCode",                 "zipCode"},
        {"CustomerChainGroupCSV.mondayRoute",             "mondayRoute"},
        {"CustomerChainGroupCSV.mondayStop",              "mondayStop"},
        {"CustomerChainGroupCSV.tuesdayRoute",            "tuesdayRoute"},
        {"CustomerChainGroupCSV.tuesdayStop",             "tuesdayStop"},
        {"CustomerChainGroupCSV.wednesdayRoute",          "wednesdayRoute"},
        {"CustomerChainGroupCSV.wednesdayStop",           "wednesdayStop"},
        {"CustomerChainGroupCSV.thursdayRoute",           "thursdayRoute"},
        {"CustomerChainGroupCSV.thursdayStop",            "thursdayStop"},
        {"CustomerChainGroupCSV.fridayRoute",             "fridayRoute"},
        {"CustomerChainGroupCSV.fridayStop",              "fridayStop"},
        {"CustomerChainGroupCSV.saturdayRoute",           "saturdayRoute"},
        {"CustomerChainGroupCSV.saturdayStop",            "saturdayStop"},
        {"CustomerChainGroupCSV.sundayRoute",             "sundayRoute"},
        {"CustomerChainGroupCSV.sundayStop",              "sundayStop"},
        {"CustRoutesTimeWindowCSV.timeWindow1Open",       "win1Start"},
        {"CustRoutesTimeWindowCSV.timeWindow1Close",      "win1Stop"},
        {"CustRoutesTimeWindowCSV.timeWindow2Open",       "win2Start"},
        {"CustRoutesTimeWindowCSV.timeWindow2Close",      "win2Stop"},
        {"CustRoutesTimeWindowCSV.openTime",              "open"},
        {"CustRoutesTimeWindowCSV.closeTime",             "close"}};

    const QMap<QString, QString> formatInvoiceCustom_
    {
        {"InvoiceCustomCSV.warehouseNumber", "warehouseNumber"},
        {"InvoiceCustomCSV.customerNumber", "customerNumber"},
        {"InvoiceCustomCSV.salesRep", "salesRep"},
        {"InvoiceCustomCSV.route", "route"},
        {"InvoiceCustomCSV.stopNumber", "stopNumber"},
        {"InvoiceCustomCSV.invoiceNumber", "invoiceNumber"},
        {"InvoiceCustomCSV.invoiceDate", "invoiceDate"},
        {"InvoiceCustomCSV.shipDate", "shipDate"},
        {"InvoiceCustomCSV.orderDate","orderDate"},
        {"InvoiceCustomCSV.orderTime", "orderTime"},
        {"InvoiceCustomCSV.netSales", "netSales"},
        {"InvoiceCustomCSV.productCost", "productCost"},
        {"InvoiceCustomCSV.GPD", "profit"},
        {"InvoiceCustomCSV.profitPercent", "profitPercent"},
        {"InvoiceCustomCSV.caseCube", "caseCube"},
        {"InvoiceCustomCSV.weight", "weight"},
        {"InvoiceCustomCSV.driverNumber", "driverNumber"},
        {"InvoiceCustomCSV.truckNumber","truckNumber"},
        {"InvoiceCustomCSV.casesShipped","casesShipped"},
        {"InvoiceCustomCSV.casesOrdered","casesOrdered"},
        {"InvoiceCustomCSV.credit","credit"}};

    const QMap<QString, QString> formatInvoice_
    {
        {"OrderTrackingCSV.invoiceNumber",                 "invoiceNumber"},
        {"OrderTrackingCSV.orderTime",                "orderTime"},
        {"OrderTrackingCSV.orderDate",                "orderDate"},
        {"OrderTrackingCSV.invoiceDate",               "invoiceDate"},
        {"OrderTrackingCSV.rep",          "salesRep"},
        {"OrderTrackingCSV.routeKey",             "route"},
        {"OrderTrackingCSV.stopNumber",              "stopNumber"},
        {"RouteProfitabilityCSV.customerNumber",              "customerNumber"},
        {"RouteProfitabilityCSV.masterCustomerNumber",        "masterCustomerNumber"},
        {"RouteProfitabilityCSV.pieces",                 "caseCount"},
        {"RouteProfitabilityCSV.weight",        "weight"},
        {"RouteProfitabilityCSV.cube",                    "caseCube"},
        {"RouteProfitabilityCSV.netSales",                 "netSales"},
        {"RouteProfitabilityCSV.cost",             "productCost"},
        {"RouteProfitabilityCSV.profit",                "profit"}};

    QJsonObject mySQLSettings_ =    {{"workingDir",     QJsonValue()},
                                     {"caStr",           QJsonValue()},
                                     {"clientKeyStr",    QJsonValue()},
                                     {"clientCertStr",   QJsonValue()},
                                     {"hostName",        QJsonValue()},
                                     {"databaseName",    QJsonValue()},
                                     {"userName",        QJsonValue()},
                                     {"password",        QJsonValue()}};

private:
    QString settingsDBPAth_ = QCoreApplication::applicationDirPath() + "/mySQLSettings.db";
    Json2Sqlite settings_;

signals:

public slots:
};

#endif // MYSQLEXPORTER_H
