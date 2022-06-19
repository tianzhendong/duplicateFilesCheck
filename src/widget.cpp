//
// Created by 12038 on 2022/6/15.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Widget.h" resolved

#include <QProcess>
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
        : QWidget(parent)
        , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //取消菜单栏
    this->setWindowFlags(Qt::FramelessWindowHint);

    //阴影边框效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(Qt::black);
    shadow->setOffset(0);

    ui->shadowWidget->setGraphicsEffect(shadow);

    //父窗口透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    //最大化最小化关闭功能实现
    connect(ui->btnMax, SIGNAL(clicked()), this, SLOT(btnMaxClickedSlot()));
    connect(ui->btnMin, SIGNAL(clicked()), this, SLOT(btnMinClickedSlot()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(btnCloseClickedSlot()));

    ui->btnMin->setStyleSheet("border-image: url(:/png/min.png)");
    ui->btnMax->setStyleSheet("border-image: url(:/png/fullscreen3.png)");
    ui->btnClose->setStyleSheet("border-image: url(:/png/close.png)");


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

Widget::~Widget()
{
    duplicateFiles->deleteLater();
    myThread->exit();
    myThread->wait(10 * 1000);

    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
//    QWidget::mousePressEvent(event);
    QPoint mouseStartPoint = event->globalPos();
    QPoint windowLeftTopPoint = this->geometry().topLeft();
    this->mousePosInWindow = mouseStartPoint - windowLeftTopPoint;
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
//    QWidget::mouseMoveEvent(event);
    if(this->mousePosInWindow == QPoint()) return;
    QPoint mousePoint = event->globalPos();
    QPoint windowLeftTopPoint = mousePoint - this->mousePosInWindow;
    this->move(windowLeftTopPoint);
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    this->mousePosInWindow = QPoint();
}

void Widget::closeEvent(QCloseEvent *event)
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

void Widget::btnMaxClickedSlot()
{
    ui->btnMax->setStyleSheet("border-image: url(:/png/fullscreen4.png)");
    if(this->isMaximized()){
        ui->layoutMain->setMargin(9);
        ui->btnMax->setStyleSheet("border-image: url(:/png/fullscreen3.png)");
        this->showNormal();
    }
    else{
        ui->layoutMain->setMargin(0);
        ui->btnMax->setStyleSheet("border-image: url(:/png/fullscreen4.png)");
        this->showMaximized();
    }
}

void Widget::btnMinClickedSlot()
{
    this->showMinimized();
}

void Widget::btnCloseClickedSlot()
{
    this->close();
}

void Widget::calMd5ofFileSlot() {
    QString path = QFileDialog::getOpenFileName(
            this, "选择文件",
            "./",
            "");
    emit calFileMd5Signal(path);
}

void Widget::showFileMd5Slot(const QByteArray & md5) {
    ui->leMd5Show->setText("");
    ui->leMd5Show->setText(md5);
}

void Widget::selectDirSlot() {
    ui->progressBar->setValue(0);
    QString dirPathUrl = QFileDialog::getExistingDirectory(this, "选择文件夹", "./");
    ui->lineDIrShow->setText(dirPathUrl);
    emit getFilesSignal(dirPathUrl);
}

void Widget::filesSlot(const QStringList &files) {
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(files);
}

void Widget::processSlot(const int &now, const int &total) {
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(now);
}

void Widget::duplicateFilesSlot(const QHash<QByteArray, QStringList> &duplicateFiles) {
    ui->listWidget->clear();
    this->duplicateResults = duplicateFiles;
    for(QHash<QByteArray, QStringList>::const_iterator itr = duplicateFiles.begin(); itr != duplicateFiles.end(); itr++){
        ui->listWidget->addItem(itr.key());
    }
}

void Widget::currentTextChangedSlot(const QString &currentText) {
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(this->duplicateResults[currentText.toLocal8Bit()]);
}

void Widget::on_listWidget_customContextMenuRequested(const QPoint &pos)
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

void Widget::delActionFeedbackSlot(bool flag) {
    if(flag){
        qDebug()<<"remove item";
        QListWidgetItem * item = ui->listWidget_2->currentItem();
        qDebug()<<item;
        ui->listWidget_2->removeItemWidget(item);
        delete item;
    }
}


