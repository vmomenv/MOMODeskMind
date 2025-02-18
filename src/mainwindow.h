#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QScrollBar>
#include<QClipboard>
#include "petai.h"
#include "weatherapi.h"
#include "MessageWidget.h"
#include "aiclient.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PetAI *petAI;
    WeatherAPI *weatherAPI;
    Settings *settings;
    void loadAvatar();
    QString avatarFilePath;
    QPoint offset;  // 鼠标按下时的偏移量，用于拖动窗口
    AIClient *aiClient;
    QWidget *reminderWidget;
    QVBoxLayout *reminderWidgetLayout;
    QScrollBar *answerScrollbar;
    //AI对话dialogueWidget框大小状态标识
    bool m_isExpanded = false;  // 添加状态标识
    QSize m_originalSize;       // 保存原始尺寸
    QClipboard *clipboard;               // 剪切板对象指针
    QLabel *a;
    bool isPasteMonitoring = false;           // 监控剪切板按钮状态
    bool isexplainCodeMonitoring = false;            //监控解释代码按钮状态
    bool isTranslateMonitoring = false;       //监控翻译代码按钮状态
    bool isTopping =false;

    void reminderLoadJsonData(const QString &filePath);
    void displayMessage(const QString &message, const QString &time,const QString &priority);
    void saveReminderToJson(const QString &message, const QString &time, const QString &priority);
    void removeReminderFromJson(const QString &message, const QString &time, const QString &priority);
    void updateReminderCount();
    void adjustScrollContent();

    void setExpandDialogueWidget();
    void setCollapseDialogueWidget();
    void moveToBottomRight();
protected:
    // 鼠标事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void updateWeatherDisplay(const QString &location, double tempC, const QString &condition);
    void onReminderTriggered(const QString &content);
    void openSettingsDialog();

    void handleModelsReceived(const QStringList& models);
    void handleResponseReceived(const QString& response);
    void handleErrorOccurred(const QString& error);
    void handleResponseComplete();
    void sendRequest();
    // void onChangeAvatarButtonClicked();

    // void onMessageWidgetDeleted(MessageWidget *widget);
    // void onNewMessageAdded(const QString &message, const QString &priority);
    bool eventFilter(QObject* watched, QEvent* event) override;
    void on_addReminderButton_clicked();

    void on_sendButton_clicked();
    void on_clearButton_clicked();
    void on_networkSearchButton_clicked();
    void on_pasteButton_clicked();
    void on_explainCodeButton_clicked();
    void on_translateButton_clicked();
    void onSettingsUpdated();

    void handleClipboardChange();        // 自定义剪切板变化处理
    void on_pinnedButton_clicked();
};
#endif // MAINWINDOW_H
