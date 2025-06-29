#include "countdowntile.h"
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QPainter>

QTimer* CountdownTile::sharedTimer = nullptr;
QList<CountdownTile*> CountdownTile::allTiles;

QList<CountdownTile*>& CountdownTile::getAllTiles() {
    return allTiles;
}

CountdownTile::CountdownTile(const QString &title, const QDateTime &target, QWidget *parent)
    : QFrame(parent), targetDateTime(target) {
    setFrameShape(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(300, 168);  // 16:9 aspect ratio
    setMaximumSize(800, 450);
    setStyleSheet(
        "QFrame {"
        " background-color: black;"
        " border: 2px solid #444;"
        " background-position: center;"
        " background-repeat: no-repeat;"
        " background-size: cover;"
        "}"
        );

    titleLabel = new QLabel(title, this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 18pt;"
        "font-weight: bold;"
        "padding: 5px;"
        "background: none;"
        "border: none;"
        );

    dayLabel = new QLabel("0", this);
    hourLabel = new QLabel("00", this);
    minuteLabel = new QLabel("00", this);
    secondLabel = new QLabel("00", this);

    dayLabel->setStyleSheet(
        "color: white;"
        "font-size: 60pt;"
        "background: transparent;"
        "border: none;"
        // 👇 These are optional but helpful for removing hidden margins
        "padding: -10px;"
        "margin: 0px;"
        );
    dayLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    hourLabel->setStyleSheet(
        "color: white;"
        "font-size: 40pt;"
        "background: transparent;"
        "border: none;"
        // 👇 These are optional but helpful for removing hidden margins
        "padding: 0px;"
        "margin: 0px;"
        );
    hourLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    minuteLabel->setStyleSheet(
        "color: white;"
        "font-size: 40pt;"
        "background: transparent;"
        "border: none;"
        // 👇 These are optional but helpful for removing hidden margins
        "padding: 0px;"
        "margin: 0px;"
        );
    minuteLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    secondLabel->setStyleSheet(
        "color: white;"
        "font-size: 40pt;"
        "background: transparent;"
        "border: none;"
        // 👇 These are optional but helpful for removing hidden margins
        "padding: 0px;"
        "margin: 0px;"
        );
    secondLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 🧮 Unified grid layout for numbers and labels
    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(30);
    grid->setVerticalSpacing(4);
    grid->setAlignment(Qt::AlignCenter);

    // ⏱ Numbers row (row 0)
    grid->addWidget(dayLabel, 0, 0, Qt::AlignCenter);
    grid->addWidget(hourLabel, 0, 1, Qt::AlignCenter);
    grid->addWidget(minuteLabel, 0, 2, Qt::AlignCenter);
    grid->addWidget(secondLabel, 0, 3, Qt::AlignCenter);

    // 🏷️ Labels row (row 1)
    auto d = new QLabel("days", this);
    auto h = new QLabel("hours", this);
    auto m = new QLabel("minutes", this);
    auto s = new QLabel("seconds", this);

    d->setAlignment(Qt::AlignCenter);
    d->setStyleSheet("color: white; background: none; border: none; margin-top: 0px; font-size: 14pt;");

    for (QLabel* lbl : {h, m, s}) {
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color: white; background: none; border: none; margin-top: 0px; font-size: 14pt;");
    }

    grid->addWidget(d, 1, 0, Qt::AlignCenter);
    grid->addWidget(h, 1, 1, Qt::AlignCenter);
    grid->addWidget(m, 1, 2, Qt::AlignCenter);
    grid->addWidget(s, 1, 3, Qt::AlignCenter);

    auto layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addLayout(grid);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->setContentsMargins(10, 60, 60, 10);
    setLayout(layout);

    // 🧼 Delete button logic
    // Create delete button
    deleteButton = new QPushButton("❌", this);
    deleteButton->setFixedSize(30, 30);
    deleteButton->setStyleSheet(
        "QPushButton {"
        " background: none;"
        " border: none;"
        " color: black;"
        " font-size: 18pt;"
        " font-weight: bold;"
        " }"
        "QPushButton:hover {"
        " color: red;"
        " }"
        );
    deleteButton->hide();
    deleteButton->raise();  // Ensure it's above all other content

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        emit requestDelete(this);
    });

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        emit requestDelete(this);
    });

    // Inside constructor — don't insert into any layout
    deleteButton = new QPushButton("❌", this);
    deleteButton->setFixedSize(30, 30);
    deleteButton->setStyleSheet(
        "QPushButton {"
        " background: none;"
        " border: none;"
        " color: black;"
        " font-size: 18pt;"
        " font-weight: bold;"
        " }"
        "QPushButton:hover {"
        " color: red;"
        " }"
        );
    deleteButton->hide();
    deleteButton->raise();

    // ⋯ Edit button
    editButton = new QPushButton("⋯", this);
    editButton->setFixedSize(30, 30);
    editButton->setStyleSheet(
        "QPushButton {"
        " background: none;"
        " border: none;"
        " color: grey;"
        " font-size: 28pt;"
        " font-weight: bold;"
        " }"
        "QPushButton:hover {"
        " color: blue;"
        " }"
        );
    editButton->hide();
    editButton->raise();

    connect(editButton, &QPushButton::clicked, this, [this]() {
        emit requestEdit(this);  // ✅ pass the tile to mainwindow
    });

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        emit requestDelete(this);
    });

    if (!sharedTimer) {
        sharedTimer = new QTimer(this);
        connect(sharedTimer, &QTimer::timeout, []() {
            for (CountdownTile* tile : allTiles)
                tile->updateCountdown();
        });

        int msUntilNextSecond = 1000 - QTime::currentTime().msec();
        QTimer::singleShot(msUntilNextSecond, []() {
            sharedTimer->start(1000);
            for (CountdownTile* tile : allTiles)
                tile->updateCountdown();
        });
    }

    allTiles.append(this);
    updateCountdown();
}

void CountdownTile::setBackgroundImage(const QString &imagePath) {
    backgroundImagePath = imagePath;
    update(); // trigger repaint with new image
}

void CountdownTile::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (!backgroundImagePath.isEmpty() && QFile::exists(backgroundImagePath)) {
        QPixmap bg(backgroundImagePath);
        bg = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPoint center((width() - bg.width()) / 2, (height() - bg.height()) / 2);
        painter.drawPixmap(center, bg);
    } else {
        painter.fillRect(rect(), Qt::black); // solid black fallback
    }

    QFrame::paintEvent(event); // ensure children are drawn correctly
}

void CountdownTile::updateCountdown() {
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = now.secsTo(targetDateTime);

    if (secs <= 0) {
        dayLabel->setText("0");
        hourLabel->setText("00");
        minuteLabel->setText("00");
        secondLabel->setText("00");
        return;
    }

    int days = secs / 86400;
    int hours = (secs % 86400) / 3600;
    int minutes = (secs % 3600) / 60;
    int seconds = secs % 60;

    dayLabel->setText(QString::number(days));
    hourLabel->setText(QString("%1").arg(hours, 2, 10, QChar('0')));
    minuteLabel->setText(QString("%1").arg(minutes, 2, 10, QChar('0')));
    secondLabel->setText(QString("%1").arg(seconds, 2, 10, QChar('0')));
}

void CountdownTile::setDeleteButtonVisible(bool visible) {
    if (deleteButton) deleteButton->setVisible(visible);
    if (editButton)   editButton->setVisible(visible);
}

QString CountdownTile::getTitle() const {
    return titleLabel->text();
}

QDateTime CountdownTile::getTargetDateTime() const {
    return targetDateTime;
}

void CountdownTile::setTitle(const QString& title) {
    titleLabel->setText(title);
}

void CountdownTile::setTargetDateTime(const QDateTime& datetime) {
    targetDateTime = datetime;
    updateCountdown();
}

void CountdownTile::resizeEvent(QResizeEvent *event) {
    QFrame::resizeEvent(event);

    if (deleteButton) {
        int btnX = width() - deleteButton->width() - 10;
        deleteButton->move(btnX, 10);
    }

    if (editButton) {
        int btnX = width() - deleteButton->width() - editButton->width() - 15;
        editButton->move(btnX, 10);
    }
}

QString CountdownTile::getBackgroundImagePath() const {
    return backgroundImagePath;
}
