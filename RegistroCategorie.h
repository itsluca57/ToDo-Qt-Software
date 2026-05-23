#ifndef REGISTROCATEGORIE_H
#define REGISTROCATEGORIE_H

#include <QString>
#include <QList>
#include <QMap>

class RegistroCategorie {
private:
    QMap<QString, QString> coloriCategorie;

public:
    RegistroCategorie();
    virtual ~RegistroCategorie() = default;

    bool esisteCategoria(const QString& nome) const;
    void addCategoria(const QString& nome, const QString& hexColore);
    void deleteCategoria(const QString& nome);
    QList<QString> getListaCategorie() const;
    QString getCodiceColoreCategoria(const QString& nome) const;
};

#endif // REGISTROCATEGORIE_H
