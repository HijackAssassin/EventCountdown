#include "countdowntile.h"
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QPainter>
#include "outlinedlabel.h"
#include <QMouseEvent>

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

    titleLabel = new OutlinedLabel(title, this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    titleLabel->setStyleSheet(
        "font-size: 18pt;"
        "font-weight: bold;"
        "padding: 5px;"
        "background: none;"
        "border: none;"
        );

    dayLabel = new OutlinedLabel("0", this);
    hourLabel = new OutlinedLabel("00", this);
    minuteLabel = new OutlinedLabel("00", this);
    secondLabel = new OutlinedLabel("00", this);

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
    dayText = new OutlinedLabel("days", this);
    hourText = new OutlinedLabel("hours", this);
    minuteText = new OutlinedLabel("minutes", this);
    secondText = new OutlinedLabel("seconds", this);

    dayText->setAlignment(Qt::AlignCenter);
    dayText->setStyleSheet("color: white; background: none; border: none; margin-top: 0px; font-size: 14pt;");

    dayText->setAlignment(Qt::AlignCenter);
    dayText->setStyleSheet("color: white; background: none; border: none; font-size: 14pt;");

    for (QLabel* lbl : {hourText, minuteText, secondText}) {
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color: white; background: none; border: none; font-size: 14pt;");
    }

    grid->addWidget(dayText, 1, 0, Qt::AlignCenter);
    grid->addWidget(hourText, 1, 1, Qt::AlignCenter);
    grid->addWidget(minuteText, 1, 2, Qt::AlignCenter);
    grid->addWidget(secondText, 1, 3, Qt::AlignCenter);

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
    updateTextColor();
}

void CountdownTile::setBackgroundImage(const QString &imagePath) {
    backgroundImagePath = imagePath;
    cachedBackground = QPixmap();  // reset cache
    update();
}

void CountdownTile::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    if (!backgroundImagePath.isEmpty() && QFile::exists(backgroundImagePath)) {
        if (cachedBackground.isNull() || size() != lastSize) {
            QPixmap bg(backgroundImagePath);
            cachedBackground = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            lastSize = size();
        }
        QPoint center((width() - cachedBackground.width()) / 2, (height() - cachedBackground.height()) / 2);
        painter.drawPixmap(center, cachedBackground);
    } else {
        painter.fillRect(rect(), Qt::black);
    }

    QFrame::paintEvent(event);
}

void CountdownTile::updateCountdown() {
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = now.secsTo(targetDateTime);

    dayLabel->show();
    hourLabel->show();
    minuteLabel->show();
    secondLabel->show();

    for (QObject* child : children()) {
        if (auto* label = qobject_cast<QLabel*>(child)) {
            QString text = label->text().toLower();
            if (text == "days" || text == "hours" || text == "minutes" || text == "seconds") {
                label->show();
            }
        }
    }

    if (secs <= 0) {
        if (!unhideAfterExpiry) {
            // Hide numbers
            dayLabel->setVisible(false);
            hourLabel->setVisible(false);
            minuteLabel->setVisible(false);
            secondLabel->setVisible(false);
            dayText->setVisible(false);
            hourText->setVisible(false);
            minuteText->setVisible(false);
            secondText->setVisible(false);
            return;
        }

        // Show numbers and count up
        dayLabel->setVisible(true);
        hourLabel->setVisible(true);
        minuteLabel->setVisible(true);
        secondLabel->setVisible(true);
        dayText->setVisible(true);
        hourText->setVisible(true);
        minuteText->setVisible(true);
        secondText->setVisible(true);

        qint64 secsSince = targetDateTime.secsTo(now);
        int days = secsSince / 86400;
        int hours = (secsSince % 86400) / 3600;
        int minutes = (secsSince % 3600) / 60;
        int seconds = secsSince % 60;

        dayLabel->setText(QString::number(days));
        hourLabel->setText(QString("%1").arg(hours, 2, 10, QChar('0')));
        minuteLabel->setText(QString("%1").arg(minutes, 2, 10, QChar('0')));
        secondLabel->setText(QString("%1").arg(seconds, 2, 10, QChar('0')));
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
}

void CountdownTile::setShowCountUp(bool enabled) {
    showCountUp = enabled;
}

bool CountdownTile::getShowCountUp() const {
    return showCountUp;
}

void CountdownTile::setUnhideAfterExpiry(bool value) {
    unhideAfterExpiry = value;
    updateCountdown();  // Ensure state change triggers countdown display refresh
}

bool CountdownTile::getUnhideAfterExpiry() const {
    return unhideAfterExpiry;
}

QString CountdownTile::getBackgroundImagePath() const {
    return backgroundImagePath;
}

void CountdownTile::setUseBlackText(bool useBlack) {
    useBlackText = useBlack;
    updateTextColor();
}

bool CountdownTile::getUseBlackText() const {
    return useBlackText;
}

void CountdownTile::updateTextColor() {
    QString color = useBlackText ? "black" : "white";

    QString numStyle = QString("color: %1; font-size: %2pt; background: transparent; border: none;")
                           .arg(color);

    dayLabel->setStyleSheet(numStyle.arg(60));
    hourLabel->setStyleSheet(numStyle.arg(40));
    minuteLabel->setStyleSheet(numStyle.arg(40));
    secondLabel->setStyleSheet(numStyle.arg(40));

    QString labelStyle = QString("color: %1; background: none; border: none; font-size: 14pt;").arg(color);

    dayText->setStyleSheet(labelStyle);
    hourText->setStyleSheet(labelStyle);
    minuteText->setStyleSheet(labelStyle);
    secondText->setStyleSheet(labelStyle);
}
void CountdownTile::mousePressEvent(QMouseEvent *event) {
    if (!isEditMode) return;

    if (deleteButton && deleteButton->geometry().contains(event->pos()))
        return;  // Prevent conflict with delete

    emit requestEdit(this);
}

void CountdownTile::setEditModeEnabled(bool enabled) {
    isEditMode = enabled;
}
