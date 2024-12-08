#include "settings.h"
#include "ui_settings.h"
#include <QPixmap>
#include <QPainter>
#include <QBitmap>
#include <QFileDialog>

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
        QPixmap avatar(filePath);
        if (!avatar.isNull()) {
            QPixmap circularAvatar = avatar.scaled(100, 100, Qt::KeepAspectRatio);  // 调整头像大小
            QBitmap mask(circularAvatar.size());
            mask.fill(Qt::color0);  // 填充为透明
            QPainter painter(&mask);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::color1);  // 用黑色填充圆形区域
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(0, 0, circularAvatar.width(), circularAvatar.height());
            painter.end();

            circularAvatar.setMask(mask);
            ui->avatarLabel->setPixmap(circularAvatar);  // 设置头像
        }
    }
}
