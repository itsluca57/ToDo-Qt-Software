#include "MainWindow.h"
#include "CompareData.h"
#include "ComparePriorita.h"
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
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Setup
    ui->setupUi(this);
    model = new ManagerAttivita();
    ui->catDeleteBtn->hide();
    ui->catModBtn->hide();
    aggiornaCategorie();
    aggiornaAttivita();
    ui->tableAttivita->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableFasi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    progettoInCreazione = nullptr;

    //Signals
    connect(model, &ManagerAttivita::modAttivita, this, &MainWindow::aggiornaAttivita);
    connect(model->getRegistroCategorie(), &RegistroCategorie::modCategorie, this, &MainWindow::aggiornaCategorie);
    connect(model->getRegistroCategorie(), &RegistroCategorie::modCategorie, model, &ManagerAttivita::modAttivita);

    //Categoria di default
    QList<QListWidgetItem*> catDefault = ui->listCategorie->findItems("Qualsiasi", Qt::MatchExactly);
    ui->listCategorie->setCurrentItem(catDefault.first());

    //Ordinamento di default
    ui->scadenzaBtn->click();
    ui->scadenzaBtn->click();

    //Gestione tempo
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::aggiornaAttivita);

    timer->start(60000);

    //Shortcuts
    configuraShortcut();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

//Categoria selezionata
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
    if(ui->listCategorie->currentRow()!=0) {
        ui->catDeleteBtn->show();
        ui->catModBtn->show();
    }
    else {
        ui->catDeleteBtn->hide();
        ui->catModBtn->hide();
    }

    //Aggiorno attività per filtro categorie
    aggiornaAttivita();
}

//Attività aggiunta
void MainWindow::on_addAttivitaBtn_clicked()
{
    ui->mainStacked->setCurrentIndex(1);

    ui->radioMedia->setChecked(true);
    ui->radioPromemoria->setChecked(true);
    ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());

    ui->radioPromemoria->setEnabled(true);
    ui->radioEvento->setEnabled(true);
    ui->radioRicorrente->setEnabled(true);
    ui->radioProgetto->setEnabled(true);

    fasiTemp.clear();
}

//Selezione evento
void MainWindow::on_radioEvento_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Inizio:");
        ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->tipoStacked->setCurrentIndex(1);
    }
}

//Selezione promemoria
void MainWindow::on_radioPromemoria_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(0);
    }
}

//Selezione ricorrente
void MainWindow::on_radioRicorrente_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(2);
        ui->radioGiorn->setChecked(true);
        ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
    }
}

//Selezione progetto
void MainWindow::on_radioProgetto_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        aggiornaFasi();
        ui->tipoStacked->setCurrentIndex(3);
        ui->dataLabel->hide();
        ui->scadenzaDateTime->hide();
    }
}

//Aggiunta categoria
void MainWindow::on_addCategoria_clicked()
{
    ui->categorieStacked->setCurrentIndex(1);
    ui->redRadio->setChecked(true);
    ui->catLineEdit->setFocus();
}

//Refresh lista categorie
void MainWindow::aggiornaCategorie() {

    //Pulizia lista categoria per l'aggiunta di attività
    ui->categoriaComboBox->clear();

    //Salvataggio selezione corrente
    QString categoriaPrecedente;
    if (!catTarget.isEmpty()) {
        categoriaPrecedente = catTarget;
        catTarget.clear();
    } else if (ui->listCategorie->currentItem()) {
        categoriaPrecedente = ui->listCategorie->currentItem()->text();
    }

    //Reset
    ui->listCategorie->clear();
    QFont fontCategoria("Sitka", 14);

    QList<QString> listaNomi = model->getRegistroCategorie()->getListaCategorie();

    //Mantenimento della categoria Qualsiasi prima
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
        //Aggiunta a combobox
        if(nome == "Qualsiasi") ui->categoriaComboBox->addItem("Generale");
        else ui->categoriaComboBox->addItem(nome);
    }

    //Ripristino selezione
    if (!categoriaPrecedente.isEmpty()) {
        QList<QListWidgetItem*> elementiTrovati = ui->listCategorie->findItems(categoriaPrecedente, Qt::MatchExactly);

        if (!elementiTrovati.isEmpty()) {
            ui->listCategorie->setCurrentItem(elementiTrovati.first());
        }
    }
}

//Refresh lista attività (con filtro per categoria)
void MainWindow::aggiornaAttivita() {

    //Controllo l'avanzamento delle attività ricorrenti
    model->aggiornaRicorrenti();

    ui->tableAttivita->blockSignals(true);
    ui->tableAttivita->setRowCount(0);

    QList<Attivita*> listaOrdinata = model->getListaAttivita();

    QString ordineScadenza = ui->scadenzaBtn->property("ordinamento_scadenza").toString();
    QString ordinePriorita = ui->prioritaBtn->property("ordinamento_priorita").toString();

    std::unique_ptr<ComparatorAttivita> strategia = nullptr;
    bool crescente = true;

    //Seleziono strategia di ordinamento
    if(ordineScadenza == "crescente" || ordineScadenza == "decrescente") {
        strategia = std::make_unique<CompareData>();
        if(ordineScadenza == "decrescente") crescente = false;
    }
    else if(ordinePriorita == "crescente" || ordinePriorita == "decrescente") {
        strategia = std::make_unique<ComparePriorita>();
        if(ordinePriorita == "decrescente") crescente = false;
    }

    //Ordinamento
    if (strategia != nullptr) {
        std::sort(listaOrdinata.begin(), listaOrdinata.end(), [&](Attivita* a, Attivita* b) {
            if (crescente) {
                return strategia->compare(a, b);
            }
            else {
                return strategia->compare(b, a);
            }
        });
    }

    //Categoria selezionata
    QString categoriaSelezionata = "Qualsiasi";
    if (ui->listCategorie->currentItem()) {
        categoriaSelezionata = ui->listCategorie->currentItem()->text();
    }

    QString categoriaDaCercare = categoriaSelezionata;

    //Mappo "Qualsiasi" alla categoria "Generale"
    if (categoriaSelezionata == "Qualsiasi") categoriaDaCercare = "Generale";

    //Ricerca testuale
    QString testoRicerca = ui->searchLine->text().trimmed();

    //Visitor
    VisitorGui visitor(ui->tableAttivita, model->getRegistroCategorie());

    //Attività non completate
    for (Attivita* attivita : listaOrdinata) {
        if (attivita != nullptr && !attivita->isCompletata()) {
            //Applica il filtro: o le mostro tutte, o mostro solo quelle della categoria giusta
            if (categoriaDaCercare == "Generale" || attivita->getCategoria() == categoriaDaCercare) {
                //Ricerca testuale
                if (testoRicerca.isEmpty() || attivita->getNome().contains(testoRicerca, Qt::CaseInsensitive)) {
                    attivita->accept(&visitor);
                }
            }
        }
    }

    //Attività completate
    for (Attivita* attivita : listaOrdinata) {
        if (attivita != nullptr && attivita->isCompletata()) {
            //Applico il filtro
            if (categoriaDaCercare == "Generale" || attivita->getCategoria() == categoriaDaCercare) {
                //Ricerca testuale
                if (testoRicerca.isEmpty() || attivita->getNome().contains(testoRicerca, Qt::CaseInsensitive)) {
                    attivita->accept(&visitor);
                }
            }
        }
    }

    ui->tableAttivita->blockSignals(false);
}

//Salvataggio nuova categoria
void MainWindow::on_catSaveBtn_clicked()
{
    //Recupero il vecchio nome se in modalità modifica
    QString old = "";
    if (catMod && ui->listCategorie->currentItem()) {
        old = ui->listCategorie->currentItem()->text().trimmed();
    }

    //Selezione nome inserito nella LineEdit
    QString nomeCategoria = ui->catLineEdit->text().trimmed();

    if (nomeCategoria.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Il nome della categoria non può essere vuoto"));
        return;
    }

    //Verifica duplicati
    if ((!catMod || nomeCategoria != old) &&
        model->getRegistroCategorie()->esisteCategoria(nomeCategoria)) {
        QMessageBox::warning(this, tr("Errore"), tr("Questa categoria esiste già"));
        return;
    }

    //Selezione colore
    QString coloreSelezionato = "#D63031";
    if (ui->pinkRadio->isChecked())        coloreSelezionato = "#FFCCFF";
    else if (ui->yellowRadio->isChecked()) coloreSelezionato = "#FFFF33";
    else if (ui->blueRadio->isChecked())   coloreSelezionato = "#1F3D99";
    else if (ui->greenRadio->isChecked())  coloreSelezionato = "#2EB82E";
    else if (ui->orangeRadio->isChecked()) coloreSelezionato = "#FF9900";
    else if (ui->aquaRadio->isChecked())   coloreSelezionato = "#00FFFF";
    else if (ui->brownRadio->isChecked())  coloreSelezionato = "#BA884B";
    else if (ui->purpleRadio->isChecked()) coloreSelezionato = "#7A007A";

    //Salvo il nuovo nome per ricordare la selezione
    catTarget = nomeCategoria;

    if (catMod) {
        //Modifica
        model->getRegistroCategorie()->modCategoria(old, nomeCategoria, coloreSelezionato);
        catMod = false;
    } else {
        // Creazione normale
        model->getRegistroCategorie()->addCategoria(nomeCategoria, coloreSelezionato);
    }

    //Reset ui
    ui->catLineEdit->clear();
    ui->redRadio->setChecked(true);
    ui->categorieStacked->setCurrentIndex(0);
}

void MainWindow::aggiornaFasi() {
    ui->tableFasi->setRowCount(0);

    if (progettoInCreazione == nullptr) {
        return;
    }

    progettoInCreazione->ordinaAggiornaFasi();

    VisitorGui visitor(ui->tableFasi, model->getRegistroCategorie());

    const QList<Attivita*>& fasi = progettoInCreazione->getFasi();
    for (Attivita* fase : fasi) {
        if (fase != nullptr) {
            fase->accept(&visitor);
        }
    }
}

//Annullamento nuova categoria
void MainWindow::on_catAnnullaBtn_clicked()
{
    //Reset
    ui->catLineEdit->clear();
    ui->redRadio->setChecked(true);
    ui->categorieStacked->setCurrentIndex(0);
}

//Annullamento nuova attività/sotto attività
void MainWindow::on_attivitaAnnullaBtn_clicked()
{
    //Ritorno al progetto padre se stavo creando/modificando una fase
    if (modalitaFasi) {
        ui->radioProgetto->show();
        ui->prioritLabel->show();
        ui->radioAlta->show();
        ui->radioBassa->show();
        ui->radioMedia->show();
        ui->attivitaSaveBtn->setText("Salva");
        ui->categoriaLabel->show();
        ui->categoriaComboBox->show();

        //Ripristino campi progetto padre
        ui->radioProgetto->setChecked(true);
        ui->nomeLineEdit->setText(progettoInCreazione->getNome());
        ui->descrizioneLineEdit->setText(progettoInCreazione->getDescrizione());
        ui->scadenzaDateTime->setDateTime(progettoInCreazione->getData());

        int index = ui->categoriaComboBox->findText(progettoInCreazione->getCategoria());
        ui->categoriaComboBox->setCurrentIndex(index);

        if(progettoInCreazione->getPriorita() == Priorita::Alta) ui->radioAlta->setChecked(true);
        else if(progettoInCreazione->getPriorita() == Priorita::Media) ui->radioMedia->setChecked(true);
        else if(progettoInCreazione->getPriorita() == Priorita::Bassa) ui->radioBassa->setChecked(true);

        //Se si era in modifica modifica tengo disabilitata la modifica dei tipi
        if (idAttivitaModifica != -1) {
            ui->radioPromemoria->setEnabled(false);
            ui->radioEvento->setEnabled(false);
            ui->radioRicorrente->setEnabled(false);
            ui->radioProgetto->setEnabled(false);
            idFaseModifica = -1;
        }

        ui->mainStacked->setCurrentIndex(1);

        ui->deleteSottoActBtn->setEnabled(false);
        ui->modSottoActBtn->setEnabled(false);
        modalitaFasi = false;
    }
    else {
        //Annullamento da schermata principale attività
        if (idAttivitaModifica != -1) {
            //In modifica
            progettoInCreazione = nullptr;
        }
        else {
            //In creazione
            if (progettoInCreazione != nullptr) {
                delete progettoInCreazione;
                progettoInCreazione = nullptr;
            }
        }

        //Reset
        idAttivitaModifica = -1;

        ui->nomeLineEdit->clear();
        ui->descrizioneLineEdit->clear();
        ui->radioMedia->setChecked(true);
        ui->categoriaComboBox->setCurrentIndex(0);
        ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());
        ui->radioPromemoria->setChecked(true);

        //Riabilita la selezione dei tipi
        ui->radioPromemoria->setEnabled(true);
        ui->radioEvento->setEnabled(true);
        ui->radioRicorrente->setEnabled(true);
        ui->radioProgetto->setEnabled(true);

        ui->deleteAttivitaBtn->setEnabled(true);
        ui->modAttivitaBtn->setEnabled(true);
        ui->addAttivitaBtn->setEnabled(true);

        ui->mainStacked->setCurrentIndex(0);
    }
}

//Salvataggio nuova attività/sotto attività o modifica di attività
void MainWindow::on_attivitaSaveBtn_clicked()
{
    //Aggiunta sotto-attività
    if (modalitaFasi) {

        Attivita* fase = nullptr;

        //Non in modifica
        if(idFaseModifica == -1) {
            QString tipoFase;
            if(ui->radioPromemoria->isChecked()) tipoFase = "Promemoria";
            else if(ui->radioEvento->isChecked()) tipoFase = "Evento";
            else if(ui->radioRicorrente->isChecked()) tipoFase = "Ricorrente";

            fase = FactoryAttivita::create(tipoFase);
        }
        //In modifica
        else {
            for(Attivita* f : progettoInCreazione->getFasi()) {
                if(idFaseModifica == f->getId()) {
                    fase = f;
                    break;
                }
            }
        }

        QString nomeFase = ui->nomeLineEdit->text().trimmed();
        if (nomeFase.isEmpty()) {
            QMessageBox::warning(this, tr("Errore"), tr("Inserisci il nome della sotto-attività"));
            return;
        }

        if(!fase) return;

        fase->setNome(nomeFase);
        fase->setDescrizione(ui->descrizioneLineEdit->text().trimmed());
        fase->setData(ui->scadenzaDateTime->dateTime());
        fase->setCategoria(progettoInCreazione->getCategoria()); //Eredita categoria del padre
        fase->setPriorita(progettoInCreazione->getPriorita()); //Eredita priorità dal padre

        //Campi specifici sotto-attività
        if(Promemoria* promemoria = dynamic_cast<Promemoria*>(fase)) {
            promemoria->setPreavviso(ui->preavvisoSpinBox->value());
            ui->preavvisoSpinBox->setValue(15);
        }
        else if(Evento* evento = dynamic_cast<Evento*>(fase)) {
            evento->setDataFine(ui->dataFineDateTimeEdit->dateTime());
            evento->setLuogo(ui->luogoLineEdit->text().trimmed());
            ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
            ui->luogoLineEdit->clear();
        }
        else if(Ricorrente* ricorrente = dynamic_cast<Ricorrente*>(fase)) {
            Frequenza frequenza;
            if(ui->radioSett->isChecked()) frequenza = Frequenza::Settimanale;
            else if(ui->radioMens->isChecked()) frequenza = Frequenza::Mensile;
            else if(ui->radioGiorn->isChecked()) frequenza = Frequenza::Giornaliera;
            ricorrente->setFrequenza(frequenza);
            ricorrente->setFineRicorrenza(ui->fineRicorrenzaDateEdit->date());
            ui->radioGiorn->setChecked(true);
            ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
        }

        //Non in modifica
        if(idFaseModifica == -1){
            //Aggiunta fase
            progettoInCreazione->addFase(fase);
        }
        //In modifica
        else {
            idFaseModifica = -1;
        }

        modalitaFasi = false;

        //Ripristino schermata padre
        ui->radioProgetto->show();
        ui->prioritLabel->show();
        ui->radioAlta->show();
        ui->radioBassa->show();
        ui->radioMedia->show();
        ui->radioProgetto->setChecked(true);
        ui->attivitaSaveBtn->setText("Salva");
        ui->categoriaLabel->show();
        ui->categoriaComboBox->show();

        //Ricarica dei dati del padre
        ui->nomeLineEdit->setText(progettoInCreazione->getNome());
        ui->descrizioneLineEdit->setText(progettoInCreazione->getDescrizione());
        ui->scadenzaDateTime->setDateTime(progettoInCreazione->getData());
        ui->categoriaComboBox->setCurrentText(progettoInCreazione->getCategoria());

        if(progettoInCreazione->getPriorita() == Priorita::Alta) ui->radioAlta->setChecked(true);
        else if(progettoInCreazione->getPriorita() == Priorita::Media) ui->radioMedia->setChecked(true);
        else ui->radioBassa->setChecked(true);

        //Blocco dei pulsanti
        if (idAttivitaModifica != -1) {
            ui->radioPromemoria->setEnabled(false);
            ui->radioEvento->setEnabled(false);
            ui->radioRicorrente->setEnabled(false);
            ui->radioProgetto->setEnabled(false);
        }
        ui->deleteSottoActBtn->setEnabled(false);
        ui->modSottoActBtn->setEnabled(false);

        ui->dataLabel->hide();
        ui->scadenzaDateTime->hide();

        ui->attivitaSaveBtn->setEnabled(true);

        aggiornaFasi();
        return;
    }

    //Modifica di attività
    if (idAttivitaModifica != -1) {
        Attivita* a = model->findById(idAttivitaModifica);
        if (!a) return;

        a->setNome(ui->nomeLineEdit->text().trimmed());
        a->setDescrizione(ui->descrizioneLineEdit->text().trimmed());
        a->setCategoria(ui->categoriaComboBox->currentText());
        a->setData(ui->scadenzaDateTime->dateTime());

        if(ui->radioAlta->isChecked()) a->setPriorita(Priorita::Alta);
        else if(ui->radioMedia->isChecked()) a->setPriorita(Priorita::Media);
        else a->setPriorita(Priorita::Bassa);

        if(Promemoria* promemoria = dynamic_cast<Promemoria*>(a)) {
            promemoria->setPreavviso(ui->preavvisoSpinBox->value());
            ui->preavvisoSpinBox->setValue(15);
        }
        else if(Evento* evento = dynamic_cast<Evento*>(a)) {
            evento->setDataFine(ui->dataFineDateTimeEdit->dateTime());
            evento->setLuogo(ui->luogoLineEdit->text().trimmed());
            ui->dataFineDateTimeEdit->setDateTime(QDateTime::currentDateTime());
            ui->luogoLineEdit->clear();
        }
        else if(Ricorrente* ricorrente = dynamic_cast<Ricorrente*>(a)) {
            Frequenza frequenza;
            if(ui->radioSett->isChecked()) frequenza = Frequenza::Settimanale;
            else if(ui->radioMens->isChecked()) frequenza = Frequenza::Mensile;
            else if(ui->radioGiorn->isChecked()) frequenza = Frequenza::Giornaliera;
            ricorrente->setFrequenza(frequenza);
            ricorrente->setFineRicorrenza(ui->fineRicorrenzaDateEdit->date());
            ui->radioGiorn->setChecked(true);
            ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
        }

        aggiornaAttivita();

        //Reset
        idAttivitaModifica = -1;
        progettoInCreazione = nullptr;

        ui->nomeLineEdit->clear();
        ui->descrizioneLineEdit->clear();
        ui->categoriaComboBox->setCurrentIndex(0);
        ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());
        ui->radioMedia->setChecked(true);

        ui->radioPromemoria->setEnabled(true);
        ui->radioEvento->setEnabled(true);
        ui->radioRicorrente->setEnabled(true);
        ui->radioProgetto->setEnabled(true);

        ui->mainStacked->setCurrentIndex(0);
        //Ripristino pulsanti
        ui->modAttivitaBtn->setEnabled(false);
        ui->deleteAttivitaBtn->setEnabled(false);
        ui->addAttivitaBtn->setEnabled(true);

        return;
    }

    //Creazione nuova attività
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

    Attivita* nuovaAttivita = nullptr;
    if (tipo == "Progetto" && progettoInCreazione != nullptr) {
        nuovaAttivita = progettoInCreazione;
        progettoInCreazione = nullptr;
    } else {
        nuovaAttivita = FactoryAttivita::create(tipo);
    }

    nuovaAttivita->setNome(nome);
    nuovaAttivita->setDescrizione(descrizione);
    nuovaAttivita->setCategoria(categoria);
    nuovaAttivita->setPriorita(priorita);
    nuovaAttivita->setData(scadenza);
    nuovaAttivita->setCompletata(false);

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
        Frequenza frequenza;
        if(ui->radioSett->isChecked()) frequenza = Frequenza::Settimanale;
        else if(ui->radioMens->isChecked()) frequenza = Frequenza::Mensile;
        else if(ui->radioGiorn->isChecked()) frequenza = Frequenza::Giornaliera;
        ricorrente->setFrequenza(frequenza);
        ricorrente->setFineRicorrenza(ui->fineRicorrenzaDateEdit->date());
        ui->radioGiorn->setChecked(true);
        ui->fineRicorrenzaDateEdit->setDate(QDate::currentDate());
    }

    model->addAttivita(nuovaAttivita);

    ui->nomeLineEdit->clear();
    ui->descrizioneLineEdit->clear();
    ui->radioMedia->setChecked(true);
    ui->categoriaComboBox->setCurrentIndex(0);
    ui->scadenzaDateTime->setDateTime(QDateTime::currentDateTime());
    ui->radioPromemoria->setChecked(true);

    ui->mainStacked->setCurrentIndex(0);
}

//Aggiunta nuova sotto attività
void MainWindow::on_addSottoActBtn_clicked()
{
    if (!modalitaFasi && idAttivitaModifica == -1) {
        if(progettoInCreazione == nullptr) {
            QString nomePadre = ui->nomeLineEdit->text().trimmed();
            if (nomePadre.isEmpty()) {
                QMessageBox::warning(this, tr("Errore"), tr("Inserisci il nome del Progetto prima di aggiungere sottoattività"));
                return;
            }

            //Creazione progetto
            Attivita* temp = FactoryAttivita::create("Progetto");
            progettoInCreazione = dynamic_cast<Progetto*>(temp);
            progettoInCreazione->setNome(nomePadre);
            progettoInCreazione->setDescrizione(ui->descrizioneLineEdit->text().trimmed());
            progettoInCreazione->setCategoria(ui->categoriaComboBox->currentText());
            progettoInCreazione->setData(ui->scadenzaDateTime->dateTime());
            progettoInCreazione->setFaseAttuale(0);

            if(ui->radioAlta->isChecked()) progettoInCreazione->setPriorita(Priorita::Alta);
            else if(ui->radioMedia->isChecked()) progettoInCreazione->setPriorita(Priorita::Media);
            else if(ui->radioBassa->isChecked()) progettoInCreazione->setPriorita(Priorita::Bassa);
        }
    }
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

    ui->radioPromemoria->setEnabled(true);
    ui->radioEvento->setEnabled(true);
    ui->radioRicorrente->setEnabled(true);
    ui->radioProgetto->setEnabled(false);

    ui->dataLabel->show();
    ui->scadenzaDateTime->show();
}

//Eliminazione categoria
void MainWindow::on_catDeleteBtn_clicked()
{
    //Salvo il nome della categoria
    QString categoriaDaEliminare = ui->listCategorie->currentItem()->text();

    //Elimino categoria dal model
    model->getRegistroCategorie()->deleteCategoria(categoriaDaEliminare);

    //Cambio a categoria "Qualsiasi" le attività della categoria eliminiata
    for (Attivita* a : model->getListaAttivita()) {
        if (a != nullptr) {

            if (a->getCategoria() == categoriaDaEliminare) a->setCategoria("Generale");

            //Se è un progetto modifico anche le sotto attività
            if (Progetto* p = dynamic_cast<Progetto*>(a)) {
                for (Attivita* fase : p->getFasi()) {
                    if (fase != nullptr && fase->getCategoria() == categoriaDaEliminare) fase->setCategoria("Generale");
                }
            }
        }
    }

    //Ripristino interfaccia grafica

    ui->catDeleteBtn->hide();
    ui->catModBtn->hide();
    QList<QListWidgetItem*> catDefault = ui->listCategorie->findItems("Qualsiasi", Qt::MatchExactly);

    if (!catDefault.isEmpty()) {
        ui->listCategorie->setCurrentItem(catDefault.first());
    }
}

void MainWindow::on_tableAttivita_itemSelectionChanged()
{
    //Attivo pulsanti eliminazione e modifica attività
    ui->deleteAttivitaBtn->setEnabled(true);
    ui->modAttivitaBtn->setEnabled(true);
}


void MainWindow::on_tableAttivita_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        //Completo attività
        int idAttivita = item->data(Qt::UserRole).toInt();
        bool completata = (item->checkState() == Qt::Checked);
        model->setCompletata(idAttivita, completata);

        //Reset pulsanti
        ui->deleteAttivitaBtn->setEnabled(false);
        ui->modAttivitaBtn->setEnabled(false);
    }
}


void MainWindow::on_deleteAttivitaBtn_clicked()
{
    int rigaSelezionata = ui->tableAttivita->currentRow();

    //Recupero l'id
    QTableWidgetItem* itemNome = ui->tableAttivita->item(rigaSelezionata, 0);
    if (!itemNome) {
        return;
    }
    int idAttivita = itemNome->data(Qt::UserRole).toInt();

    //Box di conferma
    QMessageBox::StandardButton risposta;
    risposta = QMessageBox::question(this, "Conferma Eliminazione",
                                     "Sei sicuro di voler eliminare questa attività?",
                                     QMessageBox::Yes | QMessageBox::No);

    //Eliminazione
    if (risposta == QMessageBox::Yes) {
        model->removeAttivita(idAttivita);

        //Disabilito i pulsanti
        ui->deleteAttivitaBtn->setEnabled(false);
        ui->modAttivitaBtn->setEnabled(false);
    }
}


void MainWindow::on_modAttivitaBtn_clicked()
{
    ui->addAttivitaBtn->setEnabled(false);
    ui->deleteAttivitaBtn->setEnabled(false);

    int riga = ui->tableAttivita->currentRow();
    if (riga < 0) return;

    //Prendo l'attività selezionata
    int id = ui->tableAttivita->item(riga, 0)->data(Qt::UserRole).toInt();
    Attivita* a = model->findById(id);

    if (!a) return;

    idAttivitaModifica = id;

    ui->mainStacked->setCurrentIndex(1);

    // Uso i dati dell'attività per riempire i campi
    //Campi generici
    ui->nomeLineEdit->setText(a->getNome());
    ui->descrizioneLineEdit->setText(a->getDescrizione());
    ui->scadenzaDateTime->setDateTime(a->getData());

    if(a->getPriorita() == Priorita::Alta) ui->radioAlta->setChecked(true);
    else if(a->getPriorita() == Priorita::Media) ui->radioMedia->setChecked(true);
    else ui->radioBassa->setChecked(true);

    int index = ui->categoriaComboBox->findText(a->getCategoria());
    ui->categoriaComboBox->setCurrentIndex(index);

    if(dynamic_cast<Promemoria*>(a)) ui->radioPromemoria->setChecked(true);
    else if(dynamic_cast<Evento*>(a)) ui->radioEvento->setChecked(true);
    else if(dynamic_cast<Ricorrente*>(a)) ui->radioRicorrente->setChecked(true);
    else if(dynamic_cast<Progetto*>(a)) ui->radioProgetto->setChecked(true);

    //Blocco il cambio di tipo
    ui->radioPromemoria->setEnabled(false);
    ui->radioEvento->setEnabled(false);
    ui->radioRicorrente->setEnabled(false);
    ui->radioProgetto->setEnabled(false);

    //Campi specifici
    if(Promemoria* p = dynamic_cast<Promemoria*>(a)) {
        ui->tipoStacked->setCurrentIndex(0);

        ui->preavvisoSpinBox->setValue(p->getPreavviso());
    }
    else if(Evento* e = dynamic_cast<Evento*>(a)) {
        ui->tipoStacked->setCurrentIndex(1);

        ui->dataFineDateTimeEdit->setDateTime(e->getDataFine());
        ui->luogoLineEdit->setText(e->getLuogo());
    }
    else if(Ricorrente* r = dynamic_cast<Ricorrente*>(a)) {
        ui->tipoStacked->setCurrentIndex(2);

        if(r->getFrequenza() == Frequenza::Giornaliera) ui->radioGiorn->setChecked(true);
        else if(r->getFrequenza() == Frequenza::Settimanale) ui->radioSett->setChecked(true);
        else if(r->getFrequenza() == Frequenza::Mensile) ui->radioMens->setChecked(true);
        ui->fineRicorrenzaDateEdit->setDate(r->getFineRicorrenza());
    }
    else if(Progetto* p = dynamic_cast<Progetto*>(a)) {
        ui->tipoStacked->setCurrentIndex(3);

        progettoInCreazione = p;
        aggiornaFasi();
    }
}


void MainWindow::on_tableFasi_itemSelectionChanged()
{
    ui->deleteSottoActBtn->setEnabled(true);
    ui->modSottoActBtn->setEnabled(true);
}


void MainWindow::on_deleteSottoActBtn_clicked()
{
    //Recupero dell'id
    int rigaSelezionata = ui->tableFasi->currentRow();
    if (rigaSelezionata < 0 || progettoInCreazione == nullptr) return;

    QTableWidgetItem* itemNome = ui->tableFasi->item(rigaSelezionata, 0);
    if (!itemNome) return;
    int idFase = itemNome->data(Qt::UserRole).toInt();

    //Box di conferma
    QMessageBox::StandardButton risposta;
    risposta = QMessageBox::question(this, "Conferma Eliminazione Fase",
                                     "Sei sicuro di voler eliminare questa sotto-attività?",
                                     QMessageBox::Yes | QMessageBox::No);

    //Eliminazione
    if (risposta == QMessageBox::Yes) {

        progettoInCreazione->removeFase(idFase);
        aggiornaFasi();
        //Disabito il pulsante
        ui->deleteSottoActBtn->setEnabled(false);
        ui->modSottoActBtn->setEnabled(false);
    }
}


void MainWindow::on_modSottoActBtn_clicked()
{
    //Prendo l'id della sotto-attività
    int rigaSelezionata = ui->tableFasi->currentRow();
    if (rigaSelezionata < 0 || progettoInCreazione == nullptr) return;

    QTableWidgetItem* itemNome = ui->tableFasi->item(rigaSelezionata,0);
    if(!itemNome) return;
    int idFase = itemNome->data(Qt::UserRole).toInt();

    //Trovo la sotto-attività
    Attivita* a = nullptr;
    for (Attivita* fase : progettoInCreazione->getFasi()) {
        if (fase != nullptr && fase->getId() == idFase) {
            a = fase;
            break;
        }
    }

    if (!a) return;

    modalitaFasi = true;
    idFaseModifica = idFase;

    //Set dei campi da modificare
    //Campi generici
    ui->nomeLineEdit->setText(a->getNome());
    ui->descrizioneLineEdit->setText(a->getDescrizione());
    ui->scadenzaDateTime->setDateTime(a->getData());
    //Campi specifici
    if(Promemoria* p = dynamic_cast<Promemoria*>(a)) {
        ui->radioPromemoria->setChecked(true);
        ui->tipoStacked->setCurrentIndex(0);
        ui->preavvisoSpinBox->setValue(p->getPreavviso());
    }
    else if(Evento* e = dynamic_cast<Evento*>(a)) {
        ui->radioEvento->setChecked(true);
        ui->tipoStacked->setCurrentIndex(1);
        ui->dataFineDateTimeEdit->setDateTime(e->getDataFine());
        ui->luogoLineEdit->setText(e->getLuogo());
    }
    else if(Ricorrente* r = dynamic_cast<Ricorrente*>(a)) {
        ui->radioRicorrente->setChecked(true);
        ui->tipoStacked->setCurrentIndex(2);
        ui->fineRicorrenzaDateEdit->setDate(r->getFineRicorrenza());
        if(r->getFrequenza() == Frequenza::Giornaliera) ui->radioGiorn->setChecked(true);
        else if(r->getFrequenza() == Frequenza::Settimanale) ui->radioSett->setChecked(true);
        else if(r->getFrequenza() == Frequenza::Mensile) ui->radioMens->setChecked(true);
    }

    //Set della schermata della fase
    ui->radioProgetto->hide();
    ui->prioritLabel->hide();
    ui->radioAlta->hide();
    ui->radioBassa->hide();
    ui->radioMedia->hide();
    ui->attivitaSaveBtn->setText("Salva Fase");
    ui->categoriaLabel->hide();
    ui->categoriaComboBox->hide();

    ui->radioPromemoria->setEnabled(false);
    ui->radioEvento->setEnabled(false);
    ui->radioRicorrente->setEnabled(false);

    ui->dataLabel->show();
    ui->scadenzaDateTime->show();
}


void MainWindow::on_filterButton_clicked()
{
    ui->stackedFilters->setCurrentIndex(1);
}


void MainWindow::on_escFilterBtn_clicked()
{
    ui->stackedFilters->setCurrentIndex(0);
}

//Ordinamento per priorità
void MainWindow::on_prioritaBtn_clicked()
{
    QString statoAttuale = ui->prioritaBtn->property("ordinamento_priorita").toString();

    //Ordinamento non impostato
    if (statoAttuale.isEmpty() || statoAttuale == "nessuno") {
        ui->prioritaBtn->setStyleSheet("background-color:rgb(112,112,112);");
        ui->prioritaBtn->setIcon(QIcon(":/icone/freccia_su.png"));
        ui->prioritaBtn->setProperty("ordinamento_priorita", "decrescente");

        //Reset ordinamento scadenza
        ui->scadenzaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->scadenzaBtn->setIcon(QIcon());
        ui->scadenzaBtn->setProperty("ordinamento_scadenza", "nessuno");
    }
    //Ordinamento decrescente
    else if (statoAttuale == "decrescente") {
        ui->prioritaBtn->setIcon(QIcon(":/icone/freccia_giu.png"));
        ui->prioritaBtn->setProperty("ordinamento_priorita", "crescente");

        //Reset ordinamento scadenza
        ui->scadenzaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->scadenzaBtn->setIcon(QIcon());
        ui->scadenzaBtn->setProperty("ordinamento_scadenza", "nessuno");
    }
    //Ordinamento decrescente
    else if (statoAttuale == "crescente") {
        ui->prioritaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->prioritaBtn->setIcon(QIcon());
        ui->prioritaBtn->setProperty("ordinamento_priorita", "nessuno");
    }

    aggiornaAttivita();

}

//Ordinamento per scadenza
void MainWindow::on_scadenzaBtn_clicked()
{
    QString statoAttuale = ui->scadenzaBtn->property("ordinamento_scadenza").toString();

    //Ordinamento non impostato
    if (statoAttuale.isEmpty() || statoAttuale == "nessuno") {
        ui->scadenzaBtn->setStyleSheet("background-color:rgb(112,112,112);");
        ui->scadenzaBtn->setIcon(QIcon(":/icone/freccia_su.png"));
        ui->scadenzaBtn->setProperty("ordinamento_scadenza", "decrescente");

        //Reset ordinamento priorità
        ui->prioritaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->prioritaBtn->setIcon(QIcon());
        ui->prioritaBtn->setProperty("ordinamento_priorita", "nessuno");
    }
    //Ordinamento decrescente
    else if (statoAttuale == "decrescente") {
        ui->scadenzaBtn->setIcon(QIcon(":/icone/freccia_giu.png"));
        ui->scadenzaBtn->setProperty("ordinamento_scadenza", "crescente");

        //Reset ordinamento priorità
        ui->prioritaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->prioritaBtn->setIcon(QIcon());
        ui->prioritaBtn->setProperty("ordinamento_priorita", "nessuno");
    }
    //Ordinamento decrescente
    else if (statoAttuale == "crescente") {
        ui->scadenzaBtn->setStyleSheet("background-color:rgb(40,40,40);");
        ui->scadenzaBtn->setIcon(QIcon());
        ui->scadenzaBtn->setProperty("ordinamento_scadenza", "nessuno");
    }

    aggiornaAttivita();
}

void MainWindow::on_searchLine_textChanged(const QString &arg1)
{
    aggiornaAttivita();
}

//Importa da file JSON
void MainWindow::on_actionImporta_triggered() {
    //Percorso di default
    QString cartellaDocumenti = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString percorsoFile = QFileDialog::getOpenFileName(
        this,
        tr("Seleziona il file da importare"),
        cartellaDocumenti,
        tr("File JSON (*.json);;Tutti i file (*)")
        );

    if (percorsoFile.isEmpty()) return;

    //Box per scelta sovrascrizione
    QMessageBox box(this);
    box.setWindowTitle(tr("Modalità Importazione"));
    box.setText(tr("Come desideri importare le attività contenute nel file?"));

    QPushButton* btnSostituisci = box.addButton(tr("Sostituisci Correnti"), QMessageBox::DestructiveRole);
    QPushButton* btnAggiungi = box.addButton(tr("Unisci"), QMessageBox::AcceptRole);
    QPushButton* btnAnnulla = box.addButton(QMessageBox::Cancel);

    box.exec();

    if (box.clickedButton() == btnAnnulla) {
        return;
    }

    bool sovrascrivi = false;
    if (box.clickedButton() == btnSostituisci) {
        sovrascrivi = true;
    } else if (box.clickedButton() == btnAggiungi) {
        sovrascrivi = false;
    }

    //Caricamento
    if (model->load(percorsoFile, sovrascrivi)) {
        aggiornaAttivita();
        QMessageBox::information(this, tr("Successo"), tr("Dati importati con successo!"));
    } else {
        QMessageBox::critical(this, tr("Errore"), tr("Impossibile leggere il file selezionato."));
    }
}

//Esporta da file JSON
void MainWindow::on_actionEsporta_triggered() {
    //Nome e percorso di default del salvataggio
    QString cartellaDocumenti = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString percorsoDefault = cartellaDocumenti + "/todo_save.json";

    //Finestra di sistema
    QString percorsoScelto = QFileDialog::getSaveFileName(
        this,
        tr("Esporta Attività"),
        percorsoDefault,
        tr("File JSON (*.json);;Tutti i file (*)")
        );

    if (percorsoScelto.isEmpty()) return;

    //Controllo dell'estensione
    QFileInfo fileInfo(percorsoScelto);

    if (fileInfo.suffix().toLower() != "json") {
        QString cartella = fileInfo.absolutePath();
        QString nomePulito = fileInfo.completeBaseName();

        percorsoScelto = cartella + "/" + nomePulito + ".json";
    }

    //Funzione save
    if (model->save(percorsoScelto)) {
        QMessageBox::information(this, tr("Successo"), tr("Dati esportati correttamente"));
    } else {
        QMessageBox::critical(this, tr("Errore"), tr("Impossibile salvare il file"));
    }
}


void MainWindow::on_catModBtn_clicked()
{
    QString nomeCat = ui->listCategorie->currentItem()->text();

    //Cambio pagina e set valori
    ui->categorieStacked->setCurrentIndex(1);
    ui->catLineEdit->setText(nomeCat);

    if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#D63031") ui->redRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#FFCCFF") ui->pinkRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#FFFF33") ui->yellowRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#1F3D99") ui->blueRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#2EB82E") ui->greenRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#FF9900") ui->orangeRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#00FFFF") ui->aquaRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#BA884B") ui->brownRadio->setChecked(true);
    else if(model->getRegistroCategorie()->getCodiceColoreCategoria(nomeCat) == "#7A007A") ui->purpleRadio->setChecked(true);

    catMod = true;
}

void MainWindow::configuraShortcut() {

    //Tasto invio per conferma
    connect(ui->catLineEdit, &QLineEdit::returnPressed, ui->catSaveBtn, &QPushButton::click);
    connect(ui->nomeLineEdit, &QLineEdit::returnPressed, ui->attivitaSaveBtn, &QPushButton::click);

    //Tasto esc per annulla
    QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, [this]() {
        //Se from categorie
        if(ui->categorieStacked->currentIndex() == 1) {
            ui->catAnnullaBtn->click();
        }
        //Se form attivita
        if(ui->mainStacked->currentIndex() == 1) {
            ui->attivitaAnnullaBtn->click();
        }
    });

    //CTRL+N per creazione attività
    QShortcut* newShortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(newShortcut, &QShortcut::activated, this, [this]() {
        //Se schermata principale
        if(ui->mainStacked->currentIndex() == 0) {
            ui->addAttivitaBtn->click();
        }
    });

    //CTRL+S per salvare
    ui->actionEsporta->setShortcut(QKeySequence("Ctrl+S"));


}
