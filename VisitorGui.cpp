#include "VisitorGui.h"
#include "Promemoria.h"
#include "Evento.h"
#include "Ricorrente.h"
#include "Progetto.h"
#include "RegistroCategorie.h"
#include "VisitorInfo.h"
#include <QTableWidgetItem>
#include <QColor>
#include <QFont>
#include <QBrush>

VisitorGui::VisitorGui(QTableWidget* table, const RegistroCategorie* registro)
    : table(table), registroCategorie(registro) {}

void showBaseAttivita(QTableWidget* table, int riga, Attivita* a, const QString& info, const RegistroCategorie* registro) {

    table->setRowHeight(riga, 65);

    //Colonna 0: nome+checkbox
    QTableWidgetItem* itemNome = new QTableWidgetItem(a->getNome());
    itemNome->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    itemNome->setCheckState(a->isCompletata() ? Qt::Checked : Qt::Unchecked);
    itemNome->setData(Qt::UserRole, a->getId());

    QFont fontNome = itemNome->font();
    fontNome.setBold(true);
    fontNome.setPointSize(11);
    itemNome->setFont(fontNome);

    QString hexColore = registro->getCodiceColoreCategoria(a->getCategoria());
    QColor coloreCategoria = QColor::isValidColorName(hexColore) ? QColor(hexColore) : QColor(Qt::white);
    itemNome->setForeground(QBrush(coloreCategoria));
    table->setItem(riga, 0, itemNome);

    //Colonna 1: descrizione
    QTableWidgetItem* itemDesc = new QTableWidgetItem(a->getDescrizione());
    itemDesc->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    // Opzionale: rendiamo il testo della descrizione leggermente corsivo e grigio per distinguerlo
    QFont fontDesc = itemDesc->font();
    fontDesc.setItalic(true);
    itemDesc->setFont(fontDesc);
    table->setItem(riga, 1, itemDesc);

    //Colonna 2: scadenza
    QTableWidgetItem* itemData = new QTableWidgetItem(a->getData().toString("dd/MM/yyyy\nhh:mm"));
    itemData->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    table->setItem(riga, 2, itemData);

    //Colonna 3: priorità
    QString stringaPriorita;
    if(a->getPriorita() == Priorita::Alta) stringaPriorita = "Alta";
    else if(a->getPriorita() == Priorita::Media) stringaPriorita = "Media";
    else stringaPriorita = "Bassa";

    QTableWidgetItem* itemPrio = new QTableWidgetItem(stringaPriorita);
    itemPrio->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    table->setItem(riga, 3, itemPrio);

    //Colonna 4: info specifiche
    QTableWidgetItem* itemInfo = new QTableWidgetItem(info);
    itemInfo->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    table->setItem(riga, 4, itemInfo);

    //Attività scaduta
    if(a->isScaduta()) {
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem* item = table->item(riga, col);
            if (item) item->setBackground(QBrush(QColor(255, 0, 0, 25)));
        }
    }

    //Attività completata
    if(a->isCompletata()) {
        QFont fontBarrato;
        fontBarrato.setStrikeOut(true);

        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem* item = table->item(riga, col);
            if (item) {
                item->setBackground(QBrush(QColor(50, 50, 50)));
                item->setForeground(QBrush(Qt::gray));
                if (col != 4) item->setFont(fontBarrato);
            }
        }
    }
}

// Promemoria
void VisitorGui::visit(Promemoria* p) {
    int riga = table->rowCount();
    table->insertRow(riga);

    VisitorInfo infoVisitor;
    p->accept(&infoVisitor);

    showBaseAttivita(table, riga, p, infoVisitor.getInfo(), registroCategorie);

    //Cambio colore box per i promemoria entrati in preavviso
    if(p->getData().addSecs(-p->getPreavviso()*60) < QDateTime::currentDateTime() && !p->isScaduta() && !p->isCompletata()) {
        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem* item = table->item(riga, col);
            if (item) item->setBackground(QBrush(QColor(255, 165, 0, 25)));
        }
    }
}

// Evento
void VisitorGui::visit(Evento* e) {
    int riga = table->rowCount();
    table->insertRow(riga);

    VisitorInfo infoVisitor;
    e->accept(&infoVisitor);

    showBaseAttivita(table, riga, e, infoVisitor.getInfo(), registroCategorie);
}

// Ricorrente
void VisitorGui::visit(Ricorrente* r) {
    int riga = table->rowCount();
    table->insertRow(riga);

    VisitorInfo infoVisitor;
    r->accept(&infoVisitor);

    showBaseAttivita(table, riga, r, infoVisitor.getInfo(), registroCategorie);
}

//Progetto
void VisitorGui::visit(Progetto* pr) {
    int riga = table->rowCount();
    table->insertRow(riga);

    int numFasi = pr->getFasi().size();
    int faseAttuale = pr->getFaseAttuale();

    QString nomeSottoAttivita = "Nessuna fase attiva";
    QString infoSottoAttivita = "";
    QString descSottoAttivita = "";
    Attivita* sottoAttivita = nullptr; // <--- Teniamo traccia del puntatore alla sotto-attività

    // Prendo le info della sotto-attività corrente
    if (faseAttuale < numFasi) {
        sottoAttivita = pr->getFasi().at(faseAttuale);
        if (sottoAttivita != nullptr) {
            nomeSottoAttivita = sottoAttivita->getNome();
            descSottoAttivita = sottoAttivita->getDescrizione();

            VisitorInfo infoVisitor;
            sottoAttivita->accept(&infoVisitor);
            infoSottoAttivita = infoVisitor.getInfo() + "\n";
        }
    } else {
        nomeSottoAttivita = "Progetto Completato";
    }

    // Aggiungo dati di progetto
    QString infoAvanzamento = QString("Avanzamento Progetto: %1/%2").arg(faseAttuale).arg(numFasi);
    QString infoTotale = infoSottoAttivita + infoAvanzamento;

    // ==================== IL CAMBIO STRATEGICO ====================
    if (sottoAttivita != nullptr) {
        // 1. Passiamo la sotto-attività: imposta da sola DATA, SCADENZA (Rosso) e PRIORITÀ corrette!
        showBaseAttivita(table, riga, sottoAttivita, infoTotale, registroCategorie);

        // 2. GESTIONE PREAVVISO: Se la sotto-attività è un Promemoria, controlliamo se deve diventare gialla
        if (Promemoria* p = dynamic_cast<Promemoria*>(sottoAttivita)) {
            if (p->getData().addSecs(-p->getPreavviso()*60) < QDateTime::currentDateTime() && !p->isScaduta() && !p->isCompletata()) {
                for (int col = 0; col < 5; ++col) {
                    QTableWidgetItem* item = table->item(riga, col);
                    if (item) item->setBackground(QBrush(QColor(255, 165, 0, 25))); // Applica il tuo arancione/giallo
                }
            }
        }
    } else {
        // Fallback: se il progetto è completato o vuoto, usiamo il progetto intero
        showBaseAttivita(table, riga, pr, infoTotale, registroCategorie);
    }
    // ==============================================================

    // Completo il nome (Sovrascrive il testo ma mantiene i font e i colori di showBaseAttivita)
    QTableWidgetItem* itemNome = table->item(riga, 0);
    if (itemNome) {
        QString nomeCompleto = QString("%1\nProgetto: %2").arg(nomeSottoAttivita, pr->getNome());
        itemNome->setText(nomeCompleto);
    }

    // Completo la descrizione
    QTableWidgetItem* itemDesc = table->item(riga, 1);
    if (itemDesc) {
        QString descProgetto = pr->getDescrizione();
        QString descCompleta = descProgetto;

        // Unisco le descrizioni (Tua logica originale intatta)
        if (!descSottoAttivita.isEmpty()) {
            if (!descProgetto.isEmpty()) {
                descCompleta += ":\n";
            }
            descCompleta += descSottoAttivita;
        }
        itemDesc->setText(descCompleta);
    }

    // NOTA: Il blocco finale che avevi prima con itemNome->setData(...) NON SERVE PIÙ!
    // Avendo passato 'sottoAttivita' direttamente a showBaseAttivita alla riga 32,
    // l'ID salvato in UserRole e lo stato della checkbox sono già nativamente quelli corretti.
}