
#include "comparedialog.h"
#include "ui_comparedialog.h"

CompareDialog::CompareDialog(const QString &name, const QString &hash, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CompareDialog)
{
    ui->setupUi(this);
    ui->labelName->setText(name);
    ui->labelHash->setText(hash);
}

CompareDialog::~CompareDialog()
{
    delete ui;
}
