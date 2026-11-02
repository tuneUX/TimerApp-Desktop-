#pragma once
#include <QSqlDatabase>
#include <QList>
#include <QVariantMap>

class UsageDataAccessObject {
public:
    static void createTable(QSqlDatabase& db);
    static void insert(QSqlDatabase& db, const QString& app, int overuse_min, const QString& date);
    static QList<QVariantMap> getAll(QSqlDatabase& db);
};
