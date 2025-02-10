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
    qDebug()<<"apikey"<<apiKey;
    connect(networkManager, &QNetworkAccessManager::finished, this, &WeatherAPI::parseWeatherResponse);
}

WeatherAPI::~WeatherAPI()
{
    delete networkManager;
}

QString WeatherAPI::readApiKeyFromConfig()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString settingsPath = appDir + "/settings.json";
    QFile configFile(settingsPath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning() << "打开配置失败" << configFile.errorString();
        qWarning() << "无法打开天气配置文件";
        return QString();
    }
    QByteArray data = configFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();


    return QString();
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

    emit weatherDataUpdated(location, tempC, condition);

}

QString WeatherAPI::getCurrentWeather()
{

    QString location = "auto:ip";

    QNetworkRequest request(QUrl(buildApiUrl(location)));
    networkManager->get(request);
    qDebug()<<QUrl(buildApiUrl(location));
    return "获取天气中...";
}

