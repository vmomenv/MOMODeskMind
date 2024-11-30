#ifndef PETAI_H
#define PETAI_H

#include <QObject>

class PetAI : public QObject
{
    Q_OBJECT
public:
    explicit PetAI (QObject *parent = nullptr);
    QString getPetGreeting();
    QString getResponse(const QString &question);
signals:
private:
    QString petGreeting;

};

#endif // PETAI_H
