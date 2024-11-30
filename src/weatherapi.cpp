#include "weatherapi.h"
#include <QFile>
WeatherAPI::WeatherAPI(QObject *parent)
    : QObject{parent}
{}

QString WeatherAPI::getCurrentWeather()
{
    // 调用天气API获取当前天气
    // 这里模拟返回结果
    return "当前天气：晴，25°C";
}
QString getApiKeyFromConfig()
{
    QFile configFile("config.json");
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open config file.";
        return QString();
    }

    QByteArray data = configFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();

    return jsonObj["WEATHER_API_KEY"].toString();
}
