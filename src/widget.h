//
// Created by 12038 on 2022/6/15.
//

#ifndef DUPLICATEFILESCHECK_WIDGET_H
#define DUPLICATEFILESCHECK_WIDGET_H

#include <QMouseEvent>
#include <QWidget>
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
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

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
    Ui::Widget *ui;
    DuplicateFiles * duplicateFiles;
    QThread *myThread;

    QPoint mousePosInWindow = QPoint();
    QHash<QByteArray, QStringList> duplicateResults;

    QAction *delAction;
    QAction *openAction;


};


#endif //DUPLICATEFILESCHECK_WIDGET_H
