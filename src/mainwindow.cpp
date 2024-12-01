#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"

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

MainWindow::~MainWindow()
{
    delete ui;
}
