#include "connectiondialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRegularExpression>
#include <QMessageBox>

ConnectDialog::ConnectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Подключение к серверу");

    auto *layout = new QVBoxLayout(this);

    setMinimumWidth(500);

    auto *label = new QLabel("Введите адрес сервера:");
    addressEdit = new QLineEdit(this);
    addressEdit->setPlaceholderText("127.0.0.1:8080");
    addressEdit->setText("127.0.0.1:2323");

    connectButton = new QPushButton("Подключиться", this);

    layout->addWidget(label);
    layout->addWidget(addressEdit);
    layout->addWidget(connectButton);

    connect(connectButton, &QPushButton::clicked, this, &ConnectDialog::onConnectClicked);
}

void ConnectDialog::onConnectClicked()
{
    QString address = addressEdit->text().trimmed();

    // IPv4 и порт от 1 до 65535
    QRegularExpression regex(R"(^(?:(?:25[0-5]|2[0-4]\d|1?\d{1,2})\.){3}(?:25[0-5]|2[0-4]\d|1?\d{1,2}):([1-9][0-9]{0,4})$)");
    QRegularExpressionMatch match = regex.match(address);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Введите корректный IP-адрес и порт, например:\n127.0.0.1:8080");
        return;
    }

    int port = match.captured(1).toInt();
    if (port > 65535) {
        QMessageBox::warning(this, "Ошибка", "Порт должен быть в диапазоне от 1 до 65535.");
        return;
    }

    accept();
}

QString ConnectDialog::serverIP() const
{
    QString address = addressEdit->text().trimmed();
    return address.section(':', 0, 0);  // Всё до двоеточия
}

quint16 ConnectDialog::serverPort() const
{
    QString address = addressEdit->text().trimmed();
    return address.section(':', 1, 1).toUShort();
}
