// MessageWidget.h
#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const QString &message,
                           const QString &priority,
                           QWidget *parent = nullptr);
    ~MessageWidget();

signals:
    void deleteClicked(MessageWidget* widget);  // 带对象指针的信号

private slots:
    void onDeleteClicked();

private:
    void setPriorityColor(const QString &priority);

    // UI 组件
    QWidget *priorityIndicator;
    QLabel *iconLabel;       // 新增日历图标标签
    QLabel *messageLabel;
    QPushButton *deleteButton;
    QHBoxLayout *layout;
};

#endif // MESSAGEWIDGET_H
