#ifndef MANAGERATTIVITA_H
#define MANAGERATTIVITA_H

#include "Attivita.h"
#include "AttivitaFactory.h"

class ManagerAttivita {
private:
    QList<Attivita*> listaAttivita;
    AttivitaFactory factory;
    int nextId;

    Attivita* findById(int id) const;
public:
    ManagerAttivita();
    virtual ~ManagerAttivita();
    bool save(const QString& nomeFile) const;
    bool load(const QString& nomeFile);
    void addAttivita(Attivita* nuova);
    void removeAttivita(int id);
    void updateAttivita(int id, const QJsonObject& dati);
    QList<Attivita*> filterAttivita(const QString& categoria="", Priorita priorita=Priorita::Qualsiasi,
                                   const QDate& dataInizio=QDate(), const QDate& dataFine=QDate(), const QString& testo="") const;

    //Getter
    int getNextId() const {return nextId;}

    //Setter
    void setNextId(int next) {nextId=next;}
};

#endif // MANAGERATTIVITA_H
