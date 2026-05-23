#ifndef VISITORATTIVITA_H
#define VISITORATTIVITA_H

class Promemoria;
class Evento;
class Ricorrente;
class Progetto;

class VisitorAttivita {
public:

    virtual ~VisitorAttivita() = default;

    virtual void visit(Promemoria* p) = 0;
    virtual void visit(Evento* e) = 0;
    virtual void visit(Ricorrente* r) = 0;
    virtual void visit(Progetto* pr) = 0;
};

#endif // VISITORATTIVITA_H