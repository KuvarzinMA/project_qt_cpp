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
    ui->payments_table->setModel(paymentsmodel);

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

// Проверка данных перед добавлением
bool MainWindow::validateClientData( const QString &email)
{

    qDebug() << "Checking Email:" << email;

    QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

    // Проверяем email
    if (!email.isEmpty() && !emailRegex.match(email).hasMatch()) {
        qDebug() << "Invalid email format.";
        return false;
    }

    return true; // Все данные валидны
}

// Метод проверки данных кредита
bool MainWindow::validateCreditData(const QString &startDate, const QString &endDate)
{

    QDate start = QDate::fromString(startDate, "yyyy-MM-dd");
    QDate end = QDate::fromString(endDate, "yyyy-MM-dd");

    if (!start.isValid() || !end.isValid() || start > end) {
        qDebug() << "Invalid credit dates.";
        return false;
    }


    return true;
}



// Добавление строки в таблицу клиентов
void MainWindow::on_add_row_clicked()
{
    if (!clientsmodel->insertRow(clientsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << clientsmodel->lastError().text();
    }
    qDebug() << "New client row added.";
}


// Удаление строки из таблицы
void MainWindow::on_remove_row_clicked()
{
    if (row < 0 || row >= clientsmodel->rowCount()) {
        qDebug() << "Invalid row selected for deletion";
        return;
    }

    if (!clientsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << clientsmodel->lastError().text();
    } else if (!clientsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << clientsmodel->lastError().text();
    } else {
        qDebug() << "Row removed successfully.";
    }
}

// Обработка кликов по любой таблице
void MainWindow::on_table_clicked(QAbstractItemModel *model, const QModelIndex &index, QVector<QLabel *> labels)
{
    row = index.row();
    int column = index.column();
    qDebug() << "Row: " << row << ", Column: " << column;

    if (row < 0 || row >= model->rowCount()) {
        qDebug() << "Invalid row selected.";
        return;
    }

    for (int i = 0; i < labels.size(); ++i) {
        QString data = model->data(model->index(row, i)).toString();
        labels[i]->setText(data);
    }
}

// Пример использования для клиентов
void MainWindow::on_clients_table_clicked(const QModelIndex &index)
{
    on_table_clicked(clientsmodel, index, {ui->clientid_label, ui->fullname_label, ui->birthday_label, ui->email_label});
}

// Пример использования для кредитов
void MainWindow::on_credits_table_clicked(const QModelIndex &index)
{
    on_table_clicked(creditsmodel, index, {ui->creditid_label, ui->clientid2_label, ui->sumcredit_label, ui->startcredit_label, ui->endcredit_label, ui->status_label});
}

// Пример использования для платежей
void MainWindow::on_payments_table_clicked(const QModelIndex &index)
{
    on_table_clicked(paymentsmodel, index, {ui->paymentid_label, ui->creditid2_label, ui->datepayment_label, ui->sumpayment_label});
}

// Добавление проверки на вводимые данные в базу данных
void MainWindow::on_add_row_credit_clicked()
{
    if (!creditsmodel->insertRow(creditsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << creditsmodel->lastError().text();
    } else {
        qDebug() << "Row added successfully.";
    }
}

// Удаление строки из кредитов
void MainWindow::on_remove_row_credit_clicked()
{
    if (row < 0 || row >= creditsmodel->rowCount()) {
        qDebug() << "Invalid row selected for deletion.";
        return;
    }

    if (!creditsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << creditsmodel->lastError().text();
    } else if (!creditsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << creditsmodel->lastError().text();
    } else {
        qDebug() << "Row removed successfully.";
    }
}

// Добавление строки в платежи
void MainWindow::on_add_row_payment_clicked()
{
    if (!paymentsmodel->insertRow(paymentsmodel->rowCount())) {
        qDebug() << "Failed to insert row:" << paymentsmodel->lastError().text();
    } else {
        qDebug() << "Row added successfully.";
    }
}

// Удаление строки из платежей
void MainWindow::on_remove_row_payment_clicked()
{
    if (row < 0 || row >= paymentsmodel->rowCount()) {
        qDebug() << "Invalid row selected for deletion.";
        return;
    }

    if (!paymentsmodel->removeRow(row)) {
        qDebug() << "Failed to remove row:" << paymentsmodel->lastError().text();
    } else if (!paymentsmodel->submitAll()) {
        qDebug() << "Failed to submit changes:" << paymentsmodel->lastError().text();
    } else {
        qDebug() << "Row removed successfully.";
    }
}


void MainWindow::on_save_clients_clicked()
{
    for (int row = 0; row < clientsmodel->rowCount(); ++row) {

        QString email = clientsmodel->data(clientsmodel->index(row, 3)).toString();

        if (!validateClientData(email)) {
            qDebug() << "Validation failed for row" << row << ". Data will not be saved.";
            ui->label_saved_clients->setText("Не правильный email");
            return;
        }
    }

    if (!clientsmodel->submitAll()) {
        qDebug() << "Failed to save client data:" << clientsmodel->lastError().text();

    } else {
        ui->label_saved_clients->setText("Успешно!");
        qDebug() << "Client data saved successfully.";
    }
}

void MainWindow::on_save_credits_clicked()
{
    for (int row = 0; row < creditsmodel->rowCount(); ++row) {

        QString startDate = creditsmodel->data(creditsmodel->index(row, 3)).toString();
        QString endDate = creditsmodel->data(creditsmodel->index(row, 4)).toString();


        if (!validateCreditData(startDate, endDate)) {
            qDebug() << "Validation failed for row" << row << ". Data will not be saved.";
            ui->label_saved_credits->setText("Не правильно указана дата");
            return;
        }
    }

    if (!creditsmodel->submitAll()) {
        qDebug() << "Failed to save credit data:" << creditsmodel->lastError().text();
    } else {
        qDebug() << "Credit data saved successfully.";
        ui->label_saved_credits->setText("Успешно!");
    }
}
