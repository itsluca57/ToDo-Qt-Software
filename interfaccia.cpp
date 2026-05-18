#include "interfaccia.h"
#include "./ui_interfaccia.h"
#include "ManagerAttivita.h"
#include "RegistroCategorie.h"

#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new ManagerAttivita();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

void MainWindow::on_listCategorie_itemSelectionChanged()
{
    for (int i = 0; i < ui->listCategorie->count(); ++i) {
        QListWidgetItem* item = ui->listCategorie->item(i);
        QFont font = item->font();

        if (item->isSelected()) {
            font.setBold(true);
        } else {
            font.setBold(false);
        }
        item->setFont(font);
    }
}
void MainWindow::on_addButton_clicked()
{
    ui->mainStacked->setCurrentIndex(1);
    ui->radioPromemoria->setChecked(true);
    ui->radioMedia->setChecked(true);
}


void MainWindow::on_radioEvento_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Inizio:");
        ui->tipoStacked->setCurrentIndex(1);
    }
}


void MainWindow::on_radioPromemoria_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(0);
    }
}


void MainWindow::on_radioRicorrente_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(2);
        ui->radioGiorn->setChecked(true);
    }
}


void MainWindow::on_radioProgetto_toggled(bool checked)
{
    if (checked) {
        ui->dataLabel->setText("Scadenza:");
        ui->tipoStacked->setCurrentIndex(3);
    }
}


void MainWindow::on_pushButton_clicked()
{
    ui->mainStacked->setCurrentIndex(0);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->mainStacked->setCurrentIndex(0);
}

