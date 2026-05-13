#ifndef TIPI_H
#define TIPI_H

#include <QString>
#include <QDate>

enum class Priorita{
    Qualsiasi,
    Alta,
    Media,
    Bassa
};

enum class Frequenza{
    Giornaliera,
    Settimanale,
    Mensile
};

struct CriteriRicerca {
    QString testo = "";
    QString categoria = "Qualsiasi";
    Priorita priorita = Priorita::Qualsiasi;
    QDate dataInizio;
    QDate dataFine;
};

#endif // TIPI_H