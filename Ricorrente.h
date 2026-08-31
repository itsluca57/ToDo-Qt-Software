#ifndef RICORRENTE_H
#define RICORRENTE_H

#include "Attivita.h"

class Ricorrente: public Attivita {
private:
    Frequenza frequenza;
    QDate fineRicorrenza;
public:
    Ricorrente();
    Ricorrente(int id,
               const QString& nome,
               const QString& descrizione,
               const QDateTime& data,
               Priorita priorita,
               const QString& categoria,
               Frequenza frequenza,
               const QDate& fineRicorrenza,
               bool completata=false);
    virtual ~Ricorrente();

    //Gestione futura occorrenza
    void checkAvanzamento();

    //Gestione Json
    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;

    //Visitor
    virtual void accept(VisitorAttivita* visitor) override;

    //Getter
    Frequenza getFrequenza() const {return frequenza;}
    QDate getFineRicorrenza() const {return fineRicorrenza;}

    //Setter
    void setFrequenza(Frequenza f) {frequenza=f;}
    void setFineRicorrenza(const QDate& fr) {fineRicorrenza=fr;}
};

#endif // RICORRENTE_H
