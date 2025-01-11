#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << QSqlDatabase::drivers();

    // подключение к бд
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setUserName("postgres");
    db.setPassword("1212");
    db.setDatabaseName("postgres");
    db.setHostName("localhost");
    if (db.open())
    {
        qDebug() << "Connected to database";
    }
    else {
        qDebug() << "Connected to database unsuccesfuly";
    }
    query = new QSqlQuery(db);
    query->exec("CREATE TABLE clients ("
                "ClientID SERIAL PRIMARY KEY,"
                "FullName VARCHAR(100) NOT NULL,"
                "DateOfBirth DATE NOT NULL,"
                "Email VARCHAR(100)"
                ");");

    query->exec("CREATE TABLE credits ("
                "CreditID SERIAL PRIMARY KEY,"
                "ClientID INT NOT NULL,"
                "CreditAmount NUMERIC(15, 2) NOT NULL,"
                "StartDate DATE NOT NULL,"
                "EndDate DATE NOT NULL,"
                "Status VARCHAR(20) NOT NULL,"
                "FOREIGN KEY (ClientID) REFERENCES Clients(ClientID) ON DELETE CASCADE"
                ");");

    query->exec("CREATE TABLE payments ("
                "PaymentID SERIAL PRIMARY KEY,"
                "CreditID INT NOT NULL,"
                "PaymentDate DATE NOT NULL,"
                "PaymentAmount NUMERIC(15, 2) NOT NULL,"
                "FOREIGN KEY (CreditID) REFERENCES Credits(CreditID) ON DELETE CASCADE"
                ");");

    // Настройка моделей
    clientsmodel = new QSqlTableModel(this, db);
    clientsmodel->setTable("clients");
    clientsmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    clientsmodel->select();
    ui->clients_table->setModel(clientsmodel);

    creditsmodel = new QSqlTableModel(this, db);
    creditsmodel->setTable("credits");
    creditsmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    creditsmodel->select();
    ui->credits_table->setModel(creditsmodel);

    paymentsmodel = new QSqlTableModel(this, db);
    paymentsmodel->setTable("payments");
    paymentsmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    paymentsmodel->select();

    // Установка изображения
    QPixmap pix(":/img/smile.png");
    if (!pix.isNull()) {
        int w = ui->label->width();
        int h = ui->label->height();
        ui->label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    } else {
        qDebug() << "Image not found: :/img/smile.png";
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}
//кнопка для добавления клиентов
void MainWindow::on_add_row_clicked()
{
    if (!clientsmodel->insertRow(clientsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << clientsmodel->lastError().text();
    }
}
//кнопка для удаления клиентов
void MainWindow::on_remove_row_clicked()
{
    if (row < 0 || row >= clientsmodel->rowCount()) {
        qDebug() << "Invalid row selected for deletion";
        return;
    }

    if (!clientsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << clientsmodel->lastError().text();
    }

    if (!clientsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << clientsmodel->lastError().text();
    }
}
//tableview для клиентов
void MainWindow::on_clients_table_clicked(const QModelIndex &index)
{
    row = index.row();
    int column = index.column();
    qDebug() << "Row: " << row << ", Column: " << column;
    if (row < clientsmodel->rowCount()) {
        QString fullName = clientsmodel->data(clientsmodel->index(row, 1)).toString();
        qDebug() << "Имя клиента из модели:" << fullName;
        ui->fullname_label->setText(fullName);
        QString birthday = clientsmodel->data(clientsmodel->index(row, 2)).toString();
        ui->birthday_label->setText(birthday);
        QString clientid = clientsmodel->data(clientsmodel->index(row, 0)).toString();
        ui->clientid_label->setText(clientid);
        QString email = clientsmodel->data(clientsmodel->index(row, 3)).toString();
        ui->email_label->setText(email);
    } else {
        qDebug() << "Неверный номер строки";
    }

}
//tableview для кредитов
void MainWindow::on_credits_table_clicked(const QModelIndex &index)
{
    row = index.row();
    int column = index.column();
    qDebug() << "Row: " << row << ", Column: " << column;
    if (row < creditsmodel->rowCount()) {
        QString creditid = creditsmodel->data(creditsmodel->index(row, 0)).toString();
        qDebug() << "Имя клиента из модели:" << creditid;
        ui->creditid_label->setText(creditid);
        QString clientid2 = creditsmodel->data(creditsmodel->index(row, 1)).toString();
        ui->clientid2_label->setText(clientid2);
        QString sumcredit = creditsmodel->data(creditsmodel->index(row, 2)).toString();
        ui->sumcredit_label->setText(sumcredit);
        QString startcredit = creditsmodel->data(creditsmodel->index(row, 3)).toString();
        ui->startcredit_label->setText(startcredit);
        QString endcredit = creditsmodel->data(creditsmodel->index(row, 4)).toString();
        ui->endcredit_label->setText(endcredit);
        QString statuscredit = creditsmodel->data(creditsmodel->index(row, 5)).toString();
        ui->status_label->setText(statuscredit);
    }
}

//tableview для платежей
void MainWindow::on_payments_table_clicked(const QModelIndex &index)
{
    row = index.row();
    int column = index.column();
    qDebug() << "Row: " << row << ", Column: " << column;
    if (row < paymentsmodel->rowCount()) {
        QString paymentid = paymentsmodel->data(paymentsmodel->index(row, 0)).toString();
        qDebug() << "Имя клиента из модели:" << paymentid;
        ui->paymentid_label->setText(paymentid);
        QString creditid2 = paymentsmodel->data(paymentsmodel->index(row, 1)).toString();
        ui->creditid2_label->setText(creditid2);
        QString datepayment = paymentsmodel->data(paymentsmodel->index(row, 2)).toString();
        ui->datepayment_label->setText(datepayment);
        QString sumpayment = paymentsmodel->data(paymentsmodel->index(row, 3)).toString();
        ui->sumpayment_label->setText(sumpayment);
    }
}
//кнопка для tableview в платежах
void MainWindow::on_payments_button_clicked()
{
    ui->payments_table->setModel(paymentsmodel);
    qDebug() << "Switched to 'payments' model";
}


//кнопка для добавления кредитов
void MainWindow::on_add_row_credit_clicked()
{
    if (!creditsmodel->insertRow(creditsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << creditsmodel->lastError().text();
    }
}


//кнопка для удаления кредитов
void MainWindow::on_remove_row_credit_clicked()
{
    if (row < 0 ) {
        qDebug() << "Invalid row selected for deletion";
        return;
    }

    if (!creditsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << creditsmodel->lastError().text();
    }

    if (!creditsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << creditsmodel->lastError().text();
    }
}



//кнопка для добавления платежей
void MainWindow::on_add_row_payment_clicked()
{
    if (!paymentsmodel->insertRow(paymentsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << paymentsmodel->lastError().text();
    }
}


//кнопка для удаления платежей
void MainWindow::on_remove_row_payment_clicked()
{


    if (!paymentsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << paymentsmodel->lastError().text();
    }

    if (!paymentsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << paymentsmodel->lastError().text();
    }
}



