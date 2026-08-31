#include "Evento.h"
#include "Promemoria.h"
#include "Ricorrente.h"
#include "VisitorInfo.h"

//Promemoria
void VisitorInfo::visit(Promemoria* p) {
    info = QString("Preavviso: %1 min").arg(p->getPreavviso());
}
//Evento
void VisitorInfo::visit(Evento* e) {
    QString fineStr = e->getDataFine().toString("dd/MM/yyyy hh:mm");
    if (e->getLuogo().isEmpty()) {
        info = QString("Fine: %1").arg(fineStr);
    } else {
        info = QString("Fine: %1\nLuogo: %2").arg(fineStr, e->getLuogo());
    }
}
//Ricorrente
void VisitorInfo::visit(Ricorrente* r) {
    QString freqStr = (r->getFrequenza() == Frequenza::Settimanale) ? "Settimanale" :
                          (r->getFrequenza() == Frequenza::Giornaliera) ? "Giornaliera" : "Mensile";
    QString fineRicStr = r->getFineRicorrenza().toString("dd/MM/yyyy");
    info = QString("Frequenza: %1\nFino al: %2").arg(freqStr, fineRicStr);
}
//Progetto (non sarà mai usato, non ci possono essere progetti annidati
void VisitorInfo::visit(Progetto* pr) {}
