#include "aiclient.h"

AIClient::AIClient(QObject *parent)
    : QObject{parent},
    m_networkManager(new QNetworkAccessManager(this)),
    m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setInterval(30000);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_currentReply) {
            cancelRequest();
            emit errorOccurred(tr("请求超时，请检查网络连接"));
        }
    });
}
void AIClient::listModels(){
    if(m_currentReply){
        m_currentReply->abort();
    }
    QUrl url(m_baseUrl + "/api/tags");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //发送请求并保存QNetworkReply对象
    m_currentReply=m_networkManager->get(request);

    connect(m_currentReply, &QNetworkReply::finished, this, &AIClient::handleModelsResponse);

}
//请求响应
void AIClient::generateResponse(const QString &model,const QString &prompt){
    cancelRequest();

    QUrl url(m_baseUrl + "/api/generate");
    QJsonObject body;
    body["model"] = model;
    body["prompt"] = prompt;
    body["stream"] = true;

    if(!m_context.isEmpty()){
        body["context"]=m_context;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_currentReply = m_networkManager->post(request, QJsonDocument(body).toJson());
    connect(m_currentReply, &QNetworkReply::readyRead,this, &AIClient::handleStreamData);
    connect(m_currentReply, &QNetworkReply::finished,this, &AIClient::handleGenerateResponse);

    // 启动超时检测
    m_timeoutTimer->start();

}
void AIClient::cancelRequest()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    m_buffer.clear();
    m_timeoutTimer->stop();
}
