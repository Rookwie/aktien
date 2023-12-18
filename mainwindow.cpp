#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qDebug() << "MainWindow constructor called";

    // rot
    ui->pushButtonQuit->setStyleSheet("QPushButton { background-color: #DB4437; color: white; }");

    // blau
    ui->pushButtonOrder->setStyleSheet("QPushButton { background-color: #4285F4; color: white; }");

    // add to green
    ui->pushButtonAddTo->setStyleSheet("QPushButton { background-color: #0F9D58; color: white; }");

    // remove from red
    ui->pushButtonRemoveFrom->setStyleSheet("QPushButton { background-color: #DB4437; color: white; }");

    if (ui->listWidgetAvailibleSharesList->count() == 0) {
        QStringList availableShares = {
            "Dow Jones", "NASDAQ", "S&P 500",
            "DAX", "FTSE 100", "CAC 40",
            "Nikkei 225", "Hang Seng", "Shanghai Composite",
            "ASX 200", "BSE Sensex", "KOSPI",
            "Euro Stoxx 50", "TSX Composite", "IBEX 35"
        };
        ui->listWidgetAvailibleSharesList->addItems(availableShares);
        qDebug() << "Available shares added";
    } else {
        qDebug() << "Available shares already present, count:" << ui->listWidgetAvailibleSharesList->count();
    }

    ui->labelPPMOutput->setText("0 €");

    QRegularExpression plzRegex("^[0-9]{5}$");
    auto plzValidator = new QRegularExpressionValidator(plzRegex, this);
    ui->lineEditPLZ->setValidator(plzValidator);

    ui->pushButtonOrder->setEnabled(false);

    connect(ui->pushButtonAddTo, &QPushButton::clicked, this, &MainWindow::addShare);
    connect(ui->pushButtonClearAll, &QPushButton::clicked, this, &MainWindow::clearShares);
    connect(ui->pushButtonRemoveFrom, &QPushButton::clicked, this, &MainWindow::removeShare);
    connect(ui->lineEditName, &QLineEdit::textChanged, this, &MainWindow::checkOrderButtonState);
    connect(ui->comboBoxGender, &QComboBox::currentIndexChanged, this, &MainWindow::checkOrderButtonState);
    connect(ui->lineEditPLZ, &QLineEdit::textChanged, this, &MainWindow::checkOrderButtonState);
    connect(ui->lineEditIBAN, &QLineEdit::textChanged, this, &MainWindow::checkOrderButtonState);
    connect(ui->radioButtonMonthly, &QRadioButton::toggled, this, &MainWindow::checkOrderButtonState);
    connect(ui->radioButtonYearly, &QRadioButton::toggled, this, &MainWindow::checkOrderButtonState);
    connect(ui->radioButtonMonthly, &QRadioButton::toggled, this, &MainWindow::updatePrice);
    connect(ui->radioButtonYearly, &QRadioButton::toggled, this, &MainWindow::updatePrice);


}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_pushButtonOrder_clicked() {
    QString details;
    details += "Name: " + ui->lineEditName->text() + "\n";
    details += "PLZ: " + ui->lineEditPLZ->text() + "\n";
    details += "Geschlecht: " + ui->comboBoxGender->currentText() + "\n";
    details += "IBAN: " + ui->lineEditIBAN->text() + "\n";
    QString zahlungsweise = ui->radioButtonMonthly->isChecked() ? "monatlich" : "jährlich";
    details += "Zahlungsweise: " + zahlungsweise + "\n";
    details += "Gewählte Aktienmärkte: " + getUserShares().join(", ") + "\n";
    details += "Gesamtpreis: " + ui->labelPPMOutput->text();

    QMessageBox::information(this, "Bestellübersicht", details);

    handleOrderAndReset();
}

void MainWindow::handleOrderAndReset() {
    if (!ui->checkBoxSaveData->isChecked()) {
        ui->lineEditName->clear();
        ui->lineEditPLZ->clear();
        ui->comboBoxGender->setCurrentIndex(0);
        ui->lineEditIBAN->clear();
        ui->listWidgetUserSharesList->clear();
        ui->listWidgetAvailibleSharesList->clear();
        QStringList availableShares = {
            "Dow Jones", "NASDAQ", "S&P 500",
            "DAX", "FTSE 100", "CAC 40",
            "Nikkei 225", "Hang Seng", "Shanghai Composite",
            "ASX 200", "BSE Sensex", "KOSPI",
            "Euro Stoxx 50", "TSX Composite", "IBEX 35"
        };
        ui->listWidgetAvailibleSharesList->addItems(availableShares);
        ui->labelPPMOutput->setText("0 €");
        ui->tabWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_lineEditPLZ_textChanged(const QString &arg1) {
    validatePLZ();
}

void MainWindow::on_lineEditIBAN_textChanged(const QString &arg1) {
    updateBankName();
}

void MainWindow::checkOrderButtonState() {
    bool isAllDataEntered = !ui->lineEditName->text().isEmpty() &&
                            ui->comboBoxGender->currentIndex() != 0 &&
                            ui->lineEditPLZ->hasAcceptableInput() &&
                            !ui->lineEditIBAN->text().isEmpty() &&
                            (ui->radioButtonMonthly->isChecked() || ui->radioButtonYearly->isChecked());
    ui->pushButtonOrder->setEnabled(isAllDataEntered);
}

void MainWindow::updatePrice() {
    const int pricePerMarket = 1000; // Redefine the constant in this scope
    int numberOfMarkets = ui->listWidgetUserSharesList->count();

    if (numberOfMarkets == 0) {
        ui->labelPPMOutput->setText("0 €");
        return;
    }

    if (ui->lineEditName->text().isEmpty() ||
        ui->lineEditPLZ->text().isEmpty() ||
        ui->comboBoxGender->currentIndex() == 0 ||
        ui->lineEditIBAN->text().isEmpty() ||
        (!ui->radioButtonMonthly->isChecked() && !ui->radioButtonYearly->isChecked())) {
        ui->labelPPMOutput->setText("0 €");
        return;
    }

    int price = calculatePrice();
    QString priceText = QString::number(price) + " €";

    if (ui->radioButtonYearly->isChecked() && numberOfMarkets > 0) {
        const double discountRate = 0.05;
        int discountAmount = static_cast<int>(pricePerMarket * numberOfMarkets * discountRate);
        priceText += QString(" ( -%1 € )").arg(discountAmount);
    }

    ui->labelPPMOutput->setText(priceText);
}



void MainWindow::validatePLZ() {
    QString plz = ui->lineEditPLZ->text();
    if (plz.length() != 5 || !plz.toLongLong()) {
        // PLZ ist ungültig
    }
}

void MainWindow::updateBankName() {
    QString iban = ui->lineEditIBAN->text();

    QMap<QString, QString> bankMapping;
    bankMapping["DE1234567890"] = "Bob the Builder Bank";
    bankMapping["DE8765432165"] = "Musterbank AG";
    bankMapping["DE8710070000"] = "Deutsche Bank";
    bankMapping["DE8710040000"] = "Commerzbank";
    bankMapping["DE8750060400"] = "DZ Bank";
    bankMapping["DE8710010010"] = "Postbank";


    QString ibanSuffix = iban.mid(4, 8);


    QString bankName = "Unbekannte Bank";

    for (auto i = bankMapping.begin(); i != bankMapping.end(); ++i) {
        if (i.key().mid(4, 8) == ibanSuffix) {
            bankName = i.value();
            break;
        }
    }

    ui->labelBankNameOutput->setText(bankName);
}


int MainWindow::calculatePrice() {
    const int pricePerMarket = 1000;
    int numberOfMarkets = ui->listWidgetUserSharesList->count();
    int totalPrice = numberOfMarkets * pricePerMarket;

    if (ui->radioButtonYearly->isChecked() && numberOfMarkets > 0) {
        const double discountRate = 0.05;
        int discountAmount = static_cast<int>(totalPrice * discountRate);
        totalPrice -= discountAmount;
    }

    return totalPrice;
}



void MainWindow::addShare() {
    QList<QListWidgetItem *> items = ui->listWidgetAvailibleSharesList->selectedItems();
    if (!items.isEmpty()) {
        foreach(QListWidgetItem *item, items) {
            ui->listWidgetUserSharesList ->addItem(item->text());
            delete ui->listWidgetAvailibleSharesList->takeItem(ui->listWidgetAvailibleSharesList->row(item));
        }
        updatePrice();
    }
    checkOrderButtonState();
}

void MainWindow::clearShares() {
    ui->listWidgetAvailibleSharesList->addItems(getUserShares());
    ui->listWidgetUserSharesList ->clear();
    updatePrice();
    checkOrderButtonState();
}

void MainWindow::removeShare() {
    QList<QListWidgetItem *> items = ui->listWidgetUserSharesList ->selectedItems();
    if (!items.isEmpty()) {
        foreach(QListWidgetItem *item, items) {
            ui->listWidgetAvailibleSharesList->addItem(item->text());
            delete ui->listWidgetUserSharesList ->takeItem(ui->listWidgetUserSharesList ->row(item));
        }
        updatePrice();
    }
    checkOrderButtonState();
}

QStringList MainWindow::getUserShares() {
    QStringList shares;
    for (int i = 0; i < ui->listWidgetUserSharesList ->count(); ++i) {
        shares << ui->listWidgetUserSharesList ->item(i)->text();
    }
    return shares;
}


