#include "./MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    setWindowTitle("USB Device Locker");
    resize(900, 600);

    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    setupStartScreen();
    setupTabs();

    stack->addWidget(startScreen);
    stack->addWidget(tabs);
    stack->setCurrentWidget(startScreen);

    // Switch to tabs after clicking "Open Dashboard"
    auto startBtn = startScreen->findChild<QPushButton*>("startBtn");
    if (startBtn) {
        connect(startBtn, &QPushButton::clicked, [this] {
            stack->setCurrentWidget(tabs);
        });
    }

    // Timer for auto-scanning
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &MainWindow::scan_usb);
    scanTimer->start(3000);

    WHITELIST = load_whitelist(whitelistFile);
}

MainWindow::~MainWindow() {}

void MainWindow::setupStartScreen() {
    startScreen = new QWidget;
    auto *layout = new QVBoxLayout(startScreen);

    QLabel *logo = new QLabel;
    logo->setPixmap(QIcon(":/icons/usb.png").pixmap(96, 96));
    logo->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("USB Device Locker");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #2d3748;");

    QPushButton *startBtn = new QPushButton("Open Dashboard");
    startBtn->setObjectName("startBtn");
    startBtn->setFixedHeight(40);
    startBtn->setStyleSheet("font-size: 18px; background: #3182ce; color: white; border-radius: 8px;");

    layout->addStretch();
    layout->addWidget(logo);
    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    startScreen->setStyleSheet("background: #f7fafc;");
}

void MainWindow::setupTabs() {
    tabs = new QTabWidget;

    // Dashboard tab
    QWidget *dashboard = new QWidget;
    QVBoxLayout *dashLayout = new QVBoxLayout(dashboard);

    // Action buttons
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *refreshBtn = new QPushButton("Refresh / Scan Now");
    QPushButton *addBtn = new QPushButton("Add to Whitelist");
    QPushButton *removeBtn = new QPushButton("Remove from Whitelist");
    QPushButton *ejectBtn = new QPushButton("Eject Device");
    QPushButton *toggleBtn = new QPushButton("Disable Protection");
    btnLayout->addWidget(refreshBtn);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(removeBtn);
    btnLayout->addWidget(ejectBtn);
    btnLayout->addWidget(toggleBtn);

    // USB Table
    usbTable = new QTableWidget(0, 4);
    usbTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    usbTable->setSelectionMode(QAbstractItemView::SingleSelection);
    usbTable->setHorizontalHeaderLabels({"Drive", "Label", "Serial", "Status"});
    usbTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    dashLayout->addLayout(btnLayout);
    dashLayout->addWidget(usbTable);

    // Whitelist Management tab
    QWidget *whitelist = new QWidget;
    QVBoxLayout *wlLayout = new QVBoxLayout(whitelist);
    wlLayout->addWidget(new QLabel("Manage Whitelist (coming soon)"));

    // Logs tab
    QWidget *logs = new QWidget;
    QVBoxLayout *logLayout = new QVBoxLayout(logs);
    logLayout->addWidget(new QLabel("Scan History / Logs (coming soon)"));

    // Settings tab
    QWidget *settings = new QWidget;
    QVBoxLayout *setLayout = new QVBoxLayout(settings);
    setLayout->addWidget(new QLabel("Settings (coming soon)"));

    // Help/About tab
    QWidget *help = new QWidget;
    QVBoxLayout *helpLayout = new QVBoxLayout(help);
    helpLayout->addWidget(new QLabel("USB Device Locker\nVersion 1.0\n(c) 2024"));

    tabs->addTab(dashboard, QIcon::fromTheme("view-dashboard"), "Dashboard");
    tabs->addTab(whitelist, QIcon::fromTheme("list-add"), "Whitelist");
    tabs->addTab(logs, QIcon::fromTheme("document-open"), "Logs");
    tabs->addTab(settings, QIcon::fromTheme("settings"), "Settings");
    tabs->addTab(help, QIcon::fromTheme("help-about"), "Help / About");

    // Button connections

    // Refresh
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::scan_usb);

    // Add to Whitelist
    connect(addBtn, &QPushButton::clicked, [this] {
        int selected = usbTable->currentRow();
        if (selected >= 0) {
            auto serialItem = usbTable->item(selected, 2);
            if (serialItem) {
                DWORD serial = serialItem->text().toULong(nullptr, 16);
                if (!WHITELIST.contains(serial)) {
                    save_to_whitelist(whitelistFile, serial);
                    WHITELIST.insert(serial);
                    QMessageBox::information(this, "Whitelist", "Device added to whitelist.");
                    scan_usb();
                } else {
                    QMessageBox::information(this, "Whitelist", "Device is already whitelisted.");
                }
            }
        }
    });

    // Remove from Whitelist
    connect(removeBtn, &QPushButton::clicked, [this] {
        int selected = usbTable->currentRow();
        if (selected >= 0) {
            auto serialItem = usbTable->item(selected, 2);
            if (serialItem) {
                DWORD serial = serialItem->text().toULong(nullptr, 16);
                if (WHITELIST.contains(serial)) {
                    WHITELIST.erase(serial);
                    // Rewrite whitelist file
                    std::ofstream file(whitelistFile, std::ios::trunc);
                    for (auto s : WHITELIST) file << std::hex << s << "\n";
                    QMessageBox::information(this, "Whitelist", "Device removed from whitelist.");
                    scan_usb();
                } else {
                    QMessageBox::information(this, "Whitelist", "Device is not in whitelist.");
                }
            }
        }
    });

    // Eject Device
    connect(ejectBtn, &QPushButton::clicked, [this] {
        int selected = usbTable->currentRow();
        if (selected >= 0) {
            auto driveItem = usbTable->item(selected, 0);
            if (driveItem) {
                char letter = driveItem->text().at(0).toLatin1();
                if (eject_usb_device(letter)) {
                    QMessageBox::information(this, "Eject", "Device ejected.");
                    scan_usb();
                } else {
                    QMessageBox::warning(this, "Eject", "Failed to eject device.");
                }
            }
        }
    });

    // Enable/Disable Protection
    connect(toggleBtn, &QPushButton::clicked, [this, toggleBtn] {
        protectionEnabled = !protectionEnabled;
        toggleBtn->setText(protectionEnabled ? "Disable Protection" : "Enable Protection");
        QMessageBox::information(this, "Protection", protectionEnabled ? "Protection enabled." : "Protection disabled.");
    });
}

void MainWindow::showDevice(char letter, const std::string &label, DWORD serial, bool allowed) {
    int row = usbTable->rowCount();
    usbTable->insertRow(row);
    usbTable->setItem(row, 0, new QTableWidgetItem(QString(letter) + ":\\"));
    usbTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(label)));
    usbTable->setItem(row, 2, new QTableWidgetItem(QString::number(serial, 16).toUpper()));
    usbTable->setItem(row, 3, new QTableWidgetItem(allowed ? "Allowed" : "Blocked"));
}

void MainWindow::scan_usb() {
    usbTable->setRowCount(0);

    auto drives = get_connected_drives();
    for (const auto &drive : drives) {
        bool allowed = WHITELIST.contains(drive.serial);
        showDevice(drive.letter, drive.label, drive.serial, allowed);

        if (!SEEN_THIS_SESSION.contains(drive.serial)) {
            SEEN_THIS_SESSION.insert(drive.serial);

            // Only show: "New device detected. Run virus scan?"
            QString msg = QString("New USB Device %1:\\ (%2) detected.\nWould you like to run a virus scan?")
                              .arg(drive.letter)
                              .arg(QString::fromStdString(drive.label));
            auto reply = QMessageBox::question(this, "New USB Device", msg,
                                               QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                if (scan_usb_for_threats(drive.letter)) {
                    // Threat found, ask to eject
                    auto ejectReply = QMessageBox::question(
                        this, "Threat Detected",
                        QString("Threat found on %1:\\. Eject device?").arg(drive.letter),
                        QMessageBox::Yes | QMessageBox::No
                    );
                    if (ejectReply == QMessageBox::Yes) {
                        eject_usb_device(drive.letter);
                        QMessageBox::information(this, "Eject", "Device ejected.");
                        scan_usb();
                        return;
                    }
                } else {
                    QMessageBox::information(this, "Scan Complete", "No threats found.");
                }
            }
            // If No, do nothing (user can scan later)
        }
    }
}