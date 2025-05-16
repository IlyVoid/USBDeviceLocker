#include "./MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    setWindowTitle("USB Device Locker");
    setWindowIcon(QIcon(":/icons/usb.png"));
    resize(900, 600);

    setupUI();

    // Timer for auto-scanning
    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &MainWindow::scan_usb);
    scanTimer->start(3000);

    WHITELIST = load_whitelist(whitelistFile);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *central = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    // Sidebar navigation
    QListWidget *sidebar = new QListWidget;
    sidebar->setIconSize(QSize(32, 32));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/usb.png"), "Home"));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/dashboard.png"), "Dashboard"));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/whitelist.png"), "Whitelist"));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/logs.png"), "Logs"));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/settings.png"), "Settings"));
    sidebar->addItem(new QListWidgetItem(QIcon(":/icons/help.png"), "Help / About"));
    sidebar->setFixedWidth(180);
    sidebar->setCurrentRow(0);
    sidebar->setStyleSheet(R"(
        QListWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #23272e, stop:1 #181a20);
            border: none;
            color: #e3f2fd;
            font-size: 18px;
            font-weight: 600;
            outline: none;
        }
        QListWidget::item {
            padding: 22px 18px;
            border-radius: 12px;
            margin: 6px 0;
            transition: background 0.2s;
        }
        QListWidget::item:selected {
            background: #00bcd4;
            color: #181a20;
            font-weight: bold;
            border: 2px solid #00bcd4;
            box-shadow: 0 2px 12px #00bcd488;
        }
        QListWidget::item:hover {
            background: #263238;
            color: #fff;
        }
    )");

    // Stacked pages
    stack = new QStackedWidget;
    stack->addWidget(createHomeTab());
    stack->addWidget(createDashboardTab());
    stack->addWidget(createWhitelistTab());
    stack->addWidget(createLogsTab());
    stack->addWidget(createSettingsTab());
    stack->addWidget(createHelpTab());

    // Card-like main area
    QWidget *mainCard = new QWidget;
    QVBoxLayout *cardLayout = new QVBoxLayout(mainCard);
    cardLayout->setContentsMargins(32, 32, 32, 32);
    cardLayout->addWidget(stack);
    mainCard->setStyleSheet(R"(
        background: #23272e;
        border-radius: 24px;
        box-shadow: 0 8px 32px #00000044;
    )");

    mainLayout->addWidget(sidebar);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(mainCard, 1);

    central->setStyleSheet("background: #181a20;");
    setCentralWidget(central);

    // Navigation logic
    connect(sidebar, &QListWidget::currentRowChanged, stack, &QStackedWidget::setCurrentIndex);
}

QWidget* MainWindow::createHomeTab() {
    QWidget *home = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(home);

    home->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel *logo = new QLabel;
    logo->setPixmap(QIcon(":/icons/usb.png").pixmap(96, 96));
    logo->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("USB Device Locker");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 32px; font-weight: bold;");

    QLabel *subtitle = new QLabel("The safety net for your USB devices");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("font-size: 16px; color: #aaa;");

    QLabel *desc = new QLabel("Monitor, scan, and manage your USB devices in real time.\n"
                              "Use the navigation on the left to access the dashboard, whitelist, logs, and settings.");
    desc->setAlignment(Qt::AlignCenter);
    desc->setStyleSheet("font-size: 13px; color: #bbb;");

    layout->addStretch();
    layout->addWidget(logo);
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(10);
    layout->addWidget(desc);
    layout->addStretch();

    return home;
}

QWidget* MainWindow::createDashboardTab() {
    QWidget *dashboard = new QWidget;
    QVBoxLayout *dashLayout = new QVBoxLayout(dashboard);

    // Action buttons
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *refreshBtn = new QPushButton(QIcon::fromTheme("view-refresh"), "Refresh / Scan Now");
    QPushButton *addBtn = new QPushButton(QIcon::fromTheme("list-add"), "Add to Whitelist");
    QPushButton *removeBtn = new QPushButton(QIcon::fromTheme("list-remove"), "Remove from Whitelist");
    QPushButton *ejectBtn = new QPushButton(QIcon::fromTheme("media-eject"), "Eject Device");
    QPushButton *toggleBtn = new QPushButton(QIcon::fromTheme("security-high"), "Disable Protection");

    QList<QPushButton*> btns = {refreshBtn, addBtn, removeBtn, ejectBtn, toggleBtn};
    for (auto btn : btns) {
        btn->setStyleSheet(R"(
            QPushButton {
                background: #31363f;
                color: #fff;
                border: none;
                border-radius: 6px;
                padding: 10px 24px;
                font-size: 15px;
            }
            QPushButton:hover {
                background: #00bcd4;
                color: #23272e;
            }
        )");
        btnLayout->addWidget(btn);
    }

    // USB Table
    usbTable = new QTableWidget(0, 4);
    usbTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    usbTable->setSelectionMode(QAbstractItemView::SingleSelection);
    usbTable->setHorizontalHeaderLabels({"Drive", "Label", "Serial", "Status"});
    usbTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    usbTable->setAlternatingRowColors(true);
    usbTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    usbTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    usbTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    usbTable->setStyleSheet(R"(
        QTableWidget, QHeaderView::section {
            background: #23272e;
            color: #cfd8dc;
            font-size: 15px;
            border-radius: 8px;
        }
        QHeaderView::section {
            background: #31363f;
            color: #fff;
            font-weight: bold;
            border: none;
            padding: 8px;
        }
        QTableWidget::item:selected {
            background: #00bcd4;
            color: #23272e;
        }
    )");

    dashLayout->addLayout(btnLayout);
    dashLayout->addWidget(usbTable);
    dashboard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

    return dashboard;
}

QWidget* MainWindow::createWhitelistTab() {
    QWidget *whitelist = new QWidget;
    QVBoxLayout *wlLayout = new QVBoxLayout(whitelist);
    wlLayout->addWidget(new QLabel("Manage Whitelist (coming soon)"));
    whitelist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return whitelist;
}

QWidget* MainWindow::createLogsTab() {
    QWidget *logs = new QWidget;
    QVBoxLayout *logLayout = new QVBoxLayout(logs);
    logLayout->addWidget(new QLabel("Scan History / Logs (coming soon)"));
    logs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return logs;
}

QWidget* MainWindow::createSettingsTab() {
    QWidget *settings = new QWidget;
    QVBoxLayout *setLayout = new QVBoxLayout(settings);
    setLayout->addWidget(new QLabel("Settings (coming soon)"));
    settings->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return settings;
}

QWidget* MainWindow::createHelpTab() {
    QWidget *help = new QWidget;
    QVBoxLayout *helpLayout = new QVBoxLayout(help);
    QLabel *about = new QLabel("USB Device Locker\nVersion 1.0\n(c) 2024");
    about->setAlignment(Qt::AlignCenter);
    helpLayout->addWidget(about);
    help->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return help;
}

void MainWindow::showDevice(char letter, const std::string &label, DWORD serial, bool allowed) {
    int row = usbTable->rowCount();
    usbTable->insertRow(row);
    usbTable->setItem(row, 0, new QTableWidgetItem(QString(letter) + ":\\"));
    usbTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(label)));
    usbTable->setItem(row, 2, new QTableWidgetItem(QString::number(serial, 16).toUpper()));
    auto statusItem = new QTableWidgetItem(allowed ? "Allowed" : "Blocked");
    statusItem->setForeground(allowed ? Qt::green : Qt::red);
    usbTable->setItem(row, 3, statusItem);
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