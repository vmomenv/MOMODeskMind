#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTimer>
#include <QSqlDatabase>

// ReminderItem 数据结构，用于保存单条提醒的数据
struct ReminderItem {
    int id;          // 唯一标识符
    QString time;    // 提醒时间
    QString content; // 提醒内容
    int priority;    // 提醒优先级：1-红，2-黄，3-绿
};

class Reminder : public QObject
{
    Q_OBJECT
public:
    explicit Reminder(QObject *parent = nullptr);
    ~Reminder();

    // 获取所有提醒列表
    QList<ReminderItem> getReminders() const;

    // 添加提醒
    void addReminder(const QString &time, const QString &content, int priority);

    // 删除提醒
    void removeReminder(int id);

signals:
    // 当触发提醒时发送信号
    void reminderTriggered(const QString &content);

    // 当提醒列表更新时发送信号，通知 UI 更新
    void remindersUpdated();

private slots:
    // 定时检查提醒
    void checkReminders();

private:
    // 初始化数据库
    void initializeDatabase();

    // 加载提醒数据到本地列表
    void loadReminders();

    // 提醒列表（本地缓存）
    QList<ReminderItem> reminders;

    // 定时器，用于定时检查提醒
    QTimer *checkTimer;

    // 数据库对象
    QSqlDatabase db;
};

#endif // REMINDER_H
