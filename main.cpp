#include "mainwindow.h"

#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

QSharedMemory sharedMemory("EventCountdownManagerAppInstance");

bool isAnotherInstanceRunning() {
    if (sharedMemory.attach()) {
        sharedMemory.detach(); // If it's stuck
        return true;
    }
    return !sharedMemory.create(1);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (isAnotherInstanceRunning()) {
        QMessageBox::warning(nullptr, "Already Running", "The app is already running in the background. Reopen from the system tray");
        return 0;
    }

    MainWindow w;
    w.show();

    return app.exec();
}

