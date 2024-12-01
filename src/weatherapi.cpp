#include "weatherapi.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

WeatherAPI::WeatherAPI(QObject *parent)
    : QObject{parent}, networkManager(new QNetworkAccessManager(this))
{
    apiKey = readApiKeyFromConfig();
    apiUrl = "https://api.weatherapi.com/v1/current.json";
    connect(networkManager, &QNetworkAccessManager::finished, this, &WeatherAPI::parseWeatherResponse);
}

WeatherAPI::~WeatherAPI()
{
    delete networkManager;
}

QString WeatherAPI::readApiKeyFromConfig()
{
    QFile configFile(":/config/weather-config.json");
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open config file:" << configFile.errorString();
        qWarning() << "无法打开天气配置文件";
        return QString();
    }
    QByteArray data = configFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();

    return jsonObj["WEATHER_API_KEY"].toString();  // 获取API Key
}

QString WeatherAPI::buildApiUrl(const QString &location)
{
    return QString("%1?key=%2&q=%3").arg(apiUrl, apiKey, location);
}

void WeatherAPI::parseWeatherResponse(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error occurred:" << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();  // 获取响应数据
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = doc.object();
    if (jsonObj.contains("error")) {
        qWarning() << "Error fetching weather data:" << jsonObj["error"].toObject()["message"].toString();
        return;
    }

    // 解析天气信息
    QString location = jsonObj["location"].toObject()["name"].toString();
    double tempC = jsonObj["current"].toObject()["temp_c"].toDouble();
    QString condition = jsonObj["current"].toObject()["condition"].toObject()["text"].toString();

    qDebug() << "Location:" << location;
    qDebug() << "Temperature:" << tempC << "°C";
    qDebug() << "Condition:" << condition;
}

QString WeatherAPI::getCurrentWeather()
{

    QString location = "auto:ip";
    QNetworkRequest request(QUrl(buildApiUrl(location)));
    networkManager->get(request);

    return "获取天气中...";
}


