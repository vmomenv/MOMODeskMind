#include "settings.h"
#include "ui_settings.h"
#include <QPixmap>
#include <QPainter>
#include <QBitmap>
#include <QFileDialog>
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
Settings::Settings(QWidget *parent)
    :ui(new Ui::Settings)
{
    ui->setupUi(this);
    loadSettings();
    // 连接保存按钮的点击信号到槽函数
    connect(ui->saveButton, &QPushButton::clicked, this, &Settings::saveSettings);
    connect(ui->changeAvatarButton, &QPushButton::clicked, this, &Settings::onChangeAvatarButtonClicked);
    ui->weatherRegionLineEdit->setPlaceholderText("精确到市或县，如太原市则填taiyuan");
    ui->weatherAPIKeyLineEdit->setPlaceholderText("设置密钥，为空则使用默认密钥");
    ui->ollamaAddressLineEdit->setPlaceholderText("为空则默认http://127.0.0.1:11434");
    setWindowTitle("设置");
    // 在初始化时加载头像
    loadAvatar();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::onChangeAvatarButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择头像", "", "图片文件 (*.png *.jpg *.bmp)");
    if (!filePath.isEmpty()) {
        QDir avatarDir(QCoreApplication::applicationDirPath() + "/img/avatars");
        if (!avatarDir.exists()) {
            avatarDir.mkpath("img/avatars");
        }

        QFileInfo fileInfo(filePath);
        QString newAvatarPath = avatarDir.absoluteFilePath(fileInfo.fileName());

        // 如果头像路径不同，则复制文件
        if (filePath != newAvatarPath) {
            QFile::copy(filePath, newAvatarPath);
        }

        QPixmap avatar(newAvatarPath);
        if (!avatar.isNull()) {
            ui->avatarLabel->setPixmap(avatar.scaled(128, 128, Qt::KeepAspectRatio));
        }

        // 保存头像路径到配置文件
        QSettings settings("momodesk-mind", "settings");
        qDebug() << "Settings file location:" << settings.fileName();
        settings.setValue("avatarPath", newAvatarPath);
        emit avatarUpdated();
        loadAvatar();
    }

}

void Settings::loadSettings()
{
    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("无法打开 settings.json 文件");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();
    qDebug()<<json["weather"].toObject()["API_KEY"].toString()<<"1";
    // 填充QLineEdit
    ui->weatherAPIKeyLineEdit->setText(json["weather"].toObject()["API_KEY"].toString());
    ui->weatherRegionLineEdit->setText(json["weather"].toObject()["REGION"].toString());
    ui->ollamaAddressLineEdit->setText(json["language_model"].toObject()["OLLAMA_ADDRESS"].toString());

    file.close();
}

void Settings::saveSettings()
{
    QJsonObject jsons;
    QJsonObject weatherObject;
    QJsonObject languageModelObject;
    QJsonObject avatarObject;

    // 从QLineEdit获取数据
    weatherObject["API_KEY"] = ui->weatherAPIKeyLineEdit->text();
    weatherObject["REGION"] = ui->weatherRegionLineEdit->text();
    languageModelObject["OLLAMA_ADDRESS"] = ui->ollamaAddressLineEdit->text();

    // 构建JSON对象
    jsons["weather"] = weatherObject;
    jsons["language_model"] = languageModelObject;
    jsons["avatar"] = avatarObject;

    // 写入文件
    QFile file("settings.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open settings.json for writing.");
        return;
    }
    qDebug()<<"保存设置";
    file.write(QJsonDocument(jsons).toJson());

    file.close();

}

void Settings::loadAvatar()
{
    // 获取头像路径
    QSettings settings("momodesk-mind", "settings");
    QString avatarPath = settings.value("avatarPath").toString();
    qDebug() << "Avatar path from settings:" << avatarPath;

    QPixmap avatar;
    if (!avatarPath.isEmpty() && QFile::exists(avatarPath)) {
        // 如果头像路径有效，加载头像
        avatar.load(avatarPath);
    } else {
        // 如果没有设置头像或路径无效，则加载默认头像
        qDebug() << "Failed to load avatar. Loading default avatar.";
        avatar.load(":/img/momen.jpg");
    }

    // 将头像裁剪为圆形
    QPixmap circularAvatar = avatar.scaled(128, 128, Qt::KeepAspectRatio); // 调整头像大小
    QBitmap mask(circularAvatar.size());
    mask.fill(Qt::color0);  // 填充为透明
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);  // 用黑色填充圆形区域
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, circularAvatar.width(), circularAvatar.height());
    painter.end();

    // 使用遮罩将头像裁剪成圆形
    circularAvatar.setMask(mask);
    ui->avatarLabel->setPixmap(circularAvatar); // 设置头像
    ui->avatarLabel->move(9,9);  // 设置头像的位置
}

void Settings::checkAndCopySettings()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString settingsPath = appDir + "/settings.json";
    QString defaultSettings = ":/config/settings.json";

    QFile settingsFile(settingsPath);
    if(!settingsFile.exists()){
        qDebug()<<"settings.json不可见，复制默认配置..";
        QFile defaultFile(defaultSettings);
        if (defaultFile.exists()) {
            if (defaultFile.copy(settingsPath)) {
                qDebug() << "默认配置初始化成功";
                if (!QFile::setPermissions(settingsPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser)) {
                    qDebug() << "无法设置文件权限：" << settingsPath;
                } else {
                    qDebug() << "文件权限设置成功：" << settingsPath;
                }
            } else {
                qDebug() << "默认配置复制失败.";
            }
        } else {
            qDebug() << "找不到默认配置";
        }
    }

}

void Settings::on_connectTestButton_clicked()
{
    QString ollamaAddress = ui->ollamaAddressLineEdit->text();

    if (ollamaAddress.isEmpty()) {
        QMessageBox::warning(this, "Error", "OLLAMA_ADDRESS is empty!");
        return;
    }

    // 发送一个简单的 GET 请求测试连接
    QNetworkRequest request;
    request.setUrl(QUrl(ollamaAddress));
    qDebug() << QUrl(ollamaAddress);

    QNetworkAccessManager *m_networkManager = new QNetworkAccessManager(this);
    m_currentReply = m_networkManager->get(request);

    // 连接请求完成的信号到槽函数
    connect(m_currentReply, &QNetworkReply::finished, this, [this]() {
        if (m_currentReply->error() == QNetworkReply::NoError) {
            ui->connectionStatusLabel->setText("连接成功");
        } else {
            ui->connectionStatusLabel->setText("连接错误");
        }
        m_currentReply->deleteLater();
    });
}
QMap<QString, QString> Settings::getWeatherSettings() {
    QMap<QString, QString> weatherSettings;

    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open settings.json.");
        return weatherSettings;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();

    // 获取天气相关的值
    QJsonObject weatherObject = json["weather"].toObject();
    weatherSettings["API_KEY"] = weatherObject["API_KEY"].toString();
    weatherSettings["API_URL"] = weatherObject["API_URL"].toString();
    weatherSettings["REGION"] = weatherObject["REGION"].toString();

    file.close();
    return weatherSettings;
}
QMap<QString, QString> Settings::getLanguageModelSettings() {
    QMap<QString, QString> languageModelSettings;

    QFile file("settings.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open settings.json.");
        return languageModelSettings;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();

    // 获取大语言模型相关的值
    QJsonObject languageModelObject = json["language_model"].toObject();
    languageModelSettings["OLLAMA_ADDRESS"] = languageModelObject["OLLAMA_ADDRESS"].toString();

    file.close();
    return languageModelSettings;
}
