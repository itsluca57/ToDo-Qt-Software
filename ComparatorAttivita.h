#ifndef COMPARATORATTIVITA_H
#define COMPARATORATTIVITA_H

#include"Attivita.h"

class ComparatorAttivita {
public:
    virtual ~ComparatorAttivita() {}

    virtual bool compare(Attivita* a, Attivita* b) const = 0;
};

#endif // COMPARATORATTIVITA_H
