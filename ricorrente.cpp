#include "ricorrente.h"

Ricorrente::Ricorrente(): Attivita(), frequenza(Frequenza::Mensile), fineRicorrenza(QDateTime::currentDateTime()) {}

Ricorrente::Ricorrente(int id,
                       const QString& nome,
                       const QString& descrizione,
                       const QDateTime& data,
                       Priorita priorita,
                       const QString& categoria,
                       Frequenza frequenza,
                       const QDateTime& fineRicorrenza,
                       bool completata)
                        : Attivita(id, nome, descrizione, data, priorita, categoria, completata),
                        frequenza(frequenza),
                        fineRicorrenza(fineRicorrenza) {}

Ricorrente::~Ricorrente() {}

//Visitor
void Ricorrente::accept(VisitorAttivita* visitor) {
    visitor->visit(this);
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
        fineRicorrenza = QDateTime::fromString(dataStr, Qt::ISODate);
    }
}