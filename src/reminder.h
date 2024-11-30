#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QStringListModel>

class Reminder : public QObject
{
    Q_OBJECT

public:
    explicit Reminder(QObject *parent = nullptr);
    QStringListModel* getReminders();

private:
    QStringList reminders;
};

#endif // REMINDER_H
