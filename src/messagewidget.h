#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(const QString &message, const QString &priority = "non-urgent", QWidget *parent = nullptr);
    ~MessageWidget();

signals:
    void deleteClicked(MessageWidget *widget);

private slots:
    void onDeleteClicked();

private:
    QLabel *messageLabel;
    QPushButton *deleteButton;
    QWidget *priorityIndicator;  // 用于表示颜色点
    QHBoxLayout *layout;

    void setPriorityColor(const QString &priority);  // 设置颜色点颜色
};

#endif // MESSAGEWIDGET_H
