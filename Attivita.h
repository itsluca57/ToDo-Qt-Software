#ifndef ATTIVITA_H
#define ATTIVITA_H

#include "tipi.h"
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Attivita {
private:
    int id;
    QString nome;
    QString descrizione;
    QDateTime data;
    Priorita priorita;
    QString categoria;
    bool completata;

public:
    Attivita();
    Attivita(int id,
             const QString& nome,
             const QString& descrizione,
             const QDateTime& data,
             Priorita priorita,
             const QString& categoria,
             bool completata=false);
    virtual ~Attivita();
    bool isScaduta() const;

    //Gestione Json
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& json);

    //Getter
    int getId() const {return id;}
    QString getNome() const {return nome;}
    QString getDescrizione() const {return descrizione;}
    QDateTime getData() const {return data;}
    bool isCompletata() const {return completata;}
    Priorita getPriorita() const {return priorita;}
    QString getCategoria() const {return categoria;}

    //Setter
    void setId(int i) {id=i;}
    void setNome(const QString& n) {nome = n;}
    void setDescrizione(const QString& d) {descrizione = d;}
    void setData(const QDateTime& d) {data = d;}
    void setCompletata(bool c) {completata = c;}
    void setPriorita(Priorita p) {priorita = p;}
    void setCategoria(const QString& c) {categoria = c;}
};

#endif // ATTIVITA_H
