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
#include <QStandardPaths>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    showMaximized();
    setWindowTitle("Event Countdown Manager");

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/default_icon.ico"));  // Replace with your icon
    trayIcon->setToolTip("Event Countdown Manager");

    // ✅ Create tray menu
    trayMenu = new QMenu(this);
    QAction *showAction = new QAction("Show", this);
    QAction *quitAction = new QAction("Quit", this);
    trayMenu->addAction(showAction);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);

    // ✅ Connect actions
    connect(showAction, &QAction::triggered, this, &MainWindow::show);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon->show();  // ✅ visible in tray

    // Optional: restore on left-click
    connect(trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            this->show();
        }
    });

    ui->editModeButton->setMinimumSize(60, 60);
    ui->editModeButton->setStyleSheet(R"(
    QPushButton {
        background-color: #686A71;
        border: none;
        border-radius: 30px;
        color: #000000;
        font-size: 20pt;
        font-weight: bold;
        padding: 10px;
    }
    QPushButton:hover {
        background-color: #808080;
    }
    QPushButton:pressed {
        background-color: #808080;
    }
)");
    // Size: same as editmodebutton, round shape via border-radius
    ui->plusButton->setMinimumSize(60, 60);

    ui->plusButton->setStyleSheet(R"(
    QPushButton {
        background-color: #0078D4;
        color: white;
        font-size: 28pt;
        font-weight: bold;
        border-radius: 30px;  /* half of height */
        border: none;
    }
    QPushButton:hover {
        background-color: #3399FF;
    }
    QPushButton:pressed {
        background-color: #005A9E;
    }
)");

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
    ui->yearCombo->setEditable(true);
    ui->yearCombo->lineEdit()->setValidator(new QIntValidator(1000, 9999, this));
    ui->yearCombo->setMinimumWidth(80);  // Or try 100 if it's still tight

    QLocale locale;
    for (int m = 1; m <= 12; ++m)
        ui->monthCombo->addItem(locale.monthName(m), m);

    for (int d = 1; d <= 31; ++d)
        ui->dayCombo->addItem(QString::number(d), d);

    for (int h = 0; h < 24; ++h)
        ui->hourCombo->addItem(QString("%1").arg(h, 2, 10, QChar('0')), h);

    for (int m = 0; m < 60; ++m)
        ui->minuteCombo->addItem(QString("%1").arg(m, 2, 10, QChar('0')), m);

    ui->minuteCombo->setMinimumWidth(40);  // ensures 2-digit values are readable

    connect(ui->plusButton, &QPushButton::clicked, this, &MainWindow::onPlusButtonClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::handleCreateButton);
    connect(ui->editModeButton, &QPushButton::clicked, this, &MainWindow::toggleEditMode);
    connect(ui->chooseImageButton, &QPushButton::clicked, this, &MainWindow::handleImageSelection);
    loadCountdowns();
}

void MainWindow::handleImageSelection() {
    QString file = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.jpg *.jpeg)");
    if (!file.isEmpty()) {
        QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir imageDir(appDataDir + "/tile_images");
        if (!imageDir.exists()) imageDir.mkpath(".");

        QString destPath = imageDir.filePath(QFileInfo(file).fileName());
        if (QFile::exists(destPath)) {
            QFile::remove(destPath);  // 🔁 Overwrite if same-name image already exists
        }
        if (QFile::copy(file, destPath)) {
            selectedImagePath = destPath;
        } else {
            qWarning() << "Failed to copy image to" << destPath;
            selectedImagePath.clear();
        }
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
        applyCreateFrameStyle();
    }

    ui->createFrame->setVisible(!ui->createFrame->isVisible());
}

void MainWindow::handleCreateButton() {
    QString title = ui->titleInput->text().trimmed();
    if (title.isEmpty()) title = "Untitled Event";

    int year = ui->yearCombo->currentText().toInt();
    int month = ui->monthCombo->currentData().toInt();
    int day = ui->dayCombo->currentData().toInt();
    int hour = ui->hourCombo->currentData().toInt();
    int minute = ui->minuteCombo->currentData().toInt();
    QDateTime datetime(QDate(year, month, day), QTime(hour, minute));

    bool unhide = ui->unhideCheckBox->isChecked();
    bool blackText = ui->blackTextCheckBox->isChecked();

    if (editingTile) {
        editingTile->setTitle(title);
        editingTile->setTargetDateTime(datetime);
        editingTile->setUnhideAfterExpiry(unhide);
        editingTile->setUseBlackText(blackText);
        if (!selectedImagePath.isEmpty()) {
            editingTile->setBackgroundImage(selectedImagePath);
        }
        editingTile = nullptr;
        ui->plusButton->setText("+");
        ui->createButton->setText("Create Event");
        selectedImagePath.clear();
        saveCountdowns();
        refreshTileLayout();
    } else {
        handleCountdownCreated(title, datetime, unhide, blackText);

    }

    ui->titleInput->clear();
    ui->createFrame->setVisible(false);
}

void MainWindow::handleCountdownCreated(const QString &title, const QDateTime &target, bool unhide, bool blackText) {
    CountdownTile *tile = new CountdownTile(title, target);
    tile->setUnhideAfterExpiry(unhide);
    tile->setFixedSize(627, 353);
    tile->setUseBlackText(blackText);

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

    connect(tile, &CountdownTile::countdownExpired, this, [this](const QString &title) {
        trayIcon->showMessage("Countdown Finished", QString("'%1' has ended!").arg(title),
                              QSystemTrayIcon::Information, 8000);
    });

    connect(tile, &CountdownTile::requestDelete, this, &MainWindow::handleTileDeletion);

    connect(tile, &CountdownTile::requestEdit, this, [this, tile]() {
        editingTile = tile;

        ui->plusButton->setText("Cancel Edit");
        ui->createButton->setText("Save Event");
        ui->titleInput->setText(tile->getTitle());
        ui->blackTextCheckBox->setChecked(tile->getUseBlackText());

        QDateTime dt = tile->getTargetDateTime();
        ui->yearCombo->setCurrentText(QString::number(dt.date().year()));
        ui->monthCombo->setCurrentIndex(dt.date().month() - 1);
        ui->dayCombo->setCurrentIndex(dt.date().day() - 1);
        ui->hourCombo->setCurrentIndex(ui->hourCombo->findData(dt.time().hour()));
        ui->minuteCombo->setCurrentIndex(ui->minuteCombo->findData(dt.time().minute()));

        int w = 600, h = 250;
        int x = (this->width() - w) / 2;
        int y = (this->height() - h) / 2;
        ui->createFrame->setGeometry(x, y, w, h);
        ui->createFrame->raise();
        applyCreateFrameStyle();
        ui->createFrame->setVisible(true);
    });
    refreshTileLayout();
}

void MainWindow::saveCountdowns() {
    QList<CountdownTile*> tiles = CountdownTile::getAllTiles();

    // ✅ Don't overwrite a valid file with empty data
    if (tiles.isEmpty()) {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/countdowns.json";
        if (QFileInfo::exists(path) && QFileInfo(path).size() > 10) {
            qWarning() << "Prevented accidental overwrite: tile list is empty.";
            return;
        }
    }

    // ✅ Make a backup before saving
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString mainFile = savePath + "/countdowns.json";
    QString backupFile = savePath + "/countdowns_backup.json";

    if (QFile::exists(mainFile)) {
        QFile::remove(backupFile);
        QFile::copy(mainFile, backupFile);
    }

    // ✅ Build JSON
    QJsonArray array;
    for (CountdownTile* tile : tiles) {
        QJsonObject obj;
        obj["title"] = tile->getTitle();
        obj["datetime"] = tile->getTargetDateTime().toString(Qt::ISODate);
        obj["background"] = tile->getBackgroundImagePath();
        obj["unhideAfterExpiry"] = tile->getUnhideAfterExpiry();
        obj["blackText"] = tile->getUseBlackText();
        array.append(obj);
    }

    // ✅ Write to file
    QFile file(mainFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadCountdowns() {
    isLoading = true;

    QString savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(savePath + "/countdowns.json");
    if (!file.open(QIODevice::ReadOnly)) {
        isLoading = false;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonArray array = doc.array();
    QList<CountdownTile*> loadedTiles;

    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        QString title = obj["title"].toString();
        QDateTime dt = QDateTime::fromString(obj["datetime"].toString(), Qt::ISODate);
        QString bgPath = obj["background"].toString();

        CountdownTile *tile = new CountdownTile(title, dt);
        if (!bgPath.isEmpty())
            tile->setBackgroundImage(bgPath);
        tile->setUseBlackText(obj["blackText"].toBool());
        tile->setUnhideAfterExpiry(obj["unhideAfterExpiry"].toBool());

        tile->setFixedSize(627, 353);
        loadedTiles.append(tile);
        connect(tile, &CountdownTile::countdownExpired, this, [this](const QString &title) {
            trayIcon->showMessage(
                "Countdown Finished",
                QString("'%1' has ended!").arg(title),
                QSystemTrayIcon::Information,
                8000  // ms
                );
        });

        connect(tile, &CountdownTile::requestDelete, this, &MainWindow::handleTileDeletion);
        connect(tile, &CountdownTile::requestEdit, this, [this, tile]() {
            editingTile = tile;
            ui->plusButton->setText("Cancel Edit");
            ui->createButton->setText("Save Event");
            ui->titleInput->setText(tile->getTitle());
            ui->unhideCheckBox->setChecked(tile->getUnhideAfterExpiry());
            ui->blackTextCheckBox->setChecked(tile->getUseBlackText());

            QDateTime dt = tile->getTargetDateTime();
            ui->yearCombo->setCurrentText(QString::number(dt.date().year()));
            ui->monthCombo->setCurrentIndex(dt.date().month() - 1);
            ui->dayCombo->setCurrentIndex(dt.date().day() - 1);
            ui->hourCombo->setCurrentIndex(dt.time().hour());
            ui->minuteCombo->setCurrentIndex(ui->minuteCombo->findData(dt.time().minute()));

            int w = 600, h = 250;
            int x = (this->width() - w) / 2;
            int y = (this->height() - h) / 2;
            ui->createFrame->setGeometry(x, y, w, h);
            ui->createFrame->raise();
            applyCreateFrameStyle();
            ui->createFrame->setVisible(true);
        });
    }

    // 🔃 Sort tiles by date before adding to layout
    std::sort(loadedTiles.begin(), loadedTiles.end(), [](CountdownTile* a, CountdownTile* b) {
        return a->getTargetDateTime() < b->getTargetDateTime();
    });

    CountdownTile::getAllTiles().clear();
    tileCount = 0;

    for (CountdownTile* tile : loadedTiles) {
        CountdownTile::getAllTiles().append(tile);
        int col = tileCount % 3;
        int row = tileCount / 3;
        tileLayout->addWidget(tile, row, col);
        tileLayout->setAlignment(tile, Qt::AlignHCenter);
        tileCount++;
    }

    isLoading = false;
}

void MainWindow::toggleEditMode() {
    editMode = !editMode;

    for (CountdownTile* tile : CountdownTile::getAllTiles()) {
        tile->setDeleteButtonVisible(editMode);
        tile->setEditModeEnabled(editMode);

        // Close edit form if exiting edit mode
        if (!editMode && editingTile) {
            editingTile = nullptr;
            ui->createFrame->setVisible(false);
            ui->plusButton->setText("+");
            ui->createButton->setText("Create Event");
            ui->titleInput->clear();
            selectedImagePath.clear();
        }
    }

    ui->editModeButton->setText(editMode ? "Done" : "Edit Mode");
}

void MainWindow::handleTileDeletion(CountdownTile* tile) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Tile",
                                  "Are you sure you want to delete this tile?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    if (editingTile == tile) {
        editingTile = nullptr;
        ui->createFrame->setVisible(false);
        ui->plusButton->setText("+");
        ui->createButton->setText("Create Event");
        ui->titleInput->clear();
        selectedImagePath.clear();
    }

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

void MainWindow::applyCreateFrameStyle() {
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

void MainWindow::refreshTileLayout() {
    // Clear layout
    QLayoutItem* item;
    while ((item = tileLayout->takeAt(0)) != nullptr) {
        if (item->widget()) tileLayout->removeWidget(item->widget());
        delete item;
    }

    // Sort tiles by datetime
    QList<CountdownTile*>& tiles = CountdownTile::getAllTiles();
    std::sort(tiles.begin(), tiles.end(), [](CountdownTile* a, CountdownTile* b) {
        return a->getTargetDateTime() < b->getTargetDateTime();
    });

    // Re-add to layout
    tileCount = 0;
    for (CountdownTile* tile : tiles) {
        int col = tileCount % 3;
        int row = tileCount / 3;
        tileLayout->addWidget(tile, row, col);
        tileLayout->setAlignment(tile, Qt::AlignHCenter);
        tileCount++;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (trayIcon && trayIcon->isVisible()) {
        hide();           // 👈 just hide window
        event->ignore();  // 👈 do NOT quit app
    } else {
        QMainWindow::closeEvent(event);
    }
}
