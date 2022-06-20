//
// Created by 12038 on 2022/6/20.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("重复文件检测工具-byTianZD");

    duplicateFiles = new DuplicateFiles();
    myThread = new QThread();
    duplicateFiles->moveToThread(myThread);
    myThread->start();
    connect(duplicateFiles, SIGNAL(destroyed(QObject *)),
            myThread, SLOT(deleteLater()));

//    ui->labelTitle->setText("文件一致性检测工具");
    connect(ui->btnSelectFile, SIGNAL(clicked(bool)), this, SLOT(calMd5ofFileSlot()));
    connect(ui->btnSelectDir, SIGNAL(clicked(bool)), this, SLOT(selectDirSlot()));
    connect(this, SIGNAL(calFileMd5Signal(const QString &)),
            duplicateFiles, SLOT(calMd5Slot(const QString &)));
    connect(duplicateFiles, SIGNAL(md5Signal(const QByteArray &)),
            this, SLOT(showFileMd5Slot(const QByteArray &)));
    connect(this, SIGNAL(getFilesSignal(const QString &)),
            duplicateFiles, SLOT(getFilesSlot(const QString &)));
    connect(duplicateFiles, SIGNAL(filesSignal(const QStringList &)),
            this, SLOT(filesSlot(const QStringList &)));

    connect(duplicateFiles, SIGNAL(process(const int &, const int &)),
            this, SLOT(processSlot(const int &, const int &)));
    connect(duplicateFiles, SIGNAL(duplicateFilesSignal(const QHash<QByteArray, QStringList> &)),
            this, SLOT(duplicateFilesSlot(const QHash<QByteArray, QStringList> &)));
    connect(ui->listWidget, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(currentTextChangedSlot(const QString &)));

    connect(ui->listWidget_2, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(on_listWidget_customContextMenuRequested(const QPoint &)));
    ui->listWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);

    delAction = new QAction();
    delAction->setText("删除文件");
    openAction = new QAction();
    openAction->setText("打开文件目录");

    connect(delAction, SIGNAL(triggered(bool)),
            duplicateFiles, SLOT(delActionTriggeredSlot()));
    connect(ui->listWidget_2, SIGNAL(currentTextChanged(const QString &)),
            duplicateFiles, SLOT(getTextSlot(const QString &)));
    connect(openAction, SIGNAL(triggered(bool)),
            duplicateFiles, SLOT(openDirSlot()));
    connect(duplicateFiles, SIGNAL(delActionFeedbackSignal(bool)),
            this, SLOT(delActionFeedbackSlot(bool)));

}

MainWindow::~MainWindow() {
    duplicateFiles->deleteLater();
    myThread->exit();
    myThread->wait(10 * 1000);
    delAction->deleteLater();
    openAction->deleteLater();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("退出程序"),QString(tr("确认退出程序?")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No)
    {
        event->ignore(); // 忽略退出信号，程序继续进行
    }
    else if(button==QMessageBox::Yes)
    {
        event->accept(); // 接受退出信号，程序退出
    }
}

void MainWindow::btnMaxClickedSlot()
{
    if(this->isMaximized()){
        this->showNormal();
    }
    else{
        this->showMaximized();
    }
}

void MainWindow::btnMinClickedSlot()
{
    this->showMinimized();
}

void MainWindow::btnCloseClickedSlot()
{
    this->close();
}

void MainWindow::calMd5ofFileSlot() {
    QString path = QFileDialog::getOpenFileName(
            this, "选择文件",
            "./",
            "");
    emit calFileMd5Signal(path);
}

void MainWindow::showFileMd5Slot(const QByteArray & md5) {
    ui->leMd5Show->setText("");
    ui->leMd5Show->setText(md5);
}

void MainWindow::selectDirSlot() {
    ui->progressBar->setValue(0);
    QString dirPathUrl = QFileDialog::getExistingDirectory(this, "选择文件夹", "./");
    ui->lineDIrShow->setText(dirPathUrl);
    emit getFilesSignal(dirPathUrl);
}

void MainWindow::filesSlot(const QStringList &files) {
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(files);
}

void MainWindow::processSlot(const int &now, const int &total) {
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(now);
}

void MainWindow::duplicateFilesSlot(const QHash<QByteArray, QStringList> &duplicateFiles) {
    ui->listWidget->clear();
    this->duplicateResults = duplicateFiles;
    for(QHash<QByteArray, QStringList>::const_iterator itr = duplicateFiles.begin(); itr != duplicateFiles.end(); itr++){
        ui->listWidget->addItem(itr.key());
    }
}

void MainWindow::currentTextChangedSlot(const QString &currentText) {
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(this->duplicateResults[currentText.toLocal8Bit()]);
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
//    ui->listWidget_2->currentTextChanged()
    QMenu *menu = new QMenu(this);
//    menu->addAction(ui->actionAdd);
//    menu->addAction(ui->actionClear);
//    menu->addAction(ui->actionDelete);
//    menu->addAction(ui->actionInsert);
    menu->addAction(this->delAction);
    menu->addAction(this->openAction);
    menu->addSeparator();

//    menu->addAction(ui->actionInit);
//    menu->addSeparator();
//    menu->addAction(ui->actionSelAll);
//    menu->addAction(ui->actionSelInv);
//    menu->addAction(ui->actionSelNone);
//    menu->addAction(ui->actionSelPopMenu);
    menu->exec(QCursor::pos());
    delete  menu;
}

void MainWindow::delActionFeedbackSlot(bool flag) {
    if(flag){
        qDebug()<<"remove item";
        QListWidgetItem * item = ui->listWidget_2->currentItem();
        qDebug()<<item;
        ui->listWidget_2->removeItemWidget(item);
        delete item;
    }
}