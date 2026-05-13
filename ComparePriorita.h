#ifndef COMPAREPRIORITA_H
#define COMPAREPRIORITA_H

#include "ComparatorAttivita.h"

class ComparePriorita: public ComparatorAttivita {
public:
    virtual ~ComparePriorita() {}
    bool compare(Attivita* a, Attivita* b) const override {
        int pA = static_cast<int>(a->getPriorita());
        int pB = static_cast<int>(b->getPriorita());

        if (pA == pB) {
            return a->getData() < b->getData();
        }
        return pA > pB;
    }
};

#endif // COMPAREPRIORITA_H
