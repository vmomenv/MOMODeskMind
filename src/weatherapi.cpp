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
    readConfig();
    connect(networkManager, &QNetworkAccessManager::finished, this, &WeatherAPI::parseWeatherResponse);
}

WeatherAPI::~WeatherAPI()
{
    delete networkManager;
}


void WeatherAPI::readConfig()
{
    // 打开同目录下的 settings.json 文件
    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开 settings.json 文件";
    }

    // 读取文件内容
    QByteArray jsonData = file.readAll();
    file.close();

    // 解析 JSON 数据
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull()) {
        qWarning() << "解析 JSON 数据失败";
    }

    // 获取根对象
    QJsonObject rootObject = jsonDoc.object();

    // 获取 "weather" 对象
    QJsonObject weatherObject = rootObject.value("weather").toObject();
    if (weatherObject.isEmpty()) {
        qWarning() << "未找到 weather 对象";
    }

    // 获取 "API_KEY" 值
    apiKey = weatherObject.value("API_KEY").toString();
    if (apiKey.isEmpty()) {
        qWarning() << "未找到 API_KEY";
    }
    region = weatherObject.value("REGION").toString();
    if (region.isEmpty()) {
        qWarning() << "未找到 region";
    }
}


QString WeatherAPI::buildApiUrl(const QString &location)
{
    return QString("https://api.weatherapi.com/v1/current.json?key=%1&q=%2").arg(apiKey, location);
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
    QString location;
    if(region.isEmpty()){
        location = "auto:ip";
    }else{
        location = region;
    }


    QNetworkRequest request(QUrl(buildApiUrl(location)));
    networkManager->get(request);
    qDebug()<<QUrl(buildApiUrl(location));
    return "获取天气中...";
}

