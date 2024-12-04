#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEngineProfile>

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    QWebEngineView *webView;  // 用于显示HTML页面的视图
    QWebEngineProfile *webProfile;  // 用于加载HTML的Web配置

    void loadSettingsPage();  // 加载设置页面HTML
};

#endif // SETTINGS_H
