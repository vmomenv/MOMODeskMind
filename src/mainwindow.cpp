#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "petai.h"
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include "settings.h"
#include <QSettings>
#include <QFile>
#include <QStyle>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFormLayout>
#include <QDateTimeEdit>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint); // 移除边框
    this->setStyleSheet(
        "background-color: #F3F4F6;"
        "border-radius: 8px;"
        );

    // 创建天气API对象
    weatherAPI = new WeatherAPI(this);
    ui->weatherLabel->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    loadAvatar();
    // connect(ui->changeAvatarButton, &QPushButton::clicked, this, &MainWindow::onChangeAvatarButtonClicked);

    // 连接天气API的信号到updateWeatherDisplay槽函数
    connect(weatherAPI, &WeatherAPI::weatherDataUpdated, this, &MainWindow::updateWeatherDisplay);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettingsDialog);

    // 创建宠物AI对象
    petAI = new PetAI(this);
    ui->dialogueWidget->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    ui->answerLabel->setText(petAI->getPetGreeting());

    // 初始化天气API并请求天气数据
    ui->weatherLabel->setText(weatherAPI->getCurrentWeather());


    // 初始化日程提醒
    ui->reminderWidget->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 8px;"
        );
    ui->addReminderButton->setStyleSheet("QPushButton {"
                                         "background-color: #000000;"
                                         "color: #FFFFFF;"
                                         "border-radius: 8px;"
                                         "border: none;"
                                         "padding: 0px;}"
                                         );
    ui->addReminderButton->setFixedSize(68,28);
    ui->countLabel->setStyleSheet("QLabel {"
                                  "background-color: #000000;"
                                  "color: #FFFFFF;"
                                  "border-radius: 8px;"
                                  "border: none;"
                                  "padding: 0px;}"
                                  );
    ui->countLabel->setFixedSize(68,28);
    ui->countLabel->setAlignment(Qt::AlignCenter);



    // 设置日程提醒滚动区域
    ui->reminderScrollArea->setFixedSize(328, 144);
    ui->reminderScrollArea->setWidgetResizable(false); // 必须关闭
    ui->reminderScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->reminderScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 创建内容容器
    reminderWidget = new QWidget();
    reminderWidget->setMinimumWidth(328); // 关键：固定内容宽度
    reminderWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 设置垂直布局
    reminderWidgetLayout = new QVBoxLayout(reminderWidget);
    reminderWidgetLayout->setAlignment(Qt::AlignTop);
    reminderWidgetLayout->setContentsMargins(0, 0, 10, 0);
    reminderWidgetLayout->setSpacing(4);

    ui->reminderScrollArea->setStyleSheet(R"(
    QScrollArea { border: none; }
    QScrollBar:vertical {
        width: 6px;
        background: #F3F4F6;
    }
    QScrollBar::handle:vertical {
        background: #D1D5DB;
        border-radius: 3px;
    }
)");

    // 必须显式设置widget
    ui->reminderScrollArea->setWidget(reminderWidget);


    reminderLoadJsonData("reminderdata.json");


    //初始化ai模块
    aiClient = new AIClient(this); // 修改初始化

}
void MainWindow::reminderLoadJsonData(const QString &filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning() << "Failed to open file!";
        return;
    }
    // 读取文件内容并解析JSON
    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        qWarning() << "Invalid JSON data!";
        return;
    }

    // 获取JSON数组并展示每一条消息
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject jsonObj = value.toObject();
            QString message = jsonObj["message"].toString();  // 假设每条信息的键为 "message"
            QString time = jsonObj["time"].toString();  // 假设每条信息的键为 "message"
            QString priority = jsonObj["priority"].toString();  // 紧急程度（urgent、high、non-urgent）
            displayMessage(message,time, priority);
            qDebug()<<message<<time<<priority;
        }
    }
    updateReminderCount();
}
void MainWindow::displayMessage(const QString &message, const QString &time,const QString &priority)
{
    MessageWidget *widget = new MessageWidget(message, time,priority);
    widget->setFixedHeight(30); // 确保每个条目固定高度
    reminderWidgetLayout->addWidget(widget);

    // 动态调整内容区域高度
    int contentHeight = reminderWidgetLayout->count() * 34; // 30+4=34
    reminderWidget->setMinimumHeight(contentHeight);
    connect(widget, &MessageWidget::deleteClicked, this, [=]{
        // 从布局中移除控件
        reminderWidgetLayout->removeWidget(widget);
        // 安全删除对象
        widget->deleteLater();
        // 重新计算内容高度
        int contentHeight = reminderWidgetLayout->count() * 34;
        reminderWidget->setMinimumHeight(contentHeight);
        // 强制刷新布局
        reminderWidget->adjustSize();
        ui->reminderScrollArea->updateGeometry();
        removeReminderFromJson(widget->getMessage(), widget->getTime(), widget->getPriority());
        updateReminderCount();
        adjustScrollContent();
    });
}
void MainWindow::removeReminderFromJson(const QString &message, const QString &time, const QString &priority)
{
    QFile file("reminderdata.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open file for deletion!";
        return;
    }

    // 读取现有数据
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.isArray() ? doc.array() : QJsonArray();

    // 查找并删除匹配项
    for(int i = 0; i < jsonArray.size();) {
        QJsonObject obj = jsonArray[i].toObject();
        if(obj["message"] == message &&
            obj["time"] == time &&
            obj["priority"] == priority) {
            jsonArray.removeAt(i);
        } else {
            ++i;
        }
    }

    // 写回文件
    file.resize(0);
    file.write(QJsonDocument(jsonArray).toJson());
    file.close();
}
void MainWindow::updateReminderCount()
{
    int count = reminderWidgetLayout->count();
    ui->countLabel->setText(QString("%1项待办").arg(count));
}
void MainWindow::onAskButtonClicked(){
    QString userQuestion = ui->questionInput->text();
    QString response = petAI->getResponse(userQuestion);
    ui->answerLabel->setText(response);
}
void MainWindow::adjustScrollContent()
{
    int contentHeight = reminderWidgetLayout->count() * 34;
    reminderWidget->setMinimumHeight(contentHeight);
    reminderWidget->adjustSize();
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
void MainWindow::on_addReminderButton_clicked()
{
    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加新提醒");
    dialog.setFixedSize(300, 200);

    // 表单控件
    QComboBox priorityCombo;
    QDateTimeEdit timeEdit;
    QLineEdit messageEdit;

    // 初始化控件
    priorityCombo.addItems({"紧急 (urgent)", "高 (high)", "普通 (normal)"});
    timeEdit.setDateTime(QDateTime::currentDateTime());
    timeEdit.setDisplayFormat("yyyy-MM-dd HH:mm");
    messageEdit.setPlaceholderText("输入提醒内容...");

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("优先级:", &priorityCombo);
    formLayout->addRow("时间:", &timeEdit);
    formLayout->addRow("内容:", &messageEdit);

    // 按钮组
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttons);

    if (dialog.exec() == QDialog::Accepted) {
        // 解析输入数据
        QString priority;
        switch (priorityCombo.currentIndex()) {
        case 0: priority = "urgent"; break;
        case 1: priority = "high"; break;
        default: priority = "normal";
        }

        QString time = timeEdit.dateTime().toString("hh:mm");
        QString message = messageEdit.text().trimmed();

        // 验证输入
        if (message.isEmpty()) {
            QMessageBox::warning(this, "错误", "提醒内容不能为空");
            return;
        }

        // 添加提醒到界面
        displayMessage(message, time, priority);

        // 保存到JSON文件
        saveReminderToJson(message, time, priority);

        // 更新待办计数
        ui->countLabel->setText(QString("%1项待办").arg(reminderWidgetLayout->count()));
    }
}

void MainWindow::saveReminderToJson(const QString &message, const QString &time, const QString &priority)
{
    QFile file("reminderdata.json");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning() << "Failed to open file for saving!";
        return;
    }

    // 读取现有数据
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.isArray() ? doc.array() : QJsonArray();

    // 创建新条目
    QJsonObject newEntry;
    newEntry["message"] = message;
    newEntry["time"] = time;
    newEntry["priority"] = priority;
    jsonArray.append(newEntry);

    // 写回文件
    file.resize(0);
    file.write(QJsonDocument(jsonArray).toJson());
    file.close();
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
void MainWindow::loadAvatar()
{
    // 获取配置文件中的头像路径
    QSettings settings("momodesk-mind", "settings");
    QString avatarPath = settings.value("avatarPath").toString();
    qDebug() << "Avatar path from settings:" << avatarPath;

    QPixmap avatar;
    if (!avatarPath.isEmpty() && QFile::exists(avatarPath)) {
        // 如果头像路径有效，加载头像
        avatar.load(avatarPath);
    } else {
        // 如果头像路径无效，则加载默认头像
        qDebug() << "Failed to load avatar. Loading default avatar.";
        avatar.load(":/img/momen.jpg");
    }

    // 将头像裁剪为圆形
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
    ui->avatarLabel->move(128, 16);  // 设置头像的位置
}

void MainWindow::openSettingsDialog() {
    // 创建 Settings 对话框并显示
    Settings *settingsDialog = new Settings(this);
    connect(settingsDialog,&Settings::avatarUpdated,this,&MainWindow::loadAvatar);
    settingsDialog->exec();  // 使用 exec() 打开模态对话框
}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_askButton_clicked()
{
}

