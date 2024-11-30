#include "reminder.h"

Reminder::Reminder(QObject *parent)
    : QObject(parent)
{
    reminders << "9:00 AM - 会议"
              << "12:00 PM - 午餐"
              << "3:00 PM - 完成项目报告";
}

QStringListModel* Reminder::getReminders()
{
    QStringListModel* model = new QStringListModel();
    model->setStringList(reminders);
    return model;
}
