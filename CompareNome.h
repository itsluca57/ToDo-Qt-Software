#ifndef COMPARENOME_H
#define COMPARENOME_H

#include "ComparatorAttivita.h"

class CompareNome: public ComparatorAttivita {
public:
    virtual ~CompareNome() {}
    bool compare(Attivita* a, Attivita* b) const override{
        return a->getNome().toLower() < b->getNome().toLower();
    }
};

#endif // COMPARENOME_H
