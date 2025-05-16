#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QStyle>
#include "../class/main.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool protectionEnabled = true;

private slots:
    void scan_usb();

private:
    QTimer *scanTimer;
    QTableWidget *usbTable;
    QStackedWidget *stack;
    QWidget *startScreen;
    QTabWidget *tabs;

    void setupUI();
    void showDevice(char letter, const std::string& label, DWORD serial, bool allowed);

    void setupStartScreen();
    void setupTabs();
};

#endif // MAINWINDOW_H
