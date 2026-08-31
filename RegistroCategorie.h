#ifndef REGISTROCATEGORIE_H
#define REGISTROCATEGORIE_H

#include <QString>
#include <QList>
#include <QMap>
#include <qobject.h>

class RegistroCategorie: public QObject {
    Q_OBJECT
signals:
    void modCategorie();
private:
    QMap<QString, QString> coloriCategorie;

public:
    RegistroCategorie();
    virtual ~RegistroCategorie() = default;

    //Gestione categorie
    bool esisteCategoria(const QString& nome) const;
    void addCategoria(const QString& nome, const QString& hexColore);
    void deleteCategoria(const QString& nome);
    void modCategoria(const QString& nomePrec, const QString& nome, const QString& colore);
    void clear();
    QString getColoreLibero() const;

    //Getter
    QList<QString> getListaCategorie() const;
    QString getCodiceColoreCategoria(const QString& nome) const;
};

#endif // REGISTROCATEGORIE_H
