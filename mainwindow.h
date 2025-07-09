#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "countdowntile.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QSystemTrayIcon>  // ✅ tray icon
#include <QMenu>            // ✅ tray menu
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;  // ✅ intercept close event

private slots:
    void applyCreateFrameStyle();
    void handleCreateButton();
    void onPlusButtonClicked();
    void handleCountdownCreated(const QString &title, const QDateTime &target, bool unhide, bool blackText);
    void toggleEditMode();
    void handleTileDeletion(CountdownTile* tile);
    void handleImageSelection();
    void handleImageButton();

private:
    Ui::MainWindow *ui;
    QGridLayout *tileLayout = nullptr;
    int tileCount = 0;
    void saveCountdowns();
    void loadCountdowns();
    bool isLoading = false;
    bool editMode = false;
    QString selectedImagePath;  // stores the image copied into the app folder
    CountdownTile* editingTile = nullptr;
    void refreshTileLayout();
    QSystemTrayIcon *trayIcon;  // ✅ tray icon handle
    QMenu *trayMenu;            // ✅ right-click menu
};

#endif // MAINWINDOW_H
