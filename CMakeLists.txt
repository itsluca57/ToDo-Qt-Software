cmake_minimum_required(VERSION 3.16)

project(ToDo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- RIPRISTINATI I NOMI INTERFACCIA ---
set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ToDo
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        tipi.h
        Attivita.h             # Lettera A maiuscola come sul tuo disco
        attivita.cpp
        Promemoria.h
        promemoria.cpp
        Evento.h
        evento.cpp
        Ricorrente.h
        ricorrente.cpp
        Progetto.h
        progetto.cpp
        FactoryAttivita.h
        factoryattivita.cpp
        ManagerAttivita.h
        managerattivita.cpp
        ComparatorAttivita.h
        CompareData.h
        ComparePriorita.h
        CompareNome.h
        risorse.qrc
        RegistroCategorie.h
        registrocategorie.cpp
        VisitorAttivita.h      # Aggiunto per non avere errori dopo
        VisitorGui.h           # Aggiunto per non avere errori dopo
        VisitorGui.cpp         # Aggiunto per non avere errori dopo
    )
else()
    if(ANDROID)
        add_library(ToDo SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ToDo
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ToDo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ToDo)
endif()
set_target_properties(ToDo PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ToDo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ToDo)
endif()