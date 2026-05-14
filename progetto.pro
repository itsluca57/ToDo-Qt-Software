QT += widgets
CONFIG += c++17

TEMPLATE = app
TARGET = ToDo
INCLUDEPATH += .

# File Header
HEADERS += Attivita.h \
           ComparatorAttivita.h \
           CompareData.h \
           CompareNome.h \
           ComparePriorita.h \
           Evento.h \
           FactoryAttivita.h \
           mainwindow.h \
           ManagerAttivita.h \
           Progetto.h \
           Promemoria.h \
           Ricorrente.h \
           tipi.h

# File Sorgenti
SOURCES += attivita.cpp \
           evento.cpp \
           factoryattivita.cpp \
           main.cpp \
           mainwindow.cpp \
           managerattivita.cpp \
           progetto.cpp \
           promemoria.cpp \
           ricorrente.cpp

# Interfaccia Grafica
FORMS += mainwindow.ui