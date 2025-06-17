#ifndef NEWWINDOW_H
#define NEWWINDOW_H
#include <QPixmap>
#include <QWidget>

namespace Ui {
class newWindow;
}

class newWindow : public QWidget
{
    Q_OBJECT

public:
    explicit newWindow(QWidget *parent = nullptr);
    ~newWindow();
    void setImage(const QPixmap &pixmap);
    void setPHashText(const QString & str);

private:
    Ui::newWindow *ui;
};

#endif // NEWWINDOW_H
