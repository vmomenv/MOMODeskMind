#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include "settings.h"
#include <QSettings>
#include <QFile>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("background-color: #F3F4F6;");
    // 创建天气API对象
    weatherAPI = new WeatherAPI(this);

    loadAvatar();
    // connect(ui->changeAvatarButton, &QPushButton::clicked, this, &MainWindow::onChangeAvatarButtonClicked);

    // 连接天气API的信号到updateWeatherDisplay槽函数
    connect(weatherAPI, &WeatherAPI::weatherDataUpdated, this, &MainWindow::updateWeatherDisplay);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettingsDialog);

    // 创建宠物AI对象
    petAI = new PetAI(this);
    ui->answerLabel->setText(petAI->getPetGreeting());

    // 初始化天气API并请求天气数据
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());

    // 初始化日程提醒
    reminder = new Reminder(this);
    // ui->reminderList->setModel(reminder->getReminders());
    connect(ui->addReminderButton, &QPushButton::clicked, this, &MainWindow::onAddReminderButtonClicked);

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
    // QString reminderText = reminderTime + " - " + reminderContent;

    // 调用 Reminder 类的添加方法
    // reminder->addReminder(reminderText);  // 这里直接使用 reminder 对象

    // 更新 UI
    // ui->reminderList->setModel(reminder->getReminders());
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
    // reminder->removeReminder(reminderText);

    // 更新 UI
    // ui->reminderList->setModel(reminder->getReminders());
}

void MainWindow::onReminderTriggered(const QString &content)
{
    QMessageBox::information(this, "提醒", content);
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 获取鼠标按下的位置
        offset = event->globalPos() - frameGeometry().topLeft();
    }
}

// 处理鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // 根据鼠标的移动更新窗口的位置
        move(event->globalPos() - offset);
    }
}
void MainWindow::loadAvatar(){
    QSettings settings;
    QString avatarPath = settings.value("avatarPath").toString();
    qDebug()<<avatarPath;
    QPixmap avatar;
    if (!avatarPath.isEmpty() && QFile::exists(avatarPath)) {
        // 如果头像文件存在，加载头像

        avatar.load(avatarPath);
    } else {
        // 如果没有设置头像，则使用默认头像
        qDebug()<<"未加载";
        avatar.load(":/img/momen.jpg");
    }

    QPixmap circularAvatar = avatar.scaled(128, 128, Qt::KeepAspectRatio); // 调整头像大小
    QBitmap mask(circularAvatar.size());
    mask.fill(Qt::color0);  // 填充为透明
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::color1);  // 用黑色填充圆形区域
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, circularAvatar.width(), circularAvatar.height());
    painter.end();
    // 使用遮罩将头像裁剪成圆形
    circularAvatar.setMask(mask);
    ui->avatarLabel->setPixmap(circularAvatar); // 设置头像
    ui->avatarLabel->move(128, 16);
}
void MainWindow::openSettingsDialog() {
    // 创建 Settings 对话框并显示
    Settings *settingsDialog = new Settings(this);
    settingsDialog->exec();  // 使用 exec() 打开模态对话框
}
MainWindow::~MainWindow()
{
    delete ui;
}
