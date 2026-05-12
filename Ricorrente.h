#ifndef RICORRENTE_H
#define RICORRENTE_H

#include "Attivita.h"

class Ricorrente: public Attivita {
private:
    Frequenza frequenza;
    QDateTime fineRicorrenza;
public:
    Ricorrente();
    Ricorrente(int id,
               const QString& nome,
               const QString& descrizione,
               const QDateTime& data,
               Priorita priorita,
               const QString& categoria,
               Frequenza frequenza,
               const QDateTime& fineRicorrenza,
               bool completata=false);
    virtual ~Ricorrente();
    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;

    //Getter
    Frequenza getFrequenza() const {return frequenza;}
    QDateTime getFineRicorrenza() const {return fineRicorrenza;}

    //Setter
    void setFrequenza(Frequenza f) {frequenza=f;}
    void setFineRicorrenza(const QDateTime& fr) {fineRicorrenza=fr;}
};

#endif // RICORRENTE_H
