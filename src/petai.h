#ifndef PETAI_H
#define PETAI_H

#include <QObject>

class petai : public QObject
{
    Q_OBJECT
public:
    explicit petai(QObject *parent = nullptr);

signals:
};

#endif // PETAI_H
