#ifndef MANAGERATTIVITA_H
#define MANAGERATTIVITA_H

#include "Attivita.h"
#include "FactoryAttivita.h"
#include "RegistroCategorie.h"

class ManagerAttivita: public QObject {
    Q_OBJECT
signals:
    void modAttivita();

private:
    QList<Attivita*> listaAttivita;
    FactoryAttivita factory;
    RegistroCategorie categorie;
    int nextId;
public:
    ManagerAttivita();
    virtual ~ManagerAttivita();

    //File
    bool save(const QString& nomeFile) const;
    bool load(const QString& nomeFile, bool sovrascrivi);

    //Gestione attività
    Attivita* findById(int id) const;
    void addAttivita(Attivita* nuova);
    void removeAttivita(int id);
    int newNextId() {nextId++; return nextId;}
    void setCompletata(int id, bool completata);

    //Aggiorna scadenza attività ricorrenti
    void aggiornaRicorrenti();

    //Getter
    int getNextId() {return nextId;}
    RegistroCategorie* getRegistroCategorie() {return &categorie;}
    FactoryAttivita* getFactory() {return &factory;}
    const QList<Attivita*>& getListaAttivita() const {return listaAttivita;}

    //Setter
    void setNextId(int next) {nextId=next;}
};

#endif // MANAGERATTIVITA_H