// countdowntile.h
#ifndef COUNTDOWNTILE_H
#define COUNTDOWNTILE_H

#include <QFrame>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QPushButton>

class CountdownTile : public QFrame
{
    Q_OBJECT

public:
    void setEditModeEnabled(bool enabled);
    QPixmap cachedBackground;
    QSize lastSize;
    void updateTextColor();
    void setUseBlackText(bool enable);
    bool getUseBlackText() const;
    void setUnhideAfterExpiry(bool enabled);
    bool getUnhideAfterExpiry() const;
    void setShowCountUp(bool enabled);
    bool getShowCountUp() const;
    CountdownTile(const QString &title, const QDateTime &target, QWidget *parent = nullptr);
    static QList<CountdownTile*>& getAllTiles();
    QString getTitle() const;
    QDateTime getTargetDateTime() const;
    void setDeleteButtonVisible(bool visible);
    void setBackgroundImage(const QString &path);
    QString getBackgroundImage() const;
    QString getBackgroundImagePath() const;
    void setTitle(const QString& title);
    void setTargetDateTime(const QDateTime& datetime);
    void setEditButtonVisible(bool visible);

signals:
    void requestDelete(CountdownTile* tile);
    void requestEdit(CountdownTile* tile);
    void countdownExpired(const QString& title);  // 🔔 triggered when a countdown ends

private:
    bool useBlackText = false;
    bool unhideAfterExpiry = false;
    bool showCountUp = false;
    QLabel *titleLabel;
    QLabel *dayLabel;
    QLabel *hourLabel;
    QLabel *minuteLabel;
    QLabel *secondLabel;
    QDateTime targetDateTime;
    QPushButton* deleteButton;
    QString backgroundPath;
    QString backgroundImagePath;
    void updateCountdown();
    QString selectedImagePath;
    bool isEditMode = false;
    QLabel *dayText;
    QLabel *hourText;
    QLabel *minuteText;
    QLabel *secondText;
    bool hasNotified = false;

    static QTimer *sharedTimer;
    static QList<CountdownTile*> allTiles;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // COUNTDOWNTILE_H
