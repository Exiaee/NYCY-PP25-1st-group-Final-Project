
#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>

namespace Ui {
class CompareDialog;
}

class CompareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompareDialog(const QString &name, const QString &hash, QWidget *parent = nullptr);
    ~CompareDialog();

private:
    Ui::CompareDialog *ui;
};

#endif // COMPAREDIALOG_H
