#include "ManagerAttivita.h"
#include "Progetto.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

ManagerAttivita::ManagerAttivita() : nextId(1) {}

ManagerAttivita::~ManagerAttivita() {
    qDeleteAll(listaAttivita);
    listaAttivita.clear();
}

Attivita* ManagerAttivita::findById(int id) const {
    for (Attivita* a : listaAttivita) {
        if (a->getId() == id) return a;
    }
    return nullptr;
}

void ManagerAttivita::addAttivita(Attivita* nuova) {
    if (!nuova) return;

    if (nuova->getId() <= 0 || findById(nuova->getId()) != nullptr) {
        nuova->setId(nextId++);
    } else if (nuova->getId() >= nextId) {
        nextId = nuova->getId() + 1;
    }

    if (!listaAttivita.contains(nuova)) {
        listaAttivita.append(nuova);
    }

    Progetto* p = dynamic_cast<Progetto*>(nuova);
    if (p) {
        const QList<Attivita*>& fasi = p->getFasi();
        for (Attivita* fase : fasi) {
            this->addAttivita(fase);
        }
    }
}

void ManagerAttivita::removeAttivita(int id) {
    for (int i=0; i<listaAttivita.size(); ++i) {
        if (listaAttivita[i]->getId() == id) {
            delete listaAttivita.takeAt(i);
            return;
        }
    }
}

void ManagerAttivita::updateAttivita(int id, const QJsonObject& dati) {
    Attivita* esistente = findById(id);
    if (esistente) {
        esistente->fromJson(dati);
        esistente->setId(id);
    }
}

bool ManagerAttivita::save(const QString& nomeFile) const {
    QJsonArray array;
    for (const Attivita* a : listaAttivita) {
        array.append(a->toJson());
    }

    QFile file(nomeFile);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QJsonDocument doc(array);
    file.write(doc.toJson());
    return true;
}

bool ManagerAttivita::load(const QString& nomeFile) {
    QFile file(nomeFile);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray dati = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(dati);
    QJsonArray array = doc.array();

    qDeleteAll(listaAttivita);
    listaAttivita.clear();
    nextId = 1;

    for (const QJsonValue& valore : std::as_const(array)) {
        QJsonObject obj = valore.toObject();
        Attivita* a = factory.create(obj);
        if (a) {
            addAttivita(a);
        }
    }
    return true;
}

QList<Attivita*> ManagerAttivita::filterAttivita(const QString& categoria, Priorita priorita,
                                                  const QDate& dataInizio, const QDate& dataFine,
                                                  const QString& testo) const {
    QList<Attivita*> risultati;

    for (Attivita* a : listaAttivita) {
        bool accettato = true;

        if (!categoria.isEmpty() && a->getCategoria() != categoria) accettato = false;
        if (priorita != Priorita::Qualsiasi && a->getPriorita() != priorita) accettato = false;

        QDate dataA = a->getData().date();
        if (dataInizio.isValid() && dataA < dataInizio) accettato = false;
        if (dataFine.isValid() && dataA > dataFine) accettato = false;

        if (!testo.isEmpty()) {
            bool trovato = a->getNome().contains(testo, Qt::CaseInsensitive) ||
                           a->getDescrizione().contains(testo, Qt::CaseInsensitive);
            if (!trovato) accettato = false;
        }

        if (accettato) risultati.append(a);
    }
    return risultati;
}