#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "ui_settings.h"

class Settings : public QDialog  // 继承自 QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void onChangeAvatarButtonClicked();  // 修改头像按钮点击槽函数

private:
    Ui::Settings *ui;  // UI 对象
};

#endif // SETTINGS_H
