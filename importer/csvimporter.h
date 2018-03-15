#ifndef CSVIMPORTER_H
#define CSVIMPORTER_H

#include <QObject>
#include <QtCore>
#include <QDebug>
#include <QRegularExpression>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>


class CSVImporter : public QObject
{
    Q_OBJECT
public:
    explicit CSVImporter(QObject *parent = nullptr);
    void setCSVPath(const QString &csvPath);
    QString getCSVPath();

    static QList<QStringList> importAsQStringList(const QString &filePath);
    static QList<QVariantMap> importAsQVariantMap(const QMap<QString, int> &fileMap, const QString &filePath);
    static QJsonArray importAsQJsonArray(const QMap<QString, int> &fileMap, const QString &filePath);

protected:
    QString csvPath_;

private:
    static QStringList parseCSVLine(const QByteArray &csvLine);
    static QVariantMap csvStringListToCSVVariantMap(const QStringList &csvStringList, const QMap<QString, int> &columnMap);
    static QJsonObject csvStringListToQJsonArray(const QStringList &csvStringList, const QMap<QString, int> &columnMap);

signals:
    void percentComplete(int progress);

public slots:
};

#endif // CSVIMPORTER_H
