#include "editcountdowndialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QLabel>

EditCountdownDialog::EditCountdownDialog(const QString& title, const QDateTime& dateTime, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Edit Event");
    resize(300, 150);

    titleEdit = new QLineEdit(title);
    dateTimeEdit = new QDateTimeEdit(dateTime);
    dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:00");
    dateTimeEdit->setCalendarPopup(true);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Event Title:"));
    layout->addWidget(titleEdit);
    layout->addWidget(new QLabel("Target Date/Time:"));
    layout->addWidget(dateTimeEdit);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

QString EditCountdownDialog::getTitle() const {
    return titleEdit->text();
}

QDateTime EditCountdownDialog::getDateTime() const {
    return dateTimeEdit->dateTime();
}
