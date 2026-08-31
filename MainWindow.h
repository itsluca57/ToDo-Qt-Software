#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Attivita.h"
#include <QMainWindow>
#include <qtablewidget.h>

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

    void on_addAttivitaBtn_clicked();

    void on_radioEvento_toggled(bool checked);

    void on_radioPromemoria_toggled(bool checked);

    void on_radioRicorrente_toggled(bool checked);

    void on_radioProgetto_toggled(bool checked);

    void on_addCategoria_clicked();

    void on_catSaveBtn_clicked();

    void on_catAnnullaBtn_clicked();

    void on_attivitaAnnullaBtn_clicked();

    void on_attivitaSaveBtn_clicked();

    void on_addSottoActBtn_clicked();

    void on_catDeleteBtn_clicked();

    void on_tableAttivita_itemSelectionChanged();

    void on_tableAttivita_itemChanged(QTableWidgetItem *item);

    void on_deleteAttivitaBtn_clicked();

    void on_modAttivitaBtn_clicked();

    void on_tableFasi_itemSelectionChanged();

    void on_deleteSottoActBtn_clicked();

    void on_modSottoActBtn_clicked();

    void on_filterButton_clicked();

    void on_escFilterBtn_clicked();

    void on_prioritaBtn_clicked();

    void on_scadenzaBtn_clicked();

    void on_searchLine_textChanged(const QString &arg1);

    void on_actionImporta_triggered();

    void on_actionEsporta_triggered();

    void on_catModBtn_clicked();

private:
    //Attributi
    Ui::MainWindow *ui;
    ManagerAttivita* model;
    QList<Attivita*> fasiTemp;
    Progetto* progettoInCreazione;
    bool modalitaFasi = false;
    int idAttivitaModifica = -1; //-1 -> Creazione
    int idFaseModifica = -1;
    bool catMod = false;
    QString catTarget;

    //Metodi
    void aggiornaCategorie();
    void aggiornaAttivita();
    void aggiornaFasi();
    void configuraShortcut();
};

#endif // MAINWINDOW_H
