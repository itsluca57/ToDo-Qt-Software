#ifndef ATTIVITAFACTORY_H
#define ATTIVITAFACTORY_H

#include "Attivita.h"

class AttivitaFactory {
public:
    static Attivita* create(const QJsonObject& json);
};

#endif // ATTIVITAFACTORY_H
