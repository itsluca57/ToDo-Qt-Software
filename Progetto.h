#ifndef PROGETTO_H
#define PROGETTO_H

#include "Attivita.h"

class Progetto: public Attivita {
private:
    QList<Attivita*> fasi;
    int faseAttuale;
public:
    Progetto();
    Progetto(int id,
             const QString& nome,
             const QString& descrizione,
             const QDateTime& data,
             Priorita priorita,
             const QString& categoria,
             const QList<Attivita*>& fasi,
             int faseAttuale,
             bool completata=false);
    virtual ~Progetto();

    //Visitor
    virtual void accept(VisitorAttivita* visitor) override;

    //Gestione Json
    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;

    //Gestione Fasi
    void addFase(Attivita* fase);
    void removeFase(int idFase);
    void updateFase(int idFase, const QJsonObject& dati);

    //Getter
    QList<Attivita*> getFasi() const {return fasi;}
    int getFaseAttuale() const {return faseAttuale;}

    //Setter
    void setFasi(const QList<Attivita*>& f) {
        if (&fasi != &f) {
            qDeleteAll(fasi);
            fasi=f;
        }
    }
    void setFaseAttuale(int fa) {faseAttuale=fa;}
};

#endif // PROGETTO_H
