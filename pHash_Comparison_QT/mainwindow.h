#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    newWindow *newWindowObj = NULL;

private slots:
    void on_btnOpenDB_clicked();
    void on_btnInsert_clicked();
    void on_btnSearch_clicked();
    void on_btnDelete_clicked();
    void on_btnEdit_clicked();
    void on_btnFindSimilar_clicked();
    void on_listWidget_itemClicked();
    void on_btnCreateDB_clicked();
    void on_btnShowSchema_clicked();
    void on_btnOpenImage_clicked();
    void on_method_clicked();

    void on_btnFindSimilar_toggled(bool checked);

private:
    void refreshList(const QString &filter = "");
    void updateDBLabel(const QString &path);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
