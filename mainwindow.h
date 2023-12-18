#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonOrder_clicked();
    void handleOrderAndReset();
    void on_lineEditPLZ_textChanged(const QString &arg1);
    void on_lineEditIBAN_textChanged(const QString &arg1);
    void updatePrice();
    void addShare();
    void clearShares();
    void removeShare();
    void checkOrderButtonState();

private:
    Ui::MainWindow *ui;
    void validatePLZ();
    void updateBankName();
    int calculatePrice();
    void sortLists();
    QStringList getUserShares();
    static const int pricePerMarket = 1000;
};

#endif // MAINWINDOW_H
