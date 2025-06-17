#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include "comparedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "imagephash.h"
#include <QButtonGroup>
#include "newwindow.h"
#include "imagephash.h"


QButtonGroup *method;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->imageLabel->setFixedSize(300, 300);
    ui->imageLabel->setStyleSheet("border: 1px solid gray");
    ui->imageLabel->setAlignment(Qt::AlignCenter);
    method = new QButtonGroup(this);
    method->addButton(ui->radioWhileLoop,0);
    method->addButton(ui->radioOpenMP,1);
    method->addButton(ui->radioPthread,2);
    method->addButton(ui->radioQtCompare,3);
    method->addButton(ui->radioQtOpenMP,4);
    ui->radioWhileLoop->setChecked(true);

    //radioQtOpenMP


    connect(ui->radioWhileLoop,SIGNAL(clicked(bool)),this,SLOT(on_method_clicked()));
    connect(ui->radioOpenMP,SIGNAL(clicked(bool)),this,SLOT(on_method_clicked()));
    connect(ui->radioPthread,SIGNAL(clicked(bool)),this,SLOT(on_method_clicked()));

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateDBLabel(const QString &path) {
    if (path.isEmpty()) {
        ui->labelDBInfo->setText("Database: (none)");
    } else {
        //ui->labelDBInfo->setText("Database: " + path + " (SQLite)");
        QString fileName = QFileInfo(path).fileName();
        ui->labelDBInfo->setText("Database: " + fileName + " (SQLite)");
    }
}

void MainWindow::on_btnOpenDB_clicked() {
    QString dbPath = QFileDialog::getOpenFileName(this, "Open Database", "", "SQLite DB (*.db)");
    if (dbPath.isEmpty()) return;
    if (Database::openFromFile(dbPath)) {
        QMessageBox::information(this, "Success", "Database opened.");
        updateDBLabel(dbPath);
        refreshList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to open database.");
    }
}

void MainWindow::on_btnInsert_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) return;
    QString name = QFileInfo(filePath).baseName();
    if (Database::insertImage(name, filePath)) {
        refreshList();
        QMessageBox::information(this, "Inserted", "Image inserted.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to insert image.");
    }
}

void MainWindow::on_btnSearch_clicked() {
    QString keyword = ui->lineEdit->text().trimmed();
    refreshList(keyword);
}

void MainWindow::on_btnDelete_clicked() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) return;

    QString rawText = item->text();
    QStringList parts = rawText.split(" | ");
    if (parts.isEmpty()) return;
    QString name = parts[0].trimmed();

    if (Database::deleteImage(name)) {
        refreshList();
        ui->imageLabel->clear();
        QMessageBox::information(this, "Deleted", "Image deleted.");
    }
}

void MainWindow::on_btnEdit_clicked() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) return;

    QString rawText = item->text();
    QStringList parts = rawText.split(" | ");
    if (parts.isEmpty()) return;
    QString name = parts[0].trimmed();

    QString filePath = QFileDialog::getOpenFileName(this, "Replace Image", "", "Images (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty()) return;
    if (Database::insertImage(name, filePath)) {
        QMessageBox::information(this, "Updated", "Image replaced.");
        refreshList();
    }
}

void MainWindow::on_btnFindSimilar_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select image to compare", "", "Images (*.png *.jpg)");
    if (filePath.isEmpty()) return;
    if(newWindowObj!=nullptr){
        newWindowObj->close();
        delete newWindowObj;
        newWindowObj = nullptr;
    }
    QString LoadImagePhash=ImagePHash::computeHashCV(filePath);
    int selectedMethod = method->checkedId();  // 0: while, 1: OpenMP, 2: QThread
    int repeatTimes = ui->spinRepeat->value();
    int HD = ui->spinBox->value();
    this->newWindowObj = new newWindow();
    QString filename=QFileInfo(filePath).fileName();
    newWindowObj->setWindowTitle("Show Image: " + filename+ QString(" (Hamming Distance: %1)").arg(HD));
    QPixmap pix(filePath);
    newWindowObj->setImage(pix);
    newWindowObj->setPHashText(LoadImagePhash);
    newWindowObj->show();
    QStringList matches;

    //QElapsedTimer timer;
    double elapsed = 0;
    QString matchInfo;
    double totalElapsed = 0;
    for (int i = 0; i < repeatTimes; ++i){
        QElapsedTimer timer;
        timer.start();
        switch(selectedMethod)
        {
        case 0:
            //timer.start();
            matches = Database::findSimilarImages(filePath, HD);
            //elapsed = timer.elapsed();
            //matchInfo = matches.isEmpty() ? "No Match" : QString("%1 match(es)").arg(matches.size());
            //ui->labelTimeSequential->setText(QString("Sequential Time: %1 ms (%2)").arg(elapsed).arg(matchInfo));

            qDebug()<<"Sequential";
            break;
        case 1:
            //timer.start();
            matches = Database::findSimilarImagesOpenMP(filePath, HD);
            //elapsed = timer.elapsed();
            //matchInfo=matches.isEmpty()?"No Match" : QString("%1 match(es)").arg(matches.size());
            //ui->labelTimeOpenMP->setText(QString("OpenMP Time: %1 ms (%2)").arg(elapsed).arg(matchInfo));
            qDebug()<<"OpenMP";
            break;
        case 2:
            //timer.start();
            matches = Database::findSimilarImagesPthread(filePath, HD);
            //elapsed = timer.elapsed();
           // matchInfo=matches.isEmpty()?"No Match" : QString("%1 macth(es)").arg(matches.size());
            //ui->labelTimePThread->setText(QString("PThread Time: %1 ms (%2)").arg(elapsed).arg(matchInfo));
            qDebug()<<"PThread";
            break;
        case 3:
            //timer.start();
            matches = Database::findSimilarImagesQT(filePath);
            //elapsed = timer.elapsed();
            // matchInfo=matches.isEmpty()?"No Match" : QString("%1 macth(es)").arg(matches.size());
            //ui->labelTimePThread->setText(QString("PThread Time: %1 ms (%2)").arg(elapsed).arg(matchInfo));
            qDebug()<<"QtFunction()";
            break;

        case 4:
            //timer.start();
            matches = Database::findSimilarImagesQtOpenMP(filePath);
            //elapsed = timer.elapsed();
            // matchInfo=matches.isEmpty()?"No Match" : QString("%1 macth(es)").arg(matches.size());
            //ui->labelTimePThread->setText(QString("PThread Time: %1 ms (%2)").arg(elapsed).arg(matchInfo));
            qDebug()<<"QtFunction() OpenMP";
            break;
        default:
            QMessageBox::warning(this, "Error", "Please select a comparison method.");
            return;
        }

        totalElapsed += timer.elapsed();
    }
    double avgTime = double(totalElapsed)/repeatTimes/1000;
    totalElapsed/=1000;
    matchInfo = matches.isEmpty()?"No Match":QString("%1 match(es)").arg(matches.size());
    QString labelText = QString("Avg:%1 s | Total:%2 s (%3)")
                            .arg(avgTime,0,'f',2).arg(totalElapsed,0,'f',2).arg(matchInfo);
    switch(selectedMethod){
        case 0:
            ui->labelTimeSequential->setText(labelText);
            break;
        case 1:
            ui->labelTimeOpenMP->setText(labelText);
            break;
        case 2:
            ui->labelTimePThread->setText(labelText);
            break;
        case 3:
            ui->labelTimeQt->setText(labelText);
            break;
        case 4:
            ui->labelTimeQtOpenMP->setText(labelText);
            break;
    }

    ui->listWidget->clear();
    if (matches.isEmpty()) {
        QMessageBox::information(this, "No Match", "No similar image found.");
        ui->listWidget->clear();
        ui->imageLabel->clear();
        return;
    }

    ui->listWidget->addItems(matches);
    for (const QString &name : matches) {
        QString text=name.section(" (",0,0);
        QPixmap pix = Database::loadImage(text);
        ui->imageLabel->setPixmap(pix.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));

    }


}

void MainWindow::on_listWidget_itemClicked() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (!item) return;

    QString rawText = item->text();
    QStringList parts = rawText.split(" | ");
    if (parts.isEmpty()) return;
    //QString name = parts[0].trimmed();
    QString name=rawText;
    QPixmap pix = Database::loadImage(name);
    if (!pix.isNull()) {
        ui->imageLabel->setPixmap(pix.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio));
    } else {
        ui->imageLabel->clear();
        QMessageBox::warning(this, "Not Found", "Image data not found for: " + name);
    }
}

void MainWindow::refreshList(const QString &filter) {
    ui->listWidget->clear();
    QStringList names = Database::listAllImages(filter);

    for (const QString &name : names) {
        QString trimmedName = name.trimmed();
        if (trimmedName.isEmpty()) {
            qWarning() << "[refreshList] Skip empty name";
            continue;
        }

        ui->listWidget->addItem(trimmedName);
    }
}


void MainWindow::on_btnCreateDB_clicked() {
    QString dbPath = QFileDialog::getSaveFileName(this, "Create New Database", "", "SQLite DB (*.db)");
    if (dbPath.isEmpty()) return;

    if (Database::openFromFile(dbPath)) {
        QMessageBox::information(this, "Success", "New database created.");
        updateDBLabel(dbPath);
        refreshList();
    } else {
        QMessageBox::warning(this, "Error", "Failed to create database.");
    }
}

void MainWindow::on_btnShowSchema_clicked() {
    QString output;
    QString tableName = "images";

    // 顯示欄位資訊
    QSqlQuery infoQuery(QString("PRAGMA table_info(%1)").arg(tableName));
    QStringList headers;
    while (infoQuery.next()) {
        headers << infoQuery.value(1).toString();  // 欄位名稱
    }
    output += QString("Table: %1\nFields: %2\n\n").arg(tableName).arg(headers.join(", "));

    // 顯示前五筆資料
    QSqlQuery dataQuery(QString("SELECT * FROM %1 LIMIT 5").arg(tableName));
    while (dataQuery.next()) {
        QStringList row;
        for (int i = 0; i < headers.size(); ++i) {
            QVariant val = dataQuery.value(i);
            if (val.type() == QVariant::ByteArray) {
                row << QString("[BLOB %1 bytes]").arg(val.toByteArray().size());
            } else {
                row << val.toString();
            }
        }
        output += "→ " + row.join(" | ") + "\n";
    }

    QMessageBox::information(this, "images Preview", output);
}


void MainWindow::on_btnOpenImage_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "選擇圖片",
        QDir::homePath(),
        "Image Files (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (filePath.isEmpty()) {
        qDebug() << "[OpenImage] 使用者取消選擇。";
        return;
    }

    qDebug() << "[OpenImage] 選擇檔案：" << filePath;

    // 用 computeHashCV 來計算 pHash
    QString hash = ImagePHash::computeHashCV(filePath);
    if (hash.isEmpty()) {
        QMessageBox::warning(this, "錯誤", "無法計算圖片 pHash！");
        return;
    }

    QMessageBox::information(this, "pHash 結果", "pHash: " + hash);
}

void MainWindow::on_method_clicked(){
    /*switch(method->checkedId())
    {
    case 0:

        break;
    case 1:

        break;
    case 2:
        break;
    }*/
}

void MainWindow::on_btnFindSimilar_toggled(bool checked)
{

}

