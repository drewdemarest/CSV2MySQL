#-------------------------------------------------
#
# Project created by QtCreator 2018-03-14T11:43:23
#
#-------------------------------------------------

QT       += core gui sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CSV2MySQL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    entity/order/order.cpp \
    exporter/mysqlexporter.cpp \
    importer/charliescsvimporter.cpp \
    importer/csvimporter.cpp \
    json2sqlite/json2sqlite.cpp \
    main.cpp \
    mainwindow.cpp \
    entity/order/orderfactory.cpp \
    cachedb/ordersqlite.cpp \
    entity/customer/customer.cpp \
    cachedb/sqlitedbthread.cpp

HEADERS += \
    entity/order/order.h \
    exporter/mysqlexporter.h \
    importer/charliescsvimporter.h \
    importer/csvimporter.h \
    json2sqlite/json2sqlite.h \
    mainwindow.h \
    entity/order/orderfactory.h \
    cachedb/ordersqlite.h \
    entity/customer/customer.h \
    cachedb/sqlitedbthread.h \
    datainfo/datainfo.hpp

FORMS += \
        mainwindow.ui
