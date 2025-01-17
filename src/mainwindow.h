#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QMouseEvent>
#include "petai.h"
#include "weatherapi.h"
#include "MessageWidget.h"
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
    void loadAvatar();
    QString avatarFilePath;
    QPoint offset;  // 鼠标按下时的偏移量，用于拖动窗口
    QVBoxLayout *reminderLayout;
    QWidget *reminderWidget;
    void reminderLoadJsonData(const QString &filePath);
    void displayMessage(const QString &message, const QString &priority);
protected:
    // 鼠标事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void onAskButtonClicked();
    void updateWeatherDisplay(const QString &location, double tempC, const QString &condition);
    void onAddReminderButtonClicked();
    void onRemoveReminderButtonClicked();
    void onReminderTriggered(const QString &content);
    void openSettingsDialog();
    // void onChangeAvatarButtonClicked();

    // void onMessageWidgetDeleted(MessageWidget *widget);
    // void onNewMessageAdded(const QString &message, const QString &priority);

private:

};
#endif // MAINWINDOW_H
