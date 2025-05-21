#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);

    QString serverAddress() const;
    QString serverIP() const;
    quint16 serverPort() const;

private slots:
    void onConnectClicked();

private:
    QLineEdit *addressEdit;
    QPushButton *connectButton;
};

#endif // CONNECTDIALOG_H
