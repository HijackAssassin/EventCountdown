// outlinedlabel.h
#ifndef OUTLINEDLABEL_H
#define OUTLINEDLABEL_H

#include <QLabel>
#include <QPainter>
#include <QPainterPath>

class OutlinedLabel : public QLabel {
public:
    explicit OutlinedLabel(const QString &text = "", QWidget *parent = nullptr)
        : QLabel(text, parent) {}

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter p(this);
        QFont f = font();
        p.setRenderHint(QPainter::Antialiasing);
        p.setFont(f);

        // Set up outline path
        QPainterPath path;
        path.addText(0, height() * 0.8, f, text());

        // Draw black outline
        p.setPen(QPen(Qt::black, 3));
        p.drawPath(path);

        // Draw actual text
        p.setPen(QPen(palette().color(QPalette::WindowText), 1));
        p.drawText(0, height() * 0.8, text());
    }
};

#endif // OUTLINEDLABEL_H
