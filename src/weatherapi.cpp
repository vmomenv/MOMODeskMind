#include "weatherapi.h"
#include <QFile>
WeatherAPI::WeatherAPI(QObject *parent)
    : QObject{parent},networkManager(new QNetworkAccessManager(this)){
    // 从配置文件读取API Key和URL
    apiKey = readApiKeyFromConfig();
    apiUrl = "https://api.weatherapi.com/v1/current.json";
}


QString WeatherAPI::getCurrentWeather()
{
    // 调用天气API获取当前天气
    // 这里模拟返回结果
    return "当前天气：晴，25°C";
}

QString WeatherAPI::readApiKeyFromConfig()
{
    QFile configFile("weather-config.json");
    if(!configFile.open(QIODevice::ReadOnly)){
        qWarning()<<"无法打开天气配置文件";
        return QString();
    }
    QByteArray data=configFile.readAll();
    // qDebug()<<data;
}
