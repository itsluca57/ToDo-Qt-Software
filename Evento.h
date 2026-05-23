#ifndef EVENTO_H
#define EVENTO_H

#include "Attivita.h"

class Evento: public Attivita{
private:
    QDateTime dataFine;
    QString luogo;
public:
    Evento();
    Evento(int id,
           const QString& nome,
           const QString& descrizione,
           const QDateTime& data,
           Priorita priorita,
           const QString& categoria,
           const QDateTime& dataFine,
           const QString& luogo,
           bool completata=false);
    virtual ~Evento();
    virtual QJsonObject toJson() const override;
    virtual void fromJson(const QJsonObject& json) override;

    //Visitor
    virtual void accept(VisitorAttivita* visitor) override;

    //Getter
    QDateTime getDataFine() const {return dataFine;}
    QString getLuogo() const {return luogo;}

    //Setter
    void setDataFine(const QDateTime& df) {dataFine=df;}
    void setLuogo(const QString& l) {luogo=l;}
};

#endif // EVENTO_H
