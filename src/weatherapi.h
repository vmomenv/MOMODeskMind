#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>

class WeatherAPI : public QObject
{
    Q_OBJECT
public:
    explicit WeatherAPI(QObject *parent = nullptr);
    QString getCurrentWeather();
signals:
};

#endif // WEATHERAPI_H
