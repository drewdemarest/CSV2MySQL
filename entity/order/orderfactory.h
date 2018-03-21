#ifndef ORDERFACTORY_H
#define ORDERFACTORY_H

#include <QObject>

class OrderFactory : public QObject
{
    Q_OBJECT
public:
    explicit OrderFactory(QObject *parent = nullptr);

signals:

public slots:
};

#endif // ORDERFACTORY_H