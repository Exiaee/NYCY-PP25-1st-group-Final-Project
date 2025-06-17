#include "newwindow.h"
#include "ui_newwindow.h"
#include <QPixmap>

newWindow::newWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::newWindow)
{
    ui->setupUi(this);
}

newWindow::~newWindow()
{
    delete ui;
}
void newWindow::setImage(const QPixmap &pixmap) {
    ui->ShowImage->setPixmap(pixmap.scaled(
        ui->ShowImage->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        ));
    ui->ShowImage->setStyleSheet("border: 1px solid black;");
}

void newWindow::setPHashText(const QString & str) {
    ui->LabelPHash->setText("pHash: "+str);
}
