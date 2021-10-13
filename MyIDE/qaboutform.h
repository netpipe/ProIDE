#ifndef QABOUTFORM_H
#define QABOUTFORM_H

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
class QAboutForm;
}

class QAboutForm : public QWidget
{
    Q_OBJECT

public:
    explicit QAboutForm(QWidget *parent = nullptr);
    ~QAboutForm();

private:
    void keyPressEvent(QKeyEvent * event)		override;

    Ui::QAboutForm *ui;
};

#endif // QABOUTFORM_H
