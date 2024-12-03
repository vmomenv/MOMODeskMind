#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建天气API对象
    weatherAPI = new WeatherAPI(this);
    // 连接天气API的信号到updateWeatherDisplay槽函数
    connect(weatherAPI, &WeatherAPI::weatherDataUpdated, this, &MainWindow::updateWeatherDisplay);

    // 创建宠物AI对象
    petAI = new PetAI(this);
    ui->petDisplay->setText(petAI->getPetGreeting());

    // 初始化天气API并请求天气数据
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());

    // 初始化日程提醒
    reminder = new Reminder(this);
    ui->reminderList->setModel(reminder->getReminders());
    connect(ui->addReminderButton, &QPushButton::clicked, this, &MainWindow::onAddReminderButtonClicked);
    connect(ui->removeReminderButton, &QPushButton::clicked, this, &MainWindow::onRemoveReminderButtonClicked);
    connect(reminder, &Reminder::reminderTriggered, this, &MainWindow::onReminderTriggered);

    connect(ui->askButton, &QPushButton::clicked, this, &MainWindow::onAskButtonClicked);
}

void MainWindow::onAskButtonClicked(){
    QString userQuestion = ui->questionInput->text();
    QString response = petAI->getResponse(userQuestion);
    ui->answerLabel->setText(response);
}

void MainWindow::updateWeatherDisplay(const QString &location, double tempC, const QString &condition)
{
    // 更新 QLabel 显示天气信息
    QString weatherText = QString("Location: %1\nTemperature: %2 °C\nCondition: %3")
                              .arg(location)
                              .arg(tempC)
                              .arg(condition);
    ui->weatherLabel->setText(weatherText);
}
void MainWindow::onAddReminderButtonClicked()
{
    QString reminderTime = ui->reminderTimeInput->text();
    QString reminderContent = ui->reminderContentInput->text();
    QString reminderText = reminderTime + " - " + reminderContent;

    // 调用 Reminder 类的添加方法
    reminder->addReminder(reminderText);  // 这里直接使用 reminder 对象

    // 更新 UI
    ui->reminderList->setModel(reminder->getReminders());
}

void MainWindow::onRemoveReminderButtonClicked()
{
    // 获取选中的提醒项
    QModelIndex selectedIndex = ui->reminderList->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
        return;  // 如果没有选中任何项，则返回
    }

    QString reminderText = selectedIndex.data(Qt::DisplayRole).toString();

    // 调用 Reminder 类的删除方法
    reminder->removeReminder(reminderText);

    // 更新 UI
    ui->reminderList->setModel(reminder->getReminders());
}

void MainWindow::onReminderTriggered(const QString &content)
{
    QMessageBox::information(this, "提醒", content);
}
MainWindow::~MainWindow()
{
    delete ui;
}
