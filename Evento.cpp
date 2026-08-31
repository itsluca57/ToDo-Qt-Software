#include "Evento.h"

Evento::Evento(): Attivita(), dataFine(QDateTime::currentDateTime()), luogo("") {}

Evento::Evento(int id,
               const QString& nome,
               const QString& descrizione,
               const QDateTime& data,
               Priorita priorita,
               const QString& categoria,
               const QDateTime& dataFine,
               const QString& luogo,
               bool completata):
                Attivita(id,nome,descrizione,data,priorita,categoria,completata),
                dataFine(dataFine),luogo(luogo) {}

Evento::~Evento() {}

//Visitor
void Evento::accept(VisitorAttivita* visitor) {
    visitor->visit(this);
}

QJsonObject Evento::toJson() const {
    QJsonObject json = Attivita::toJson();
    json["tipo"] = "Evento";
    json["dataFine"] = dataFine.toString(Qt::ISODate);
    json["luogo"] = luogo;
    return json;
}

void Evento::fromJson(const QJsonObject& json) {
    Attivita::fromJson(json);
    if(json.contains("dataFine")) {
        QString dataString = json["dataFine"].toString();
        dataFine = QDateTime::fromString(dataString,Qt::ISODate);
    }
    if(json.contains("luogo")) luogo = json["luogo"].toString();
}