#include "ManagerAttivita.h"
#include "Progetto.h"
#include "Ricorrente.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <qtimer.h>

ManagerAttivita::ManagerAttivita() : nextId(0) {
    categorie.addCategoria("Qualsiasi", "#FFFFFF");

    //Timer per aggiornamento ricorrenze
    QTimer *timerRicorrenze = new QTimer(this);
    connect(timerRicorrenze, &QTimer::timeout, this, &ManagerAttivita::aggiornaRicorrenti);

    // Controlla ogni 60 secondi (60000 millisecondi)
    timerRicorrenze->start(60000);
}

ManagerAttivita::~ManagerAttivita() {
    qDeleteAll(listaAttivita);
    listaAttivita.clear();
}

Attivita* ManagerAttivita::findById(int id) const {
    for (Attivita* a : listaAttivita) {
        //Trovo attività
        if (a->getId() == id) return a;

        //Trovo sotto-attività
        if (Progetto* p = dynamic_cast<Progetto*>(a)) {
            for (Attivita* fase : p->getFasi()) {
                if (fase->getId() == id) {
                    return p;
                }
            }
        }
    }
    return nullptr;
}

void ManagerAttivita::addAttivita(Attivita* nuova) {
    if (!nuova) return;

    if (nuova->getId() <= 0 || findById(nuova->getId()) != nullptr) {
        nuova->setId(nextId++);
    } else if (nuova->getId() >= nextId) {
        nextId = nuova->getId() + 1;
    }

    if (!listaAttivita.contains(nuova)) {
        listaAttivita.append(nuova);
    }

    Progetto* p = dynamic_cast<Progetto*>(nuova);
    if (p) {
        const QList<Attivita*>& fasi = p->getFasi();
        for (Attivita* fase : fasi) {

            if (fase->getId() <= 0 || findById(fase->getId()) != nullptr) {
                fase->setId(nextId++);
            } else if (fase->getId() >= nextId) {
                nextId = fase->getId() + 1;
            }
        }
    }

    emit modAttivita();
}

void ManagerAttivita::removeAttivita(int id) {
    Attivita* a = findById(id);
    if (!a) return;

    int index = listaAttivita.indexOf(a);

    if (index != -1) {
        listaAttivita.removeAt(index);
        delete a;
        emit modAttivita();
    }
}

void ManagerAttivita::setCompletata(int id, bool completata) {
    bool trovataComeFase = false;

    //Cerco come sotto-attività (per non completare il progetto intero)
    for (Attivita* a : std::as_const(listaAttivita)) {
        if (Progetto* p = dynamic_cast<Progetto*>(a)) {
            //Scorro le sotto-attivita per trovare l'id
            for (Attivita* sottoAttivita : p->getFasi()) {
                if (sottoAttivita && sottoAttivita->getId() == id) {

                    sottoAttivita->setCompletata(completata);

                    if (Ricorrente* ric = dynamic_cast<Ricorrente*>(sottoAttivita)) {

                        if (completata && QDateTime::currentDateTime() >= ric->getData()) {

                            //Calcolo la data della prossima occorrenza
                            QDateTime prossimaData = ric->getData();
                            if (ric->getFrequenza() == Frequenza::Giornaliera) {
                                prossimaData = prossimaData.addDays(1);
                            } else if (ric->getFrequenza() == Frequenza::Settimanale) {
                                prossimaData = prossimaData.addDays(7);
                            } else if (ric->getFrequenza() == Frequenza::Mensile) {
                                prossimaData = prossimaData.addMonths(1);
                            }

                            //Controllo di non aver superato la data di fine ricorrenza
                            if (prossimaData.date() <= ric->getFineRicorrenza()) {

                                //Creo la nuova occorrenza
                                Ricorrente* nuovaOccorrenza = new Ricorrente(*ric);
                                nuovaOccorrenza->setId(getNextId());

                                nuovaOccorrenza->setData(prossimaData);
                                nuovaOccorrenza->setCompletata(false);

                                //Aggiungo al progetto di appartenenza
                                p->addFase(nuovaOccorrenza);
                            }
                        }
                    }

                    //Ordino e ricalcolo faseAttuale
                    p->ordinaAggiornaFasi();

                    //Controllo se il progetto è terminato
                    if (p->getFaseAttuale() >= p->getFasi().size()) {
                        p->setCompletata(true);
                    } else {
                        p->setCompletata(false);
                    }

                    trovataComeFase = true;
                    break;
                }
            }
            if (trovataComeFase) break;
        }
    }

    //Cerco come attività
    if (!trovataComeFase) {
        Attivita* attivitaTrovata = findById(id);
        if (attivitaTrovata) {
            attivitaTrovata->setCompletata(completata);
        }
    }

    emit modAttivita();
}

//Salvataggio su file JSON
bool ManagerAttivita::save(const QString& nomeFile) const {
    QJsonArray array;
    for (const Attivita* a : listaAttivita) {
        array.append(a->toJson());
    }

    QFile file(nomeFile);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(array);
    file.write(doc.toJson());
    return true;
}

//Caricamento da file JSON
bool ManagerAttivita::load(const QString& nomeFile, bool sovrascrivi) {
    QFile file(nomeFile);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray dati = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(dati);
    QJsonArray array = doc.array();

    this->blockSignals(true);

    //Reset dati
    if (sovrascrivi) {
        qDeleteAll(listaAttivita);
        listaAttivita.clear();
        nextId = 1;
        categorie.clear();
    }

    //Caricamento attività
    for (const QJsonValue& valore : std::as_const(array)) {
        QJsonObject obj = valore.toObject();
        Attivita* a = factory.create(obj);
        if (a) {
            addAttivita(a);

            //Controllo categoria presente
            QString cat = a->getCategoria();

            if (!cat.isEmpty() && !categorie.esisteCategoria(cat)) {
                QString coloreLibero = categorie.getColoreLibero();
                categorie.addCategoria(cat, coloreLibero);
            }
        }
    }

    this->blockSignals(false);
    emit modAttivita();

    return true;
}

void ManagerAttivita::aggiornaRicorrenti() {
    bool modificheEffettuate = false;
    QDateTime adesso = QDateTime::currentDateTime();

    for (Attivita* a : std::as_const(listaAttivita)) {

        //Controllo attività ricorrenti
        if (Ricorrente* r = dynamic_cast<Ricorrente*>(a)) {
            r->checkAvanzamento();
        }

        //Controllo sotto-attività ricorrenti
        if (Progetto* p = dynamic_cast<Progetto*>(a)) {
            auto fasiAttuali = p->getFasi();
            bool progettoModificato = false;

            for (Attivita* sottoAttivita : std::as_const(fasiAttuali)) {

                if (sottoAttivita && sottoAttivita->isCompletata()) {
                    if (Ricorrente* ric = dynamic_cast<Ricorrente*>(sottoAttivita)) {

                        //Controllo data scaduta
                        if (adesso >= ric->getData()) {

                            //Calcolo data prossima occorrenza
                            QDateTime prossimaData = ric->getData();
                            if (ric->getFrequenza() == Frequenza::Giornaliera) {
                                prossimaData = prossimaData.addDays(1);
                            } else if (ric->getFrequenza() == Frequenza::Settimanale) {
                                prossimaData = prossimaData.addDays(7);
                            } else if (ric->getFrequenza() == Frequenza::Mensile) {
                                prossimaData = prossimaData.addMonths(1);
                            }

                            //Controllo data fine ricorrenza
                            if (prossimaData.date() <= ric->getFineRicorrenza()) {

                                //Controllo unicità per non duplicare
                                bool giaPresente = false;
                                for (Attivita* f : p->getFasi()) {
                                    if (f && f->getData() == prossimaData && f->getNome() == ric->getNome()) {
                                        giaPresente = true;
                                        break;
                                    }
                                }

                                //Se è scaduta e non esiste viene creata
                                if (!giaPresente) {
                                    Ricorrente* nuovaOccorrenza = new Ricorrente(*ric);
                                    nuovaOccorrenza->setId(getNextId());
                                    nuovaOccorrenza->setData(prossimaData);
                                    nuovaOccorrenza->setCompletata(false);

                                    p->addFase(nuovaOccorrenza);
                                    progettoModificato = true;
                                    modificheEffettuate = true;
                                }
                            }
                        }
                    }
                }
            }

            // Se abbiamo aggiunto sotto-attività, aggiorniamo il progetto padre
            if (progettoModificato) {
                p->ordinaAggiornaFasi();

                if (p->getFaseAttuale() >= p->getFasi().size()) {
                    p->setCompletata(true);
                } else {
                    p->setCompletata(false);
                }
            }
        }
    }

    // Se il timer ha inserito nuove sotto-attività, aggiorniamo l'interfaccia
    if (modificheEffettuate) {
        emit modAttivita();
    }
}