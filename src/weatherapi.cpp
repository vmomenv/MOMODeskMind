#include "weatherapi.h"

WeatherAPI::WeatherAPI(QObject *parent)
    : QObject{parent}
{}

QString WeatherAPI::getCurrentWeather()
{
    // 调用天气API获取当前天气
    // 这里模拟返回结果
    return "当前天气：晴，25°C";
}
