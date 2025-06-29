#include "OutlinedLabel.h"
#include <QPainter>
#include <QStyleOption>

OutlinedLabel::OutlinedLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent) {}

void OutlinedLabel::setOutlineColor(const QColor& color) {
    outlineColor = color;
    update();
}

void OutlinedLabel::setOutlineWidth(int width) {
    outlineWidth = width;
    update();
}

void OutlinedLabel::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QFont f = font();
    painter.setFont(f);

    QPen pen(outlineColor);
    pen.setWidth(outlineWidth);
    painter.setPen(pen);

    // Outline
    for (int dx = -outlineWidth; dx <= outlineWidth; ++dx) {
        for (int dy = -outlineWidth; dy <= outlineWidth; ++dy) {
            if (dx == 0 && dy == 0) continue;
            painter.drawText(rect().adjusted(dx, dy, dx, dy), alignment(), text());
        }
    }

    // Main text (foreground)
    painter.setPen(QPen(palette().color(QPalette::WindowText), 1));
    painter.drawText(rect(), alignment(), text());
}
