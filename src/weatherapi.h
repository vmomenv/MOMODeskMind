#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QNetworkAccessManager>

class WeatherAPI : public QObject
{
    Q_OBJECT
public:
    explicit WeatherAPI(QObject *parent = nullptr);
    QString getCurrentWeather();  // 获取当前天气
    QString getWeatherForecast(); // 获取未来几天的天气预报
private:
    QString apiKey;   // 存储API Key
    QString apiUrl;   // 存储API URL
    QNetworkAccessManager *networkManager;  // 用于发起网络请求

    QString readApiKeyFromConfig();  // 从配置文件读取API Key
    QString buildApiUrl(const QString &location);  // 构建API请求URL
    void parseWeatherResponse(const QByteArray &data); // 解析天气数据

signals:

};

#endif // WEATHERAPI_H
