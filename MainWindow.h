#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Attivita.h"
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

    void on_addCategoria_clicked();

    void on_catSaveBtn_clicked();

    void on_catAnnullaBtn_clicked();

    void on_attivitaAnnullaBtn_clicked();

    void on_attivitaSaveBtn_clicked();

    void on_sottoactBtn_clicked();

    void on_catDeleteBtn_clicked();

private:
    Ui::MainWindow *ui;
    ManagerAttivita* model;
    QList<Attivita*> fasiTemp;
    Progetto* progettoInCreazione;
    bool modalitaFasi = false;

    void aggiornaCategorie();
    void aggiornaAttivita();
};

#endif // MAINWINDOW_H
