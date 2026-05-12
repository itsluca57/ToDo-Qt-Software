#include "Promemoria.h"

Promemoria::Promemoria(): Attivita(), preavviso(0) {}

Promemoria::Promemoria(int id,
                       const QString& nome,
                       const QString& descrizione,
                       const QDateTime& data,
                       Priorita priorita,
                       int preavviso,
                       const QString& categoria,
                       bool completata):
                        Attivita(id,nome,descrizione,data,priorita,categoria,completata),
                        preavviso(preavviso) {}

Promemoria::~Promemoria() {}

QJsonObject Promemoria::toJson() const {
    QJsonObject json = Attivita::toJson();
    json["tipo"] = "Promemoria";
    json["preavviso"] = preavviso;
    return json;
}

void Promemoria::fromJson(const QJsonObject& json) {
    Attivita::fromJson(json);
    if(json.contains("preavviso")) preavviso=json["preavviso"].toInt();
}