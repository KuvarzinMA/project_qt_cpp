#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QPixMap>
#include <QAbstractItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_add_row_clicked();

    void on_remove_row_clicked();

    void on_clients_table_clicked(const QModelIndex &index);

    void on_payments_button_clicked();


    void on_add_row_credit_clicked();

    void on_remove_row_credit_clicked();

    void on_add_row_payment_clicked();

    void on_remove_row_payment_clicked();

    void on_credits_table_clicked(const QModelIndex &index);

    void on_payments_table_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *clientsmodel;
    QSqlTableModel *creditsmodel;
    QSqlTableModel *paymentsmodel;
    QPixmap *picture;
    QAbstractItemModel *model;

    int row; //номерок активной строки да
};
#endif // MAINWINDOW_H