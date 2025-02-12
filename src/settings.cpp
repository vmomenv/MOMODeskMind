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

Settings::Settings(QWidget *parent)
    :ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->changeAvatarButton, &QPushButton::clicked, this, &Settings::onChangeAvatarButtonClicked);
    ui->RegionLineEdit->setPlaceholderText("精确到市或县，如太原市则填taiyuan");
    ui->keyLineEdit->setPlaceholderText("设置密钥，为空则使用默认密钥");
    ui->serverAddressLineEdit->setPlaceholderText("为空则默认http://127.0.0.1:11434");
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
    ui->avatarLabel->move(128, 16);  // 设置头像的位置
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
            } else {
                qDebug() << "默认配置复制失败.";
            }
        } else {
            qDebug() << "找不到默认配置";
        }
    }

}
