#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>

class weatherapi : public QObject
{
    Q_OBJECT
public:
    explicit weatherapi(QObject *parent = nullptr);

signals:
};

#endif // WEATHERAPI_H
