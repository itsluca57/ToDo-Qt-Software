#ifndef PROMEMORIA_H
#define PROMEMORIA_H

#include "Attivita.h"

class Promemoria: public Attivita{
private:
    int preavviso;
public:
    Promemoria();
    Promemoria(int id,
               const QString& nome,
               const QString& descrizione,
               const QDateTime& data,
               Priorita priorita,
               int preavviso,
               const QString& categoria,
               bool completata=false);
    virtual ~Promemoria();

    //Gestione Json
    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;

    //Visitor
    virtual void accept(VisitorAttivita* visitor) override;

    //Getter
    int getPreavviso() const {return preavviso;}

    //Setter
    void setPreavviso(int p) {preavviso=p;}
};

#endif // PROMEMORIA_H
