//
// Created by 12038 on 2022/6/20.
//

#ifndef DUPLICATEFILESCHECK_MAINWINDOW_H
#define DUPLICATEFILESCHECK_MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include "DuplicateFiles.h"
#include <QDir>
#include <QDebug>
#include <QThread>
#include <QFileDialog>
#include <QMenu>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void calFileMd5Signal(const QString &path);
    void getFilesSignal(const QString &dirPath);

public slots:
    void showFileMd5Slot(const QByteArray &);
    void filesSlot(const QStringList &);
    void processSlot(const int &, const int &);
    void duplicateFilesSlot(const QHash<QByteArray, QStringList> &);
    void currentTextChangedSlot(const QString &);
    void delActionFeedbackSlot(bool flag);

private slots:
    void btnMaxClickedSlot();
    void btnMinClickedSlot();
    void btnCloseClickedSlot();
    void calMd5ofFileSlot();
    void selectDirSlot();

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
    DuplicateFiles * duplicateFiles;
    QThread *myThread;

    QPoint mousePosInWindow = QPoint();
    QHash<QByteArray, QStringList> duplicateResults;

    QAction *delAction;
    QAction *openAction;
};


#endif //DUPLICATEFILESCHECK_MAINWINDOW_H
