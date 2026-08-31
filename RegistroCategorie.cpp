#include "RegistroCategorie.h"

RegistroCategorie::RegistroCategorie() {}

bool RegistroCategorie::esisteCategoria(const QString& nome) const {return coloriCategorie.contains(nome.trimmed());}

void RegistroCategorie::addCategoria(const QString& nome, const QString& hexColore) {
    if (!esisteCategoria(nome)) {
        coloriCategorie.insert(nome.trimmed(), hexColore);
    }
    emit modCategorie();
}

void RegistroCategorie::deleteCategoria(const QString& nome) {
    coloriCategorie.remove(nome);
    emit modCategorie();
}

void RegistroCategorie::clear() {
    coloriCategorie.clear();
    addCategoria("Qualsiasi", "#FFFFFF");
    emit modCategorie();
}

void RegistroCategorie::modCategoria(const QString& nomePrec, const QString& nome, const QString& colore) {

    if (!coloriCategorie.contains(nomePrec)) {
        return;
    }
    //controllo che il nuovo nome non sia già presente
    if (nomePrec != nome && coloriCategorie.contains(nome)) {
        return;
    }
    //Rimuovo la vecchia categoria
    if (nomePrec != nome) {
        coloriCategorie.remove(nomePrec);
    }

    coloriCategorie[nome] = colore;

    emit modCategorie();
}

QList<QString> RegistroCategorie::getListaCategorie() const {
    return coloriCategorie.keys();
}

QString RegistroCategorie::getCodiceColoreCategoria(const QString& nome) const {
    return coloriCategorie.value(nome.trimmed());
}

QString RegistroCategorie::getColoreLibero() const {
    const QStringList palette = {
        "#D63031",
        "#FFCCFF",
        "#FFFF33",
        "#1F3D99",
        "#2EB82E",
        "#FF9900",
        "#00FFFF",
        "#BA884B",
        "#7A007A"
    };

    //Cerco il primo colore non usato
    for (const QString& colore : palette) {
        bool usato = false;

        //Guardo ogni colore di categoria presente
        for (const QString& coloreUsato : std::as_const(coloriCategorie)) {
            if (coloreUsato == colore) {
                usato = true;
                break;
            }
        }

        if (!usato) return colore;
    }

    //Se non ci sono colori libero assegno rosso
    return "#D63031";
}