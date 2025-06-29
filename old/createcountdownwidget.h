#ifndef CREATECOUNTDOWNWIDGET_H
#define CREATECOUNTDOWNWIDGET_H

#include <QDialog>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class createcountdownwidget; }
QT_END_NAMESPACE

class createcountdownwidget : public QDialog {
    Q_OBJECT

public:
    explicit createcountdownwidget(QWidget *parent = nullptr);
    ~createcountdownwidget();

signals:
    void countdownCreated(const QString &title, const QDateTime &dateTime);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleCreateClicked();

private:
    Ui::createcountdownwidget *ui;
};

bool isDarkModeEnabled();

#endif // CREATECOUNTDOWNWIDGET_H
