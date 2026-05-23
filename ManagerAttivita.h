#ifndef MANAGERATTIVITA_H
#define MANAGERATTIVITA_H

#include "Attivita.h"
#include "FactoryAttivita.h"
#include "ComparatorAttivita.h"
#include "RegistroCategorie.h"

class ManagerAttivita {

private:
    QList<Attivita*> listaAttivita;
    FactoryAttivita factory;
    RegistroCategorie categorie;
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
    QList<Attivita*> getVista(const ComparatorAttivita& comparator, const CriteriRicerca& filtro) const;
    QList<Attivita*> getVista(const CriteriRicerca& filtro) const;
    int newNextId() {nextId++; return nextId;}

    //Getter
    int getNextId() {return nextId;}
    RegistroCategorie* getRegistroCategorie() {return &categorie;}
    FactoryAttivita* getFactory() {return &factory;}
    const QList<Attivita*>& getListaAttivita() const {return listaAttivita;}

    //Setter
    void setNextId(int next) {nextId=next;}
};

#endif // MANAGERATTIVITA_H