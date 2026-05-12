#include "AttivitaFactory.h"
#include "Evento.h"
#include "Promemoria.h"
#include "Ricorrente.h"
#include "Progetto.h"

Attivita* AttivitaFactory::create(const QJsonObject& json) {
    if (!json.contains("tipo")) {
        return nullptr;
    }

    QString tipo = json["tipo"].toString();
    Attivita* nuovaAttivita = nullptr;

    if (tipo == "Evento") {
        nuovaAttivita = new Evento();
    }
    else if (tipo == "Promemoria") {
        nuovaAttivita = new Promemoria();
    }
    else if (tipo == "Ricorrente") {
        nuovaAttivita = new Ricorrente();
    }
    else if (tipo == "Progetto") {
        nuovaAttivita = new Progetto();
    }

    if (nuovaAttivita) {
        nuovaAttivita->fromJson(json);
    }
    return nuovaAttivita;
}