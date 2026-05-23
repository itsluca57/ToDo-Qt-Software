#include "MainWindow.h"
#include "Evento.h"
#include "Promemoria.h"
#include "Ricorrente.h"
#include "Progetto.h"
#include "VisitorGui.h"
#include "ui_MainWindow.h"
#include "ManagerAttivita.h"

#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new ManagerAttivita();
    ui->catDeleteBtn->hide();
    aggiornaCategorie();
    aggiornaAttivita();
    progettoInCreazione = nullptr;

    QList<QListWidgetItem*> catDefault = ui->listCategorie->findItems("Qualsiasi", Qt::MatchExactly);
    ui->listCategorie->setCurrentItem(catDefault.first());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

void MainWindow::on_listCategorie_itemSelectionChanged()
{
    for (int i = 0; i < ui->listCategorie->count(); ++i) {
        QListWidgetItem* item = ui->listCategorie->item(i);
        QFont font = item->font();

        if (item->isSelected()) {
            font.setBold(true);
        } else {
            font.setBold(false);
        }
        item->setFont(font);
    }

    //Pulsante eliminazione
    if(ui->listCategorie->currentRow()!=0)ui->catDeleteBtn->show();
    else ui->catDeleteBtn->hide();
}
void MainWindow::on_addButton_clicked()
{
    ui->mainStacked->setCurrentIndex(1);

    ui->radioMedia->setChecked(true);
    ui->radioPromemoria->setChecked(true);
    ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());

    fasiTemp.clear();
}


void MainWindow::on_radioEvento_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Inizio:");
        ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->tipoStacked->setCurrentIndex(1);
    }
}


void MainWindow::on_radioPromemoria_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(0);
    }
}


void MainWindow::on_radioRicorrente_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(2);
        ui->radioGiorn->setChecked(true);
        ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
    }
}


void MainWindow::on_radioProgetto_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(3);
    }
}

void MainWindow::on_addCategoria_clicked()
{
    ui->categorieStacked->setCurrentIndex(1);
    ui->redRadio->setChecked(true);
    ui->catLineEdit->setFocus();
}

void MainWindow::aggiornaCategorie() {
    //Salvataggio selezione corrente
    QString categoriaPrecedente;
    if (ui->listCategorie->currentItem()) {
        categoriaPrecedente = ui->listCategorie->currentItem()->text();
    }

    //Reset
    ui->listCategorie->clear();
    QFont fontCategoria("Sitka", 14);

    QList<QString> listaNomi = model->getRegistroCategorie()->getListaCategorie();

    //Mantenimento di qualsiasi prima
    if (listaNomi.contains("Qualsiasi")) {
        listaNomi.removeAll("Qualsiasi");
        listaNomi.prepend("Qualsiasi");
    }

    //Ciclo su categorie
    for (const QString& nome : std::as_const(listaNomi)) {

        QListWidgetItem* item = new QListWidgetItem(nome);

        //Formattazione
        item->setFont(fontCategoria);
        item->setTextAlignment(Qt::AlignCenter);

        //Colore
        QString hexColore = model->getRegistroCategorie()->getCodiceColoreCategoria(nome);
        QColor colore(hexColore);

        if (colore.isValid()) {
            item->setForeground(colore);
        }

        ui->listCategorie->addItem(item);
    }

    //Ripristino selezione
    if (!categoriaPrecedente.isEmpty()) {
        QList<QListWidgetItem*> elementiTrovati = ui->listCategorie->findItems(categoriaPrecedente, Qt::MatchExactly);

        if (!elementiTrovati.isEmpty()) {
            ui->listCategorie->setCurrentItem(elementiTrovati.first());
        }
    }
}

void MainWindow::aggiornaAttivita() {
    // 1. Blocca temporaneamente i segnali visivi per migliorare le prestazioni durante il reset
    ui->tableAttivita->setSortingEnabled(false);

    // 2. Svuota completamente le righe della tabella grafica
    ui->tableAttivita->setRowCount(0);

    // 5. Istanzia il Visitor passandogli la UI (l'oggetto 'ui' di MainWindow)
    VisitorGui visitor(ui);

    // 6. Cicla su ogni attività e delega il popolamento della riga al Visitor polimorfico
    for (Attivita* attivita : model->getListaAttivita()) {
        if (attivita != nullptr) {
            // Il metodo accept chiamerà automaticamente il 'visit' corretto (Promemoria, Evento, ecc.)
            attivita->accept(&visitor);
        }
    }

    // 7. Opzionale: Regola automaticamente la larghezza delle colonne in base al contenuto
    ui->tableAttivita->resizeColumnsToContents();

}

void MainWindow::on_catSaveBtn_clicked()
{

    //Selezione nome
    QString nomeCategoria = ui->catLineEdit->text().trimmed();

    //Controlli
    if (nomeCategoria.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Il nome della categoria non può essere vuoto"));
        return;
    }

    if (model->getRegistroCategorie()->esisteCategoria(nomeCategoria)) {
        QMessageBox::warning(this, tr("Errore"), tr("Questa categoria esiste già"));
        return;
    }

    //Selezione colore
    QString coloreSelezionato = "#D63031";

    if (ui->pinkRadio->isChecked()) {
        coloreSelezionato = "#FFCCFF";
    } else if (ui->yellowRadio->isChecked()) {
        coloreSelezionato = "#FFFF33";
    } else if (ui->blueRadio->isChecked()) {
        coloreSelezionato = "#1F3D99";
    } else if (ui->greenRadio->isChecked()) {
        coloreSelezionato = "#2EB82E";
    } else if (ui->orangeRadio->isChecked()) {
        coloreSelezionato = "#FF9900";
    } else if (ui->aquaRadio->isChecked()) {
        coloreSelezionato = "#00FFFF";
    } else if (ui->brownRadio->isChecked()) {
        coloreSelezionato = "#BA884B";
    } else if (ui->purpleRadio->isChecked()) {
        coloreSelezionato = "#7A007A";
    }

    //Salvataggio
    model->getRegistroCategorie()->addCategoria(nomeCategoria, coloreSelezionato);

    //Aggiorna lista
    aggiornaCategorie();

    //Reset input
    ui->catLineEdit->clear();
    ui->redRadio->setChecked(true);

    ui->categorieStacked->setCurrentIndex(0);
}


void MainWindow::on_catAnnullaBtn_clicked()
{
    //Reset
    ui->catLineEdit->clear();
    ui->redRadio->setChecked(true);
    ui->categorieStacked->setCurrentIndex(0);
}


void MainWindow::on_attivitaAnnullaBtn_clicked()
{
    //Reset
    if (modalitaFasi) {
        ui->radioProgetto->show();
        ui->prioritLabel->show();
        ui->radioAlta->show();
        ui->radioBassa->show();
        ui->radioMedia->show();
        ui->categoriaLabel->show();
        ui->categoriaComboBox->show();
        ui->attivitaSaveBtn->setText("Salva");

        //Ripristino campi progetto
        ui->radioProgetto->setChecked(true);
        ui->nomeLineEdit->setText(progettoInCreazione->getNome());
        ui->descrizioneLineEdit->setText(progettoInCreazione->getDescrizione());
        ui->scadenzaDateTime->setDateTime(progettoInCreazione->getData());
        int index = ui->categoriaComboBox->findText(progettoInCreazione->getCategoria());
        ui->categoriaComboBox->setCurrentIndex(index);
        if(progettoInCreazione->getPriorita() == Priorita::Alta) ui->radioAlta->setChecked(true);
        else if(progettoInCreazione->getPriorita() == Priorita::Media) ui->radioMedia->setChecked(true);
        else ui->radioBassa->setChecked(true);

        ui->mainStacked->setCurrentIndex(1);

        modalitaFasi = false;
    }
    else {
        //Gestione garbage
        if (progettoInCreazione != nullptr) {
            delete progettoInCreazione;
            progettoInCreazione = nullptr;
        }

        //Reset
        ui->nomeLineEdit->clear();
        ui->descrizioneLineEdit->clear();
        ui->radioMedia->setChecked(true);
        ui->categoriaComboBox->setCurrentIndex(0);
        ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());
        ui->radioPromemoria->setChecked(true);

        ui->mainStacked->setCurrentIndex(0);
    }


}


void MainWindow::on_attivitaSaveBtn_clicked()
{
    if(modalitaFasi == false) {

        //Acquisizione dati generici
        QString nome = ui->nomeLineEdit->text().trimmed();
        QString descrizione = ui->descrizioneLineEdit->text().trimmed();
        QString categoria = ui->categoriaComboBox->currentText();
        QDateTime scadenza = ui->scadenzaDateTime->dateTime();

        Priorita priorita;
        if(ui->radioAlta->isChecked()) priorita = Priorita::Alta;
        else if(ui->radioMedia->isChecked()) priorita = Priorita::Media;
        else priorita = Priorita::Bassa;

        QString tipo;
        if(ui->radioPromemoria->isChecked()) tipo = "Promemoria";
        else if(ui->radioEvento->isChecked()) tipo = "Evento";
        else if(ui->radioRicorrente->isChecked()) tipo = "Ricorrente";
        else if(ui->radioProgetto->isChecked()) tipo = "Progetto";

        if (nome.isEmpty()) {
            QMessageBox::warning(this, tr("Errore"), tr("Il nome dell'attività non può essere vuoto."));
            return;
        }

        //Creazione
        Attivita* nuovaAttivita = nullptr;
        if (tipo == "Progetto" && progettoInCreazione != nullptr) {
            //Progetto con sotto fasi
            nuovaAttivita = progettoInCreazione;
            progettoInCreazione = nullptr;
        } else {
            //Attività normale
            nuovaAttivita = FactoryAttivita::create(tipo);
        }

        //Set campi generici
        nuovaAttivita->setNome(nome);
        nuovaAttivita->setDescrizione(descrizione);
        nuovaAttivita->setCategoria(categoria);
        nuovaAttivita->setPriorita(priorita);
        nuovaAttivita->setData(scadenza);
        nuovaAttivita->setCompletata(false);

        //Set campi specifici
        if(Promemoria* promemoria = dynamic_cast<Promemoria*>(nuovaAttivita)) {
            promemoria->setPreavviso(ui->preavvisoSpinBox->value());
            ui->preavvisoSpinBox->setValue(15);
        }
        else if(Evento* evento = dynamic_cast<Evento*>(nuovaAttivita)) {
            evento->setDataFine(ui->dataFineDateTimeEdit->dateTime());
            evento->setLuogo(ui->luogoLineEdit->text().trimmed());
            ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
            ui->luogoLineEdit->clear();
        }
        else if(Ricorrente* ricorrente = dynamic_cast<Ricorrente*>(nuovaAttivita)) {
            Frequenza frequenza = ui->radioSett->isChecked() ? Frequenza::Settimanale :
                                      (ui->radioMens->isChecked() ? Frequenza::Mensile : Frequenza::Giornaliera);
            ricorrente->setFrequenza(frequenza);
            ricorrente->setFineRicorrenza(ui->fineRicorrenzaDateEdit->dateTime());
            ui->radioGiorn->setChecked(true);
            ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
        }

        model->addAttivita(nuovaAttivita);

        aggiornaAttivita();

        //Reset
        ui->nomeLineEdit->clear();
        ui->descrizioneLineEdit->clear();
        ui->radioMedia->setChecked(true);
        ui->categoriaComboBox->setCurrentIndex(0);
        ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());
        ui->radioPromemoria->setChecked(true);

        ui->mainStacked->setCurrentIndex(0);
    }
    else {

        //Nome e controllo
        QString nomeFase = ui->nomeLineEdit->text().trimmed();
        if (nomeFase.isEmpty()) {
            QMessageBox::warning(this, tr("Errore"), tr("Inserisci il nome della sotto-attività"));
            return;
        }

        //Set campi generici
        QString tipoFase;
        if(ui->radioPromemoria->isChecked()) tipoFase = "Promemoria";
        else if(ui->radioEvento->isChecked()) tipoFase = "Evento";
        else if(ui->radioRicorrente->isChecked()) tipoFase = "Ricorrente";

        Attivita* nuovaFase = FactoryAttivita::create(tipoFase);
        nuovaFase->setNome(nomeFase);
        nuovaFase->setDescrizione(ui->descrizioneLineEdit->text().trimmed());
        nuovaFase->setData(ui->scadenzaDateTime->dateTime());
        nuovaFase->setCategoria(progettoInCreazione->getCategoria()); // Eredita la categoria del padre

        //Set ampi specifici
        if(Promemoria* promemoria = dynamic_cast<Promemoria*>(nuovaFase)) {
            promemoria->setPreavviso(ui->preavvisoSpinBox->value());
            ui->preavvisoSpinBox->setValue(15);
        }
        else if(Evento* evento = dynamic_cast<Evento*>(nuovaFase)) {
            evento->setDataFine(ui->dataFineDateTimeEdit->dateTime());
            evento->setLuogo(ui->luogoLineEdit->text().trimmed());
            ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
            ui->luogoLineEdit->clear();
        }
        else if(Ricorrente* ricorrente = dynamic_cast<Ricorrente*>(nuovaFase)) {
            Frequenza frequenza = ui->radioSett->isChecked() ? Frequenza::Settimanale :
                                      (ui->radioMens->isChecked() ? Frequenza::Mensile : Frequenza::Giornaliera);
            ricorrente->setFrequenza(frequenza);
            ricorrente->setFineRicorrenza(ui->fineRicorrenzaDateEdit->dateTime());
            ui->radioGiorn->setChecked(true);
            ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
        }

        //Aggiunta fase
        progettoInCreazione->addFase(nuovaFase);

        //Reset
        ui->radioProgetto->show();
        ui->prioritLabel->show();
        ui->radioAlta->show();
        ui->radioBassa->show();
        ui->radioMedia->show();
        ui->radioProgetto->setChecked(true);
        ui->attivitaSaveBtn->setText("Salva");
        ui->categoriaLabel->show();
        ui->categoriaComboBox->show();

        //Caricamento campi progetto
        ui->nomeLineEdit->setText(progettoInCreazione->getNome());
        ui->descrizioneLineEdit->setText(progettoInCreazione->getDescrizione());
        ui->scadenzaDateTime->setDateTime(progettoInCreazione->getData());
        ui->categoriaComboBox->setCurrentText(progettoInCreazione->getCategoria());

        if(progettoInCreazione->getPriorita() == Priorita::Alta) ui->radioAlta->setChecked(true);
        else if(progettoInCreazione->getPriorita() == Priorita::Media) ui->radioMedia->setChecked(true);
        else ui->radioBassa->setChecked(true);

        modalitaFasi = false;
    }
}

void MainWindow::on_sottoactBtn_clicked()
{
    if (!modalitaFasi) {
        QString nomePadre = ui->nomeLineEdit->text().trimmed();
        if (nomePadre.isEmpty()) {
            QMessageBox::warning(this, tr("Errore"), tr("Inserisci il nome del Progetto prima di aggiungere sottoattività"));
            return;
        }

        //Creazione progetto
        progettoInCreazione = new Progetto();
        progettoInCreazione->setNome(nomePadre);
        progettoInCreazione->setDescrizione(ui->descrizioneLineEdit->text().trimmed());
        progettoInCreazione->setCategoria(ui->categoriaComboBox->currentText());
        progettoInCreazione->setData(ui->scadenzaDateTime->dateTime());
        progettoInCreazione->setFaseAttuale(0);

        if(ui->radioAlta->isChecked()) progettoInCreazione->setPriorita(Priorita::Alta);
        else if(ui->radioMedia->isChecked()) progettoInCreazione->setPriorita(Priorita::Media);
        else if(ui->radioBassa->isChecked()) progettoInCreazione->setPriorita(Priorita::Bassa);

        //Modalita fasi
        modalitaFasi = true;

        //Set schermata per sotto attività
        ui->radioProgetto->hide();
        ui->prioritLabel->hide();
        ui->radioAlta->hide();
        ui->radioBassa->hide();
        ui->radioMedia->hide();
        ui->radioPromemoria->setChecked(true);
        ui->attivitaSaveBtn->setText("Aggiungi");
        ui->categoriaLabel->hide();
        ui->categoriaComboBox->hide();

        //Reset
        ui->nomeLineEdit->clear();
        ui->descrizioneLineEdit->clear();
    }
}


void MainWindow::on_catDeleteBtn_clicked()
{
    model->getRegistroCategorie()->deleteCategoria(ui->listCategorie->currentItem()->text());
    aggiornaCategorie();
    ui->catDeleteBtn->hide();
    QList<QListWidgetItem*> catDefault = ui->listCategorie->findItems("Qualsiasi", Qt::MatchExactly);
    ui->listCategorie->setCurrentItem(catDefault.first());
}