#ifndef COMPAREDATA_H
#define COMPAREDATA_H

#include "ComparatorAttivita.h"

class CompareData: public ComparatorAttivita {
public:
    virtual ~CompareData() {}
    bool compare(Attivita* a, Attivita* b) const override {
        if (a->getData() == b->getData()) {
            return a->getNome().toLower() < b->getNome().toLower();
        }
        return a->getData() < b->getData();
    }
};

#endif // COMPAREDATA_H
