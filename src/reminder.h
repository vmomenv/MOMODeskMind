#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>

class Reminder : public QObject
{
    Q_OBJECT
public:
    explicit Reminder(QObject *parent = nullptr);

signals:
};

#endif // REMINDER_H
