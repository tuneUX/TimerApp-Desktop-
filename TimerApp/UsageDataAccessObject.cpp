#include "UsageDataAccessObject.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

void UsageDataAccessObject::createTable(QSqlDatabase& db) {
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS usage ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "app TEXT NOT NULL,"
        "overuse_min INTEGER NOT NULL,"
        "date TEXT NOT NULL)");
}

void UsageDataAccessObject::insert(QSqlDatabase& db, const QString& app, int overuse_min, const QString& date) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO usage(app, overuse_min, date) VALUES(:app, :overuse_min, :date)");
    query.bindValue(":app", app);
    query.bindValue(":overuse_min", overuse_min);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "insert error:" << query.lastError().text();
    }
}

QList<QVariantMap> UsageDataAccessObject::getAll(QSqlDatabase& db) {
    QList<QVariantMap> results;
    QSqlQuery query(db);

    if (query.exec("SELECT id, app, overuse_min, date FROM usage")) {
        while (query.next()) {
            QVariantMap row;
            row["id"] = query.value("id");
            row["app"] = query.value("app");
            row["overuse_min"] = query.value("overuse_min");
            row["date"] = query.value("date");
            results.append(row);
        }
    }
    else {
        qDebug() << "select error:" << query.lastError().text();
    }

    return results;
}
