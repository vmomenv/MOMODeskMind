#include "MessageWidget.h"
#include <QDebug>
#include <QColor>

MessageWidget::MessageWidget(const QString &message, const QString &priority, QWidget *parent)
    : QWidget(parent),
    messageLabel(new QLabel(message, this)),
    deleteButton(new QPushButton("删除", this)),
    priorityIndicator(new QWidget(this)),  // 新增颜色点
    layout(new QHBoxLayout(this))
{
    messageLabel->setWordWrap(true);  // 自动换行
    layout->addWidget(priorityIndicator);  // 将颜色点加入布局
    layout->addWidget(messageLabel);
    layout->addWidget(deleteButton);

    // 设置颜色点的背景色
    setPriorityColor(priority);

    // 连接删除按钮
    connect(deleteButton, &QPushButton::clicked, this, &MessageWidget::onDeleteClicked);

    setLayout(layout);
}

MessageWidget::~MessageWidget()
{
}

void MessageWidget::onDeleteClicked()
{
    // 发射删除信号，通知主窗口删除此 widget
    emit deleteClicked(this);
}

void MessageWidget::setPriorityColor(const QString &priority)
{
    // 设置颜色点的背景色
    QColor color;
    if (priority == "urgent") {
        color = QColor("#EF4444");  // 紧急：红色
    } else if (priority == "high") {
        color = QColor("#EAB308");  // 较紧急：黄色
    } else {
        color = QColor("#22C55E");  // 非紧急：绿色
    }

    // 设置颜色点的大小和颜色
    priorityIndicator->setFixedSize(15, 15);  // 设置颜色点的尺寸
    priorityIndicator->setStyleSheet("background-color: " + color.name() + "; border-radius: 7px;");  // 圆形背景
}
