//
// Created by 12038 on 2022/6/16.
//
#include "DuplicateFiles.h"

DuplicateFiles::DuplicateFiles(QObject *) {

}

DuplicateFiles::~DuplicateFiles() {

}

void DuplicateFiles::calMd5Slot(const QString &filePath) {
    QByteArray md5 = calMd5(filePath);
    emit md5Signal(md5);
}

QByteArray DuplicateFiles::calMd5(const QString &filePath) {
    QFile file(filePath);
    QByteArray md5 = "";
    if(file.open(QIODevice::ReadOnly)){
        QCryptographicHash hash(QCryptographicHash::Md5);
        while(!file.atEnd()){
            QByteArray buff = file.read(100 * 1024 * 1024);
            hash.addData(buff);
        }
        md5 = hash.result().toHex();
        file.close();
    }
    return md5;
}

void DuplicateFiles::getFilesSlot(const QString &filesDirPath) {
    //获取文件夹下所有的文件
    QStringList files= this->getFiles(filesDirPath);
    //计算md5，并存到map中
    QHash<QByteArray, QStringList> md5Results;
    for (int i = 0; i < files.count(); i++){
        QString fileName = files.at(i);
        QByteArray fileMd5 = calMd5(fileName);
        md5Results[fileMd5].append(fileName);
        qDebug() <<"file:" << fileName << "md5："<< fileMd5;
        emit process(i + 1, files.count());
    }
    //找出map中value大于1的
    for (QHash<QByteArray, QStringList>::iterator itr = md5Results.begin(); itr != md5Results.end(); itr++){
        if(itr.value().count() > 1){
            qDebug()<< "md5:"<<itr.key()<<"file:"<<itr.value();
            duplicateFiles[itr.key()] = itr.value();
        }
    }
//    emit filesSignal(files);
    emit duplicateFilesSignal(duplicateFiles);
}


QStringList DuplicateFiles::getFiles(const QString &filesDirPath) {
    QStringList files;
    QDir filesDir(filesDirPath);
    QList<QFileInfo> fileInfoList = filesDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i = 0; i < fileInfoList.count(); i++){
        QFileInfo fileInfo = fileInfoList.at(i);
        if(fileInfo.isDir()){
            QStringList subFiles= getFiles(fileInfo.absoluteFilePath());
            files.append(subFiles);
        }
        else{
            QString fileName = fileInfo.absoluteFilePath();
            files.append(fileName);
        }
    }
    return files;
}

