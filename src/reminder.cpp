#include "reminder.h"
#include <QDebug>
#include <QDateTime>
#include <QSqlQueryModel>
#include <QSqlError>
Reminder::Reminder(QObject *parent)
    : QObject(parent),
    checkTimer(new QTimer(this))
{
    // 初始化数据库
    initializeDatabase();

    // 加载数据库中的提醒数据
    loadReminders();
    connect(checkTimer, &QTimer::timeout, this, &Reminder::checkReminders);
    checkTimer->start(1000); // 每秒检查一次
}

Reminder::~Reminder()
{
    // 在析构函数中关闭数据库
    if (db.isOpen()) {
        db.close();
    }
}

void Reminder::initializeDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("reminders.db");

    if (!db.open()) {
        qWarning() << "无法打开数据库：" << db.lastError().text();
        return;
    }

    // 创建提醒表（如果不存在）
    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS reminders (id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT, content TEXT)")) {
        qWarning() << "无法创建表格：" << query.lastError().text();
    }
}

void Reminder::loadReminders()
{
    reminders.clear();

    // 从数据库中加载提醒数据
    QSqlQuery query("SELECT time, content FROM reminders");

    while (query.next()) {
        QString time = query.value(0).toString();
        QString content = query.value(1).toString();
        reminders.append(time + " - " + content);
    }
}

QStringListModel* Reminder::getReminders()
{
    QStringListModel *model = new QStringListModel();
    model->setStringList(reminders);
    return model;
}

void Reminder::addReminder(const QString &reminder)
{
    // 从提醒字符串中提取时间和内容
    QStringList parts = reminder.split(" - ");
    if (parts.size() != 2) return;

    QString time = parts[0];
    QString content = parts[1];

    // 插入提醒到数据库
    QSqlQuery query;
    query.prepare("INSERT INTO reminders (time, content) VALUES (:time, :content)");
    query.bindValue(":time", time);
    query.bindValue(":content", content);

    if (!query.exec()) {
        qWarning() << "插入提醒失败：" << query.lastError().text();
        return;
    }

    // 更新本地提醒列表
    reminders.append(reminder);
}

void Reminder::removeReminder(const QString &reminder)
{
    // 从提醒字符串中提取时间和内容
    QStringList parts = reminder.split(" - ");
    if (parts.size() != 2) return;

    QString time = parts[0];
    QString content = parts[1];

    // 删除数据库中的提醒
    QSqlQuery query;
    query.prepare("DELETE FROM reminders WHERE time = :time AND content = :content");
    query.bindValue(":time", time);
    query.bindValue(":content", content);

    if (!query.exec()) {
        qWarning() << "删除提醒失败：" << query.lastError().text();
        return;
    }

    // 更新本地提醒列表
    reminders.removeOne(reminder);
}

void Reminder::checkReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    QString currentTime = now.toString("hh:mm AP");
    qDebug() << "currentTime" << currentTime;

    for (const QString &reminder : reminders) {
        QString reminderTime = reminder.section(" - ", 0, 0).trimmed();
        QString reminderContent = reminder.section(" - ", 1, 1).trimmed();
        qDebug() << reminderTime;

        if (reminderTime == currentTime) {
            qDebug() << "触发";
            emit reminderTriggered(reminderContent);
        }
    }
}
