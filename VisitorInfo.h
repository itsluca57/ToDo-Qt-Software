#ifndef VISITORINFO_H
#define VISITORINFO_H

#include "VisitorAttivita.h"
#include <QString>

class Promemoria;
class Evento;
class Ricorrente;
class Progetto;

class VisitorInfo : public VisitorAttivita {
private:
    QString info;

public:
    VisitorInfo() = default;
    virtual ~VisitorInfo() = default;

    void visit(Promemoria* p) override;
    void visit(Evento* e) override;
    void visit(Ricorrente* r) override;
    void visit(Progetto* pr) override;

    QString getInfo() const {return info;};
};

#endif // VISITORINFO_H