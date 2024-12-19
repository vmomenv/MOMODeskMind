#include "settings.h"
#include "ui_settings.h"
#include <QPixmap>
#include <QPainter>
#include <QBitmap>
#include <QFileDialog>
#include <QSettings>
Settings::Settings(QWidget *parent)
    : QDialog(parent)  // 构造函数改为 QDialog
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->changeAvatarButton, &QPushButton::clicked, this, &Settings::onChangeAvatarButtonClicked);
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
            avatarDir.mkpath(".");
        }

        QFileInfo fileInfo(filePath);
        QString newAvatarPath = avatarDir.absoluteFilePath(fileInfo.fileName());
        if (filePath != newAvatarPath) {
            QFile::copy(filePath, newAvatarPath);
        }
        QPixmap avatar(newAvatarPath);
        if (!avatar.isNull()) {
            QPixmap circularAvatar = avatar.scaled(100, 100, Qt::KeepAspectRatio);
            QBitmap mask(circularAvatar.size());
            mask.fill(Qt::color0);
            QPainter painter(&mask);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::color1);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(0, 0, circularAvatar.width(), circularAvatar.height());
            painter.end();

            circularAvatar.setMask(mask);
            ui->avatarLabel->setPixmap(circularAvatar);
        }

        // 保存头像路径到配置文件
        QSettings settings("momodesk-mind", "settings");
        qDebug() << "Settings file location:" << settings.fileName();
        settings.setValue("avatarPath", newAvatarPath);
    }
}
