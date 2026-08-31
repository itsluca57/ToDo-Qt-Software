#include "Ricorrente.h"

Ricorrente::Ricorrente(): Attivita(), frequenza(Frequenza::Mensile), fineRicorrenza(QDate::currentDate()) {}

Ricorrente::Ricorrente(int id,
                       const QString& nome,
                       const QString& descrizione,
                       const QDateTime& data,
                       Priorita priorita,
                       const QString& categoria,
                       Frequenza frequenza,
                       const QDate& fineRicorrenza,
                       bool completata)
                        : Attivita(id, nome, descrizione, data, priorita, categoria, completata),
                        frequenza(frequenza),
                        fineRicorrenza(fineRicorrenza) {}

Ricorrente::~Ricorrente() {}

//Visitor
void Ricorrente::accept(VisitorAttivita* visitor) {
    visitor->visit(this);
}

//Cambio data per successiva occorrenza dell'attività
void Ricorrente::checkAvanzamento() {
    if(this->isCompletata() && this->getData() < QDateTime::currentDateTime()) {
        switch(this->getFrequenza()) {
        case Frequenza::Giornaliera:
            if(this->getData().addDays(1).date() <= this->getFineRicorrenza()) {
                this->setData(this->getData().addDays(1));
                this->setCompletata(false);
            }
            break;
        case Frequenza::Settimanale:
            if(this->getData().addDays(7).date() <= this->getFineRicorrenza()) {
                this->setData(this->getData().addDays(7));
                this->setCompletata(false);
            }
            break;
        case Frequenza::Mensile:
            if(this->getData().addMonths(1).date() <= this->getFineRicorrenza()) {
                this->setData(this->getData().addMonths(1));
                this->setCompletata(false);
            }
            break;
        }
    }
}

QJsonObject Ricorrente::toJson() const {
    QJsonObject json = Attivita::toJson();
    json["tipo"] = "Ricorrente";
    json["frequenza"] = static_cast<int>(frequenza);
    json["fineRicorrenza"] = fineRicorrenza.toString(Qt::ISODate);
    return json;
}

void Ricorrente::fromJson(const QJsonObject& json) {
    Attivita::fromJson(json);
    if (json.contains("frequenza")) {
        int freqInt = json["frequenza"].toInt();
        frequenza = static_cast<Frequenza>(freqInt);
    }
    if (json.contains("fineRicorrenza")) {
        QString dataStr = json["fineRicorrenza"].toString();
        fineRicorrenza = QDate::fromString(dataStr, Qt::ISODate);
    }
}