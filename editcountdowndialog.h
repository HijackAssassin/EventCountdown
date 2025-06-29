#ifndef EDITCOUNTDOWNDIALOG_H
#define EDITCOUNTDOWNDIALOG_H

#include <QDialog>
#include <QDateTime>

class QLineEdit;
class QDateTimeEdit;

class EditCountdownDialog : public QDialog {
    Q_OBJECT

public:
    EditCountdownDialog(const QString& title, const QDateTime& dateTime, QWidget* parent = nullptr);
    QString getTitle() const;
    QDateTime getDateTime() const;

private:
    QLineEdit* titleEdit;
    QDateTimeEdit* dateTimeEdit;
};

#endif // EDITCOUNTDOWNDIALOG_H
