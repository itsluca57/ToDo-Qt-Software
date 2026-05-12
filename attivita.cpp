#include "attivita.h"

Attivita::Attivita() : Attivita(0, "", "", QDateTime::currentDateTime(), Priorita::Bassa, "ToDo", false) {}

Attivita::Attivita(int id,
                   const QString& nome,
                   const QString& descrizione,
                   const QDateTime& data,
                   Priorita priorita,
                   const QString& categoria,
                   bool completata):
                    id(id),
                    nome(nome),
                    descrizione(descrizione),
                    data(data),
                    priorita(priorita),
                    categoria(categoria),
                    completata(completata){}

Attivita::~Attivita() {}

bool Attivita::isScaduta() const {
    return !completata && data < QDateTime::currentDateTime();
}

QJsonObject Attivita::toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["nome"] = nome;
    json["descrizione"] = descrizione;
    json["data"] = data.toString(Qt::ISODate);
    json["completata"] = completata;
    json["priorita"] = static_cast<int>(priorita);
    json["categoria"] = categoria;
    return json;
}

void Attivita::fromJson(const QJsonObject& json) {
    if (json.contains("id")) id = json["id"].toInt();

    if (json.contains("nome")) nome = json["nome"].toString();

    if (json.contains("descrizione")) descrizione = json["descrizione"].toString();

    if (json.contains("data")) {
        QString dataStr = json["data"].toString();
        data = QDateTime::fromString(dataStr, Qt::ISODate);
    }

    if (json.contains("completata")) completata = json["completata"].toBool();

    if (json.contains("priorita")) {
        int prioInt = json["priorita"].toInt();
        priorita = static_cast<Priorita>(prioInt);
    }

    if (json.contains("categoria")) categoria = json["categoria"].toString();
}