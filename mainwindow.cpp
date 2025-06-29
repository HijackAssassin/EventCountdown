#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "countdowntile.h"
#include "utils.h"
#include <QGridLayout>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "editcountdowndialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    showMaximized();
    setWindowTitle("Event Countdown Manager");

    if (isDarkModeEnabled())
        setStyleSheet("background-color: #1e1e1e; color: white;");

    // Setup grid layout
    tileLayout = qobject_cast<QGridLayout*>(ui->scrollAreaWidgetContents->layout());
    tileLayout->setSpacing(0);
    tileLayout->setContentsMargins(0, 0, 0, 0);

    // Set default combo box values for embedded create menu
    int currentYear = QDate::currentDate().year();
    for (int y = 2000; y <= currentYear + 100; ++y)
        ui->yearCombo->addItem(QString::number(y), y);

    ui->yearCombo->setCurrentText(QString::number(currentYear));

    QLocale locale;
    for (int m = 1; m <= 12; ++m)
        ui->monthCombo->addItem(locale.monthName(m), m);

    for (int d = 1; d <= 31; ++d)
        ui->dayCombo->addItem(QString::number(d), d);

    for (int h = 0; h < 24; ++h)
        ui->hourCombo->addItem(QString("%1:00").arg(h, 2, 10, QChar('0')), h);

    connect(ui->plusButton, &QPushButton::clicked, this, &MainWindow::onPlusButtonClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::handleCreateButton);
    connect(ui->editModeButton, &QPushButton::clicked, this, &MainWindow::toggleEditMode);
    connect(ui->chooseImageButton, &QPushButton::clicked, this, &MainWindow::handleImageSelection);
    loadCountdowns();
}

void MainWindow::handleImageSelection() {
    QString file = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!file.isEmpty()) {
        QDir dir(QCoreApplication::applicationDirPath() + "/tile_images");
        if (!dir.exists()) dir.mkpath(".");

        QString destPath = dir.filePath(QFileInfo(file).fileName());
        QFile::copy(file, destPath);
        selectedImagePath = destPath;
    }
}

void MainWindow::onPlusButtonClicked() {
    // If canceling edit mode
    if (editingTile) {
        editingTile = nullptr;
        ui->createFrame->setVisible(false);
        ui->plusButton->setText("+");
        ui->createButton->setText("Create Event");
        ui->titleInput->clear();
        selectedImagePath.clear();
        return;
    }

    if (!ui->createFrame->isVisible()) {
        int w = 600, h = 250;
        int x = (this->width() - w) / 2;
        int y = (this->height() - h) / 2;

        ui->createFrame->setGeometry(x, y, w, h);
        ui->createFrame->raise();
        ui->createFrame->setStyleSheet(R"(
            QFrame#createFrame {
                background-color: white;
                border-radius: 10px;
                border: 2px solid #999;
            }
            QLineEdit, QComboBox, QPushButton {
                font-size: 14pt;
                padding: 6px;
            }
        )");
    }

    ui->createFrame->setVisible(!ui->createFrame->isVisible());
}

void MainWindow::handleCreateButton() {
    QString title = ui->titleInput->text().trimmed();
    if (title.isEmpty()) title = "Untitled Event";

    int year = ui->yearCombo->currentData().toInt();
    int month = ui->monthCombo->currentData().toInt();
    int day = ui->dayCombo->currentData().toInt();
    int hour = ui->hourCombo->currentData().toInt();
    QDateTime datetime(QDate(year, month, day), QTime(hour, 0));

    if (editingTile) {
        editingTile->setTitle(title);
        editingTile->setTargetDateTime(datetime);
        if (!selectedImagePath.isEmpty()) {
            editingTile->setBackgroundImage(selectedImagePath);
        }
        editingTile = nullptr;
        ui->plusButton->setText("+");
        ui->createButton->setText("Create Event");
        selectedImagePath.clear();
        saveCountdowns();
    } else {
        handleCountdownCreated(title, datetime);
    }

    ui->titleInput->clear();
    ui->createFrame->setVisible(false);
}

void MainWindow::handleCountdownCreated(const QString &title, const QDateTime &target) {
    CountdownTile *tile = new CountdownTile(title, target);
    tile->setFixedSize(627, 353);

    if (!selectedImagePath.isEmpty()) {
        tile->setBackgroundImage(selectedImagePath);
        selectedImagePath.clear();
    }

    int col = tileCount % 3;
    int row = tileCount / 3;
    tileLayout->addWidget(tile, row, col);
    tileLayout->setAlignment(tile, Qt::AlignHCenter);
    tileCount++;

    if (!isLoading)
        saveCountdowns();

    ui->createFrame->setVisible(false);

    connect(tile, &CountdownTile::requestDelete, this, &MainWindow::handleTileDeletion);

    connect(tile, &CountdownTile::requestEdit, this, [this, tile]() {
        editingTile = tile;

        ui->plusButton->setText("Cancel Edit");
        ui->createButton->setText("Save Event");
        ui->titleInput->setText(tile->getTitle());

        QDateTime dt = tile->getTargetDateTime();
        ui->yearCombo->setCurrentText(QString::number(dt.date().year()));
        ui->monthCombo->setCurrentIndex(dt.date().month() - 1);
        ui->dayCombo->setCurrentIndex(dt.date().day() - 1);
        ui->hourCombo->setCurrentIndex(dt.time().hour());

        int w = 600, h = 250;
        int x = (this->width() - w) / 2;
        int y = (this->height() - h) / 2;
        ui->createFrame->setGeometry(x, y, w, h);
        ui->createFrame->raise();
        ui->createFrame->setVisible(true);
    });
}

void MainWindow::saveCountdowns() {
    QJsonArray array;
    for (CountdownTile* tile : CountdownTile::getAllTiles()) {
        QJsonObject obj;
        obj["title"] = tile->getTitle();
        obj["datetime"] = tile->getTargetDateTime().toString(Qt::ISODate);
        obj["background"] = tile->getBackgroundImagePath();
        array.append(obj);
    }

    QFile file("countdowns.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadCountdowns() {
    isLoading = true;

    QFile file("countdowns.json");
    if (!file.open(QIODevice::ReadOnly)) {
        isLoading = false;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray array = doc.array();
    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        QString title = obj["title"].toString();
        QDateTime dt = QDateTime::fromString(obj["datetime"].toString(), Qt::ISODate);
        QString bgPath = obj["background"].toString();

        CountdownTile *tile = new CountdownTile(title, dt);
        if (!bgPath.isEmpty())
            tile->setBackgroundImage(bgPath);

        tile->setFixedSize(627, 353);
        int col = tileCount % 3;
        int row = tileCount / 3;
        tileLayout->addWidget(tile, row, col);
        tileLayout->setAlignment(tile, Qt::AlignHCenter);
        tileCount++;

        connect(tile, &CountdownTile::requestDelete, this, &MainWindow::handleTileDeletion);

        // ✅ Connect the edit signal here
        connect(tile, &CountdownTile::requestEdit, this, [this, tile]() {
            editingTile = tile;
            ui->plusButton->setText("Cancel Edit");
            ui->createButton->setText("Save Event");
            ui->titleInput->setText(tile->getTitle());

            QDateTime dt = tile->getTargetDateTime();
            ui->yearCombo->setCurrentText(QString::number(dt.date().year()));
            ui->monthCombo->setCurrentIndex(dt.date().month() - 1);
            ui->dayCombo->setCurrentIndex(dt.date().day() - 1);
            ui->hourCombo->setCurrentIndex(dt.time().hour());

            int w = 600, h = 250;
            int x = (this->width() - w) / 2;
            int y = (this->height() - h) / 2;
            ui->createFrame->setGeometry(x, y, w, h);
            ui->createFrame->raise();
            ui->createFrame->setVisible(true);
        });
    }

    isLoading = false;
}

void MainWindow::toggleEditMode() {
    editMode = !editMode;

    for (CountdownTile* tile : CountdownTile::getAllTiles()) {
        tile->setDeleteButtonVisible(editMode);
    }

    ui->editModeButton->setText(editMode ? "Done" : "Edit Mode");
}

void MainWindow::handleTileDeletion(CountdownTile* tile) {
    CountdownTile::getAllTiles().removeOne(tile);
    delete tile;

    // Clear layout
    QLayoutItem *item;
    while ((item = tileLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            tileLayout->removeWidget(item->widget());
        }
        delete item;
    }

    // Re-add all tiles
    tileCount = 0;
    for (CountdownTile* t : CountdownTile::getAllTiles()) {
        int col = tileCount % 3;
        int row = tileCount / 3;
        tileLayout->addWidget(t, row, col);
        tileCount++;
    }

    saveCountdowns();
}

MainWindow::~MainWindow() {
    saveCountdowns(); // ✅ call this before cleanup
    delete ui;
}

void MainWindow::handleImageButton() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Background Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!filePath.isEmpty()) {
        QDir dir("images");
        if (!dir.exists()) QDir().mkdir("images");

        QFileInfo fi(filePath);
        QString destPath = "images/" + fi.fileName();
        QFile::copy(filePath, destPath);

        selectedImagePath = destPath;
    }
}
