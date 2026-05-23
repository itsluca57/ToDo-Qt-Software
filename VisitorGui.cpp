#include "VisitorGui.h"
#include "ui_MainWindow.h"
#include "Promemoria.h"
#include "Evento.h"
#include "Ricorrente.h"
#include "Progetto.h"
#include <QTableWidgetItem>

VisitorGui::VisitorGui(Ui::MainWindow* interfacciaUi) : ui(interfacciaUi) {}

// Funzione helper privata per evitare di duplicare il codice dei campi comuni
void riempiCampiComuni(Ui::MainWindow* ui, int riga, Attivita* a, const QString& tipo, const QString& infoSpecifiche) {
    // 0. Nome
    ui->tableAttivita->setItem(riga, 0, new QTableWidgetItem(a->getNome()));

    // 1. Tipo
    ui->tableAttivita->setItem(riga, 1, new QTableWidgetItem(tipo));

    // 2. Data / Scadenza
    ui->tableAttivita->setItem(riga, 2, new QTableWidgetItem(a->getData().toString("dd/MM/yyyy hh:mm")));

    // 3. Categoria
    ui->tableAttivita->setItem(riga, 3, new QTableWidgetItem(a->getCategoria()));

    // 4. Priorità (Convertiamo l'enum in stringa leggibile)
    QString stringaPriorita;
    if(a->getPriorita() == Priorita::Alta) stringaPriorita = "Alta";
    else if(a->getPriorita() == Priorita::Media) stringaPriorita = "Media";
    else stringaPriorita = "Bassa";

    ui->tableAttivita->setItem(riga, 4, new QTableWidgetItem(stringaPriorita));

    // 5. Info Specifiche (Generate custom dai singoli metodi visit)
    ui->tableAttivita->setItem(riga, 5, new QTableWidgetItem(infoSpecifiche));

    // (Opzionale) Se l'attività è completata, potresti sbarrare il testo o cambiare colore alla riga
    if (a->isCompletata()) {
        for (int col = 0; col < 6; ++col) {
            ui->tableAttivita->item(riga, col)->setBackground(Qt::lightGray);
            // puoi anche aggiungere l'effetto sbarrato se vuoi
        }
    }
}

void VisitorGui::visit(Promemoria* p) {
    // Crea una nuova riga alla fine della tabella
    int riga = ui->tableAttivita->rowCount();
    ui->tableAttivita->insertRow(riga);

    // Prepariamo la stringa specifica per il Promemoria
    QString info = QString("Preavviso: %1 min").arg(p->getPreavviso());

    // Invochiamo l'helper per riempire la tabella
    riempiCampiComuni(ui, riga, p, "Promemoria", info);
}

void VisitorGui::visit(Evento* e) {
    int riga = ui->tableAttivita->rowCount();
    ui->tableAttivita->insertRow(riga);

    // Prepariamo la stringa specifica per l'Evento
    QString fineStr = e->getDataFine().toString("dd/MM/yyyy hh:mm");
    QString info = QString("Fine: %1 | Luogo: %2").arg(fineStr, e->getLuogo());

    riempiCampiComuni(ui, riga, e, "Evento", info);
}

void VisitorGui::visit(Ricorrente* r) {
    int riga = ui->tableAttivita->rowCount();
    ui->tableAttivita->insertRow(riga);

    // Convertiamo l'enum Frequenza in stringa
    QString freqStr;
    if(r->getFrequenza() == Frequenza::Settimanale) freqStr = "Settimanale";
    else if(r->getFrequenza() == Frequenza::Giornaliera) freqStr = "Giornaliera";
    else freqStr = "Mensile";

    QString fineRicStr = r->getFineRicorrenza().toString("dd/MM/yyyy");
    QString info = QString("Ogni: %1 (Fino al %2)").arg(freqStr, fineRicStr);

    riempiCampiComuni(ui, riga, r, "Ricorrente", info);
}

void VisitorGui::visit(Progetto* pr) {
    int riga = ui->tableAttivita->rowCount();
    ui->tableAttivita->insertRow(riga);

    // Per il progetto stampiamo il numero di sotto-attività (fasi) totali
    // Supponendo che la tua classe Progetto abbia un metodo per ottenere la lista delle fasi (es. getFasi())
    int numFasi = pr->getFasi().size();
    int faseAttuale = pr->getFaseAttuale();

    QString info = QString("Progetto (%1/%2 Fasi completate)").arg(faseAttuale).arg(numFasi);

    riempiCampiComuni(ui, riga, pr, "Progetto", info);
}