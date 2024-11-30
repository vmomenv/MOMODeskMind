#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>

class reminder : public QObject
{
    Q_OBJECT
public:
    explicit reminder(QObject *parent = nullptr);

signals:
};

#endif // REMINDER_H
