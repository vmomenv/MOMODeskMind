#include "aiclient.h"

AIClient::AIClient(QObject *parent)
    : QObject{parent},
    m_networkManager(new QNetworkAccessManager(this))
{


}
bool AIClient::initializeConnection()
{
    // 创建一个网络请求
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl));

    // 发送一个简单的GET请求来测试连接
    m_currentReply = m_networkManager->get(request);

    // 检查响应状态
    if (m_currentReply->error() == QNetworkReply::NoError) {
        // 连接成功
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return true;
    } else {
        // 连接失败
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return false;
    }
}
void AIClient::listModels(){
    if(m_currentReply){
        m_currentReply->abort();
    }
    QUrl url(m_baseUrl + "/api/tags"); // 设置请求的 URL 为 /api/tags
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //发送请求并保存QNetworkReply对象
    m_currentReply = m_networkManager->get(request);

    connect(m_currentReply, &QNetworkReply::finished, this, &AIClient::handleModelsResponse); // 连接请求完成信号到 handleModelsResponse 方法
}
//请求响应
void AIClient::generateResponse(const QString &model,const QString &prompt){
    // cancelRequest();

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
    // connect(m_currentReply, &QNetworkReply::finished,this, &AIClient::handleGenerateResponse);

    // 启动超时检测
    // m_timeoutTimer->start();

}
void AIClient::cancelRequest()
{
    if (m_currentReply->error() == QNetworkReply::NoError) {
            // 中止请求
            m_currentReply->abort();
            m_currentReply = nullptr;

            // 清空缓冲区
            m_buffer.clear();

            // 发送中断信号（需要在头文件中添加信号声明）
            emit responseInterrupted();
    }
    // if (m_currentReply) {


    //     // 中止请求
    //     m_currentReply->abort();
    //     m_currentReply = nullptr;

    //     // 清空缓冲区
    //     m_buffer.clear();

    //     // 发送中断信号（需要在头文件中添加信号声明）
    //     emit responseInterrupted();
    // }
}
void AIClient::setServerUrl(const QString &url)
{
    m_baseUrl = url;
    if (m_baseUrl.endsWith("/")) {
        m_baseUrl.chop(1);
    }
}

void AIClient::clearContext()
{
    m_context = QJsonArray();
}
//模型响应
void AIClient::handleModelsResponse()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender()); // 获取发送信号的 QNetworkReply 对象
    if (!reply) return;

    const QByteArray data = reply->readAll(); // 读取所有响应数据
    if (reply->error() != QNetworkReply::NoError) { // 检查是否有错误
        emit errorOccurred(reply->errorString()); // 发送错误信号
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError); // 解析 JSON 数据
    if (parseError.error != QJsonParseError::NoError) { // 检查解析是否有错误
        emit errorOccurred("JSON解析错误: " + parseError.errorString()); // 发送解析错误信号
        reply->deleteLater();
        return;
    }

    QJsonArray models = doc.object()["models"].toArray(); // 获取 "models" 数组
    QStringList modelList;
    for (const auto &modelValue : models) { // 遍历数组中的每个模型
        QJsonObject modelObj = modelValue.toObject();
        modelList << modelObj["name"].toString(); // 将模型名称添加到列表中
    }

    emit modelsReceived(modelList); // 发送模型接收信号
    reply->deleteLater(); // 删除 reply 对象
}

//数据流数据
void AIClient::handleStreamData()
{
    while (m_currentReply->bytesAvailable()) {
        m_buffer += m_currentReply->readLine();

        // 检查是否收到完整JSON对象
        while (m_buffer.contains('\n')) {
            int newlinePos = m_buffer.indexOf('\n');
            QString chunk = m_buffer.left(newlinePos).trimmed();
            m_buffer = m_buffer.mid(newlinePos + 1);

            if (!chunk.isEmpty()) {
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(chunk.toUtf8(), &parseError);
                if (parseError.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("response")) {
                        emit responseReceived(obj["response"].toString());
                    }
                    if (obj.contains("context")) {
                        m_context = obj["context"].toArray(); // 更新上下文
                    }
                } else {
                    emit errorOccurred("流数据解析错误: " + parseError.errorString());
                }
            }
        }
    }
}
void AIClient::handleGenerateResponse()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError &&
        reply->error() != QNetworkReply::OperationCanceledError) {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
    m_currentReply = nullptr;
    m_buffer.clear();
    // m_timeoutTimer->stop();
    emit responseComplete();
}
