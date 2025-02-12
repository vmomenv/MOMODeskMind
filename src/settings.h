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

    void loadAvatar();
    void checkAndCopySettings(); //检测同级目录是否存在settings.json
private slots:
    void onChangeAvatarButtonClicked();
signals:
    void avatarUpdated();


private:
    Ui::Settings *ui;  // UI 对象
};

#endif // SETTINGS_H
