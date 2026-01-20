#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFileInfo>

QSharedMemory sharedMemory("EventCountdownManagerAppInstance");

bool isAnotherInstanceRunning() {
    if (sharedMemory.attach()) {
        sharedMemory.detach(); // If it's stuck
        return true;
    }
    return !sharedMemory.create(1);
}

void launchNotifierIfExists() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString notifierPath =
        QDir(appDir).filePath("Notifier/EventCountdownsNotifier.exe");

    if (QFileInfo::exists(notifierPath)) {
        QProcess::startDetached(notifierPath);
    } else {
        qWarning() << "Notifier not found at:" << notifierPath;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (isAnotherInstanceRunning()) {
        QMessageBox::warning(nullptr,
                             "Already Running",
                             "The app is already running.");
        return 0;
    }

    launchNotifierIfExists();  // 👈 START NOTIFIER

    MainWindow w;
    w.show();

    return app.exec();
}
