#ifndef INTERFACCIA_H
#define INTERFACCIA_H

#include <QMainWindow>

class ManagerAttivita;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listCategorie_itemSelectionChanged();


    void on_addButton_clicked();

    void on_radioEvento_toggled(bool checked);

    void on_radioPromemoria_toggled(bool checked);

    void on_radioRicorrente_toggled(bool checked);

    void on_radioProgetto_toggled(bool checked);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    ManagerAttivita* model;
};

#endif // INTERFACCIA_H