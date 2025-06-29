#ifndef OUTLINEDLABEL_H
#define OUTLINEDLABEL_H

#include <QLabel>
#include <QColor>
#include "countdowntile.cpp"

class OutlinedLabel : public QLabel {
    Q_OBJECT

public:
    explicit OutlinedLabel(const QString& text = "", QWidget* parent = nullptr);

    void setOutlineColor(const QColor& color);
    void setOutlineWidth(int width);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor outlineColor = Qt::black;
    int outlineWidth = 2;
};

#endif // OUTLINEDLABEL_H
