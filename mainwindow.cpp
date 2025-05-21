#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QPlainTextEdit>
#include <QFileInfo>
#include <QDir>
#include <QSplitter>
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->showMaximized();
    // Удаляем 2 стартовые страницы, прячем tabwidget и кнопку
    ui->tabWidget->clear();
    ui->tabWidget->hide();
    ui->pushButton->setVisible(false);

    // Клик по файлу
    connect(ui->treeView, &QTreeView::doubleClicked, this, &MainWindow::FileClicked);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    // splitter = ui->splitter;
    // splitter->setSizes({600, 200});
    // Обработка нажатие кнопки, вызов функции
    connect(ui->pu[xz@archlinux LogViewer-main]$ git push
                    Everything up-to-date
                    [xz@archlinux LogViewer-main]$shButton, &QPushButton::clicked,this,&MainWindow::toggleTree);

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
    // 123 123 123
}


// СЛОТ
void MainWindow::FileClicked(const QModelIndex &index)
{
    QString filePath = fileModel->filePath(index);

    // Проверка является ли это файлом
    if (!QFileInfo(filePath).isFile())
        return;

    QString ext = QFileInfo(filePath).suffix().toLower();
    QStringList allowedExt = { "txt", "log", "csv" };

    if (!allowedExt.contains(ext)) {
        QString msg = "Файл с расширением ." + ext + " не поддерживается.";
        statusBar()->showMessage(msg, 5000);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString msg = "Ошибка открытия файла: " + file.errorString();
        statusBar()->showMessage(msg, 5000);
        return;
    }

    QString content = file.readAll();
    file.close();

    // Открываем tabwidget если был скрыт
    ui->tabWidget->show();
    ui->pushButton->setVisible(true);

    // Создание виджета с текстом лога
    QPlainTextEdit *logViewer = new QPlainTextEdit;
    logViewer->setPlainText(content);
    logViewer->setReadOnly(true);

    // Имя файла как заголовок вкладки
    QString fileName = QFileInfo(filePath).fileName();

    // Добавление новой вкладки
    ui->tabWidget->addTab(logViewer, fileName);
    ui->tabWidget->setCurrentWidget(logViewer);
}

// СЛОТ
void MainWindow::closeTab(int index)
{
    // Получение индекса вкладки
    QWidget *tab = ui->tabWidget->widget(index);
    // Удаляем вкладку
    ui->tabWidget->removeTab(index);
    delete tab;

    // Перемещаем сплитер влево если вкладок нет
    if (ui->tabWidget->count()==0)
    {
        ui->tabWidget->hide();
        ui->pushButton->setVisible(false);
        ui->treeView->setVisible(true);
    } else
    {
        ui->pushButton->setVisible(true);
    }
}

// СЛОТ
void MainWindow::toggleTree()
{
    if(!ui->tabWidget->isVisible()) return;
    bool visible = ui->treeView->isVisible();
    ui->treeView->setVisible(!visible);

    // Двигаем сплитер
    visible? splitter->setSizes({800,0}) : splitter->setSizes({600,200});
    // Меняем текст на кнопке
    ui->pushButton->setText(visible ? "<<" : ">>");
}


MainWindow::~MainWindow()
{
    delete ui;
}


// ПОДКЛЮЧАЕМСЯ
void MainWindow::on_pushButton_connect_clicked()
{
    socket->connectToHost("127.0.0.1", 2323);
}

// СЛОТ - ЧИТАЕМ
void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok)
    {
        // Чтение происходит только после того, как придут все отправленные данные
        for (;;)
        {
            if (nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }

            QString str;
            in >> str;
            nextBlockSize = 0;
            // Проверяем, является ли строка JSON
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &parseError);

            if (parseError.error == QJsonParseError::NoError && doc.isObject())
            {
                // Это JSON - парсим и выводим в treeView
                processJsonTree(doc.object());
            }
            else
            {
                // Это обычный текст - выводим в textBrowser
                ui->textBrowser->append(str);
            }
        }
    }
    else
    {
        ui->textBrowser->append("Ошибка при чтении данных!");
    }
}




// УПАКОВЫВАЕМ И ОТПРАВЛЯЕМ
void MainWindow::SendToServer(QString str)
{
    Data.clear();  // Чистить перед каждым использованием
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);

    out << quint16(0) << str; // Резервация 2-х первых байтов под значение всего размера и запись str
    out.device()->seek(0);
    out << quint16(Data.size() - sizeof(quint16));

    socket->write(Data);
    qDebug() << "Данные отправлены в сокет на сервер";
}


// СЛОТ ? - РЕАГИРУЕМ НА КЛИК
void MainWindow::on_pushButton_getTree_clicked()
{
    // fileModel = new QFileSystemModel(this);
    // QString defaultPath = QDir("C:").filePath("logs");
    // fileModel->setRootPath(defaultPath);
    // ui->treeView->setModel(fileModel);
    // ui->treeView->setRootIndex(fileModel->index(defaultPath));

    SendToServer(ui->pushButton_getTree->text());
}

void MainWindow::on_lineEdit_returnPressed()
{
    SendToServer(ui->lineEdit->text());
    ui->lineEdit->clear();
}


void MainWindow::processJsonTree(const QJsonObject &json)
{
    // Создаём модель (если ещё не создана)
    if (!fileSystemModel) {
        fileSystemModel = new QFileSystemModel(this);
        fileSystemModel->setRootPath(""); // Пустая строка - отображаем всё
        ui->treeView->setModel(fileSystemModel);
    }

    // Альтернатива: кастомная модель для отображения полученного JSON
    // Можно использовать QStandardItemModel:
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Name", "Size", "Modified"});

    // Рекурсивная функция для заполнения модели
    std::function<void(QStandardItem*, const QJsonObject&)> buildTree;
    buildTree = [&buildTree](QStandardItem *parent, const QJsonObject &node) {
        // Добавляем подпапки
        if (node.contains("folders")) {
            for (const QJsonValue &folder : node["folders"].toArray()) {
                QJsonObject folderObj = folder.toObject();
                QStandardItem *folderItem = new QStandardItem(folderObj["name"].toString());
                folderItem->setData(folderObj["path"].toString(), Qt::UserRole); // Сохраняем путь
                parent->appendRow(folderItem);
                buildTree(folderItem, folderObj);
            }
        }

        // Добавляем файлы
        if (node.contains("files")) {
            for (const QJsonValue &file : node["files"].toArray()) {
                QJsonObject fileObj = file.toObject();
                QList<QStandardItem*> items;
                items << new QStandardItem(fileObj["name"].toString());
                items << new QStandardItem(QString::number(fileObj["size"].toInt()));
                items << new QStandardItem(fileObj["lastModified"].toString());
                parent->appendRow(items);
            }
        }
    };

    // Заполняем корень
    QStandardItem *rootItem = new QStandardItem(json["name"].toString());
    rootItem->setData(json["path"].toString(), Qt::UserRole);
    model->appendRow(rootItem);
    buildTree(rootItem, json);

    // Устанавливаем модель в treeView
    ui->treeView->setModel(model);
    ui->treeView->expandAll(); // Раскрываем все узлы
}
