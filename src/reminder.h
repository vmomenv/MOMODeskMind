#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QStringListModel>
#include <QTimer>
class Reminder : public QObject
{
    Q_OBJECT

public:
    explicit Reminder(QObject *parent = nullptr);
    QStringListModel* getReminders();
    void addReminder(const QString &time,const QString &content);
    void removeReminder(int index);
signals:
    void reminderTriggered(const QString &content);
private slots:
    // 检查提醒是否到期
    void checkReminders();

private:
    QStringList reminders;
    QStringListModel *reminderModel;
    QTimer *checkTimer;
};

#endif // REMINDER_H
