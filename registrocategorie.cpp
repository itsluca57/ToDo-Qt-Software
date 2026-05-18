#include "registrocategorie.h"

RegistroCategorie::RegistroCategorie() {}

bool RegistroCategorie::esisteCategoria(const QString& nome) const {return coloriCategorie.contains(nome.trimmed());}

void RegistroCategorie::addCategoria(const QString& nome, const QString& hexColore) {
    if (!esisteCategoria(nome)) {
        coloriCategorie.insert(nome.trimmed(), hexColore);
    }
}

QList<QString> RegistroCategorie::getListaCategorie() const {
    return coloriCategorie.keys();
}

QString RegistroCategorie::getCodiceColoreCategoria(const QString& nome) const {
    return coloriCategorie.value(nome.trimmed());
}