#ifndef VISITORGUI_H
#define VISITORGUI_H

#include "RegistroCategorie.h"
#include "VisitorAttivita.h"
#include <qtablewidget.h>


class Promemoria;
class Evento;
class Ricorrente;
class Progetto;

class VisitorGui : public VisitorAttivita {
private:
    QTableWidget* table;
    const RegistroCategorie* registroCategorie;

public:
    VisitorGui(QTableWidget* table, const RegistroCategorie* registroCategorie);
    virtual ~VisitorGui() = default;

    void visit(Promemoria* p) override;
    void visit(Evento* e) override;
    void visit(Ricorrente* r) override;
    void visit(Progetto* pr) override;
};

#endif // VISITORGUI_H