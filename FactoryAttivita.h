#ifndef FACTORYATTIVITA_H
#define FACTORYATTIVITA_H

#include "Attivita.h"

class FactoryAttivita {
public:
    static Attivita* create(const QString& tipo);
    static Attivita* create(const QJsonObject& json);
};

#endif // FACTORYATTIVITA_H
