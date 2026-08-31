#include "Progetto.h"
#include "FactoryAttivita.h"
#include <qjsonarray.h>

Progetto::Progetto(): Attivita(), faseAttuale(0) {}

Progetto::Progetto(int id,
                   const QString& nome,
                   const QString& descrizione,
                   const QDateTime& data,
                   Priorita priorita,
                   const QString& categoria,
                   const QList<Attivita*>& fasi,
                   int faseAttuale,
                   bool completata):
                    Attivita(id, nome, descrizione, data, priorita, categoria, completata),
                    fasi(fasi),
                    faseAttuale(faseAttuale) {}

Progetto::~Progetto() {
    qDeleteAll(fasi);
}

//Visitor
void Progetto::accept(VisitorAttivita* visitor) {
    visitor->visit(this);
}

QJsonObject Progetto::toJson() const {
    QJsonObject json = Attivita::toJson();
    json["tipo"] = "Progetto";
    json["faseAttuale"] = faseAttuale;

    QJsonArray arrayFasi;
    for (const Attivita* fase : fasi) {
        if (fase) {
            QJsonObject faseJson = fase->toJson();
            arrayFasi.append(faseJson);
        }
    }
    json["fasi"] = arrayFasi;

    return json;
}

void Progetto::fromJson(const QJsonObject& json) {
    Attivita::fromJson(json);

    if (json.contains("faseAttuale")) {
        faseAttuale = json["faseAttuale"].toInt();
    }

    if (json.contains("fasi")) {
        qDeleteAll(fasi);
        fasi.clear();

        QJsonArray arrayFasi = json["fasi"].toArray();
        for (QJsonArray::const_iterator it=arrayFasi.begin(); it!=arrayFasi.end(); ++it) {
            QJsonObject faseJson = (*it).toObject();

            Attivita* nuovaFase = FactoryAttivita::create(faseJson);

            if (nuovaFase) {
                fasi.append(nuovaFase);
            }
        }
    }
}

void Progetto::addFase(Attivita* fase) {
    if (fase && !fasi.contains(fase)) {
        fasi.append(fase);
    }
}

void Progetto::removeFase(int idFase) {
    for (int i = 0; i < fasi.size(); ++i) {
        if (fasi[i]->getId() == idFase) {
            delete fasi.takeAt(i);
            return;
        }
    }
}

void Progetto::updateFase(int idFase, const QJsonObject& dati) {
    for (Attivita* fase : std::as_const(fasi)) {
        if (fase->getId() == std::as_const(idFase)) {
            fase->fromJson(dati);
            fase->setId(idFase);
            return;
        }
    }
}

void Progetto::ordinaAggiornaFasi() {
    //Ordina le sotto-attivita per data di scadenza
    std::sort(fasi.begin(), fasi.end(), [](const Attivita* a, const Attivita* b) {
        if (a == nullptr) return false;
        if (b == nullptr) return true;
        return a->getData() < b->getData();
    });

    //Riassegno faseAttuale al valore corretto
    faseAttuale = fasi.size();
    //Assegno fase attuale alla prima attività non completata
    for (int i = 0; i < fasi.size(); ++i) {
        if (fasi.at(i) != nullptr && !fasi.at(i)->isCompletata()) {
            faseAttuale = i;
            break;
        }
    }
}