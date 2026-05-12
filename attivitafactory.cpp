#include "AttivitaFactory.h"
#include "Evento.h"
#include "Promemoria.h"
#include "Ricorrente.h"
#include "Progetto.h"

Attivita* AttivitaFactory::create(const QString& tipo) {
    if(tipo == "Evento") return new Evento();
    if(tipo == "Progetto") return new Progetto();
    if(tipo == "Promemoria") return new Promemoria();
    if(tipo == "Ricorrente") return new Ricorrente();
    return nullptr;
}

Attivita* AttivitaFactory::create(const QJsonObject& json) {
    QString tipo = json["tipo"].toString();
    Attivita* nuova = create(tipo);
    if(nuova) nuova->fromJson(json);
    return nuova;
}