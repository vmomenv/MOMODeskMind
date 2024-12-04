#include "settings.h"
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineView>

Settings::Settings(QWidget *parent) : QWidget(parent)
{
    // 创建QWebEngineView实例
    webView = new QWebEngineView(this);
    webProfile = QWebEngineProfile::defaultProfile();  // 获取默认的Web配置

    // 设置页面加载和显示
    loadSettingsPage();

    // 使用垂直布局将Web视图放入窗口
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(webView);
    setLayout(layout);
}

Settings::~Settings()
{
    delete webView;
}

// 加载HTML设置页面
void Settings::loadSettingsPage()
{
    // HTML字符串可以通过QUrl加载本地HTML文件，或者通过直接加载URL来显示远程HTML
    webView->setUrl(QUrl("qrc:/html/settings.html"));  // 你可以根据需要调整文件路径
}
