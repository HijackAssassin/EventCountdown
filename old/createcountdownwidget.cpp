#include "createcountdownwidget.h"
#include "ui_createcountdownwidget.h"
#include <QDate>
#include <QTime>
#include <windows.h>
#include <QSettings>
#include <QLocale>
#include <QKeyEvent>

createcountdownwidget::createcountdownwidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::createcountdownwidget) {
    ui->setupUi(this);

    setWindowTitle("Create Countdown Event");
    setMinimumSize(600, 400);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    connect(ui->createButton, &QPushButton::clicked, this, &createcountdownwidget::handleCreateClicked);

    // Prevent duplicate emit if re-opened
    setModal(true);
    setWindowModality(Qt::ApplicationModal);

    if (isDarkModeEnabled()) {
        setStyleSheet("background-color: #1e1e1e; color: white;");
    } else {
        setStyleSheet("background-color: white; color: black;");
    }

    for (int y = 2024; y <= 2035; ++y)
        ui->yearCombo->addItem(QString::number(y), y);

    QLocale locale;
    for (int m = 1; m <= 12; ++m)
        ui->monthCombo->addItem(locale.monthName(m), m);

    for (int d = 1; d <= 31; ++d)
        ui->dayCombo->addItem(QString::number(d), d);

    for (int h = 0; h < 24; ++h)
        ui->hourCombo->addItem(QString("%1:00").arg(h, 2, 10, QChar('0')), h);
}

createcountdownwidget::~createcountdownwidget() {
    delete ui;
}

void createcountdownwidget::handleCreateClicked() {
    int year = ui->yearCombo->currentData().toInt();
    int month = ui->monthCombo->currentData().toInt();
    int day = ui->dayCombo->currentData().toInt();
    int hour = ui->hourCombo->currentData().toInt();
    QDateTime datetime(QDate(year, month, day), QTime(hour, 0));

    QString title = ui->titleInput->text().trimmed();
    if (title.isEmpty()) title = "TEST TILE";
    emit countdownCreated(title, datetime);
    qDebug() << "[createcountdownwidget] Emitted TEST TILE for:" << datetime.toString();
    close();
}

void createcountdownwidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QDialog::keyPressEvent(event);
    }
}

bool isDarkModeEnabled() {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}
