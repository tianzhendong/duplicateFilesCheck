//
// Created by 12038 on 2022/6/16.
//

#ifndef DUPLICATEFILESCHECK_DUPLICATEFILES_H
#define DUPLICATEFILESCHECK_DUPLICATEFILES_H

#include <QObject>
#include <QHash>
#include <QDebug>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>

class DuplicateFiles : public QObject{
Q_OBJECT

public:
    DuplicateFiles(QObject *parent = nullptr);
    ~DuplicateFiles();

signals:
    void md5Signal(const QByteArray &);
    void filesSignal(const QStringList &);
    void process(const int &, const int &);
    void duplicateFilesSignal(const QHash<QByteArray, QStringList> &);
    void delActionFeedbackSignal(bool delFlag);

public slots:
    void getFilesSlot(const QString & filesDirPath);
    void calMd5Slot(const QString &filePath);
    void getTextSlot(const QString &);
    void openDirSlot();
    void delActionTriggeredSlot();

private:
    QByteArray calMd5(const QString &filePath);
    QStringList getFiles(const QString &);
    QHash<QByteArray, QStringList> duplicateFiles;

    QString currentText = "";

};


#endif //DUPLICATEFILESCHECK_DUPLICATEFILES_H
