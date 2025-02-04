#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QCache>
#include <QJsonArray>
class AIClient : public QObject
{
    Q_OBJECT
public:
    explicit AIClient(QObject *parent = nullptr);

    // 核心功能接口
    void listModels();  // 获取可用模型列表
    void generateResponse(const QString &model, const QString &prompt);  // 生成AI响应
    void cancelRequest();  // 取消当前请求

    // 配置接口
    void setServerUrl(const QString &url);  // 设置服务器地址
    QString serverUrl() const { return m_baseUrl; }

    // 上下文管理
    void clearContext();  // 清空对话上下文

signals:
    // 状态通知信号
    void modelsReceived(const QStringList &models);  // 收到模型列表
    void responseReceived(const QString &response);  // 收到流式响应片段
    void responseComplete();  // 响应完成
    void errorOccurred(const QString &error);  // 发生错误
    void responseInterrupted();
private slots:
    // 网络响应处理
    void handleModelsResponse();
    void handleGenerateResponse();
    void handleStreamData();

private:
    // 网络管理
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply = nullptr;

    // 配置参数
    QString m_baseUrl = "http://localhost:11434";
    QString m_buffer;  // 流式数据缓冲区

    // 上下文管理
    QJsonArray m_context;  // 对话上下文

    // 超时检测
    QTimer *m_timeoutTimer;
};
