#include "reminder.h"
#include <QDebug>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

Reminder::Reminder(QObject *parent)
    : QObject(parent),
    checkTimer(new QTimer(this))
{
    // 初始化数据库
    initializeDatabase();

    // 加载数据库中的提醒数据
    loadReminders();

    // 定时器检查提醒
    connect(checkTimer, &QTimer::timeout, this, &Reminder::checkReminders);
    checkTimer->start(60000); // 每分钟检查一次
}

Reminder::~Reminder()
{
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
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS reminders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            time TEXT,
            content TEXT,
            priority INTEGER
        )
    )")) {
        qWarning() << "无法创建表格：" << query.lastError().text();
    }
}

void Reminder::loadReminders()
{
    reminders.clear();

    // 从数据库中加载提醒数据
    QSqlQuery query("SELECT time, content, priority FROM reminders");

    while (query.next()) {
        ReminderItem item;
        item.time = query.value(0).toString();
        item.content = query.value(1).toString();
        item.priority = query.value(2).toInt();
        reminders.append(item);
    }
}

QList<ReminderItem> Reminder::getReminders() const
{
    return reminders;
}

void Reminder::addReminder(const QString &time, const QString &content, int priority)
{
    // 插入提醒到数据库
    QSqlQuery query;
    query.prepare("INSERT INTO reminders (time, content, priority) VALUES (:time, :content, :priority)");
    query.bindValue(":time", time);
    query.bindValue(":content", content);
    query.bindValue(":priority", priority);

    if (!query.exec()) {
        qWarning() << "插入提醒失败：" << query.lastError().text();
        return;
    }

    // 更新本地提醒列表
    ReminderItem item;
    item.time = time;
    item.content = content;
    item.priority = priority;
    reminders.append(item);

    emit remindersUpdated(); // 通知 UI 更新
}

void Reminder::removeReminder(int id)
{
    // 删除数据库中的提醒
    QSqlQuery query;
    query.prepare("DELETE FROM reminders WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "删除提醒失败：" << query.lastError().text();
        return;
    }

    // 更新本地提醒列表
    auto it = std::find_if(reminders.begin(), reminders.end(), [id](const ReminderItem &item) {
        return item.id == id;
    });
    if (it != reminders.end()) {
        reminders.erase(it);
    }

    emit remindersUpdated(); // 通知 UI 更新
}

void Reminder::checkReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    QString currentTime = now.toString("hh:mm AP");

    for (const ReminderItem &item : reminders) {
        if (item.time == currentTime) {
            emit reminderTriggered(item.content);
        }
    }
}
