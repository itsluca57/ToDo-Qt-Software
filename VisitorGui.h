#ifndef VISITORGUI_H
#define VISITORGUI_H

#include "VisitorAttivita.h"
#include "ui_MainWindow.h"


class Promemoria;
class Evento;
class Ricorrente;
class Progetto;

class VisitorGui : public VisitorAttivita {
public:

    VisitorGui(Ui::MainWindow* interfacciaUi);

    void visit(Promemoria* p) override;
    void visit(Evento* e) override;
    void visit(Ricorrente* r) override;
    void visit(Progetto* pr) override;

private:
    Ui::MainWindow* ui;
};

#endif // VISITORGUI_H