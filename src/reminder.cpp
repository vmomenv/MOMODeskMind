#include "reminder.h"
#include <QDateTime>
#include <QDebug>

Reminder::Reminder(QObject *parent)
    : QObject(parent),
    reminderModel(new QStringListModel(this)),
    checkTimer(new QTimer(this))
{
    // 初始化默认提醒
    reminders << "9:00 AM - 会议"
              << "12:00 PM - 午餐"
              << "3:00 PM - 完成项目报告";
    reminderModel->setStringList(reminders);

    // 设置定时器
    connect(checkTimer, &QTimer::timeout, this, &Reminder::checkReminders);
    checkTimer->start(1000); // 每秒检查一次
}

QStringListModel* Reminder::getReminders()
{
    return reminderModel;
}

void Reminder::addReminder(const QString &time, const QString &content)
{
    QString reminder = QString("%1 - %2").arg(time, content);
    reminders.append(reminder);
    reminderModel->setStringList(reminders);
}

void Reminder::removeReminder(int index)
{
    if (index >= 0 && index < reminders.size()) {
        reminders.removeAt(index);
        reminderModel->setStringList(reminders);
    } else {
        qWarning() << "Invalid index:" << index;
    }
}

void Reminder::checkReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    QString currentTime = now.toString("hh:mm AP");  // 保持12小时制，前导零
    qDebug() << "currentTime" << currentTime;

    for (const QString &reminder : reminders) {
        QString reminderTime = reminder.section(" - ", 0, 0).trimmed();  // 提取提醒时间
        QString reminderContent = reminder.section(" - ", 1, 1).trimmed(); // 提取提醒内容
        qDebug() << reminderTime;

        if (reminderTime == currentTime) {  // 使用严格匹配
            qDebug() << "触发";
            emit reminderTriggered(reminderContent);  // 触发提醒
        }
    }
}
