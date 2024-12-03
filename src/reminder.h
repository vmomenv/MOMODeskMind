#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QStringListModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>

class Reminder : public QObject
{
    Q_OBJECT
public:
    explicit Reminder(QObject *parent = nullptr);
    ~Reminder();

    QStringListModel* getReminders();
    void addReminder(const QString &reminder);
    void removeReminder(const QString &reminder);

signals:
    void reminderTriggered(const QString &content);

private:
    void initializeDatabase();  // 初始化数据库
    void loadReminders();  // 从数据库加载提醒数据
    void saveReminderToDatabase(const QString &reminder);  // 保存提醒到数据库

    QStringList reminders;  // 用于保存提醒的列表
    QSqlDatabase db;  // SQLite数据库对象
    QTimer *checkTimer;
private slots:
    void checkReminders();
};

#endif // REMINDER_H
