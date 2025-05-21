#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTcpSocket>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QStandardItemModel>


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
    void toggleTree();

private:
    Ui::MainWindow *ui;
    QSplitter *splitter;
    QFileSystemModel *fileModel;

    QTcpSocket *socket;
    QByteArray Data;

    void closeTab(int index);
    void FileClicked(const QModelIndex &index);
    void processJsonTree(const QJsonObject &json);

    QFileSystemModel *fileSystemModel = nullptr;
    QStandardItemModel *jsonModel = nullptr;

    void SendToServer(QString str);
    quint16 nextBlockSize;

private slots:
    void on_pushButton_connect_clicked();
    void on_pushButton_getTree_clicked();
    void on_lineEdit_returnPressed();

public slots:
    void slotReadyRead();

};
#endif // MAINWINDOW_H
