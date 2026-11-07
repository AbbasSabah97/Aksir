#include "spreadsheetwindow.h"
#include "spreadsheetmodel.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QToolButton>
#include <QColorDialog>
#include <QFontComboBox>
#include <QComboBox>
#include <QFrame>
#include <QApplication>

SpreadsheetWindow::SpreadsheetWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("أكسير - محرر الجداول");
    resize(1200, 800);

    m_model = new SpreadsheetModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setDefaultSectionSize(100);
    m_tableView->verticalHeader()->setDefaultSectionSize(25);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setCurrentIndex(m_model->index(0, 0));

    // شريط التبويبات (Ribbon)
    m_ribbonTabs = new QTabWidget(this);
    m_ribbonTabs->setFixedHeight(100); // تصغير ارتفاع الشريط
    setupRibbon();

    // شريط الصيغة
    QWidget *formulaWidget = new QWidget(this);
    QHBoxLayout *formulaLayout = new QHBoxLayout(formulaWidget);
    formulaLayout->setContentsMargins(5, 2, 5, 2);
    
    m_cellLabel = new QLabel("A1", this);
    m_cellLabel->setMinimumWidth(60);
    m_cellLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *fxLabel = new QLabel("fx", this);
    fxLabel->setStyleSheet("font-style: italic;");
    m_formulaBar = new QLineEdit(this);
    m_formulaBar->setStyleSheet("font-size: 14px; padding: 2px;");
    
    formulaLayout->addWidget(m_cellLabel);
    formulaLayout->addWidget(fxLabel);
    formulaLayout->addWidget(m_formulaBar);

    // التجميع الرئيسي
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_ribbonTabs);
    mainLayout->addWidget(formulaWidget);
    mainLayout->addWidget(m_tableView);
    setCentralWidget(centralWidget);

    setupStatusBar();

    // إضافة زر تبديل الوضع (Light/Dark) في شريط علوي جديد
    QToolBar *topToolbar = addToolBar("إعدادات");
    topToolbar->setMovable(false);
    QAction *themeAction = topToolbar->addAction("🌓 تبديل الوضع (فاتح/داكن)");
    connect(themeAction, &QAction::triggered, this, &SpreadsheetWindow::toggleTheme);

    // ربط الأحداث
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &SpreadsheetWindow::updateFormulaBar);
    connect(m_formulaBar, &QLineEdit::returnPressed,
            this, &SpreadsheetWindow::commitFormula);

    // تطبيق الوضع الفاتح افتراضياً
    applyTheme(false);
}

// =========================================================
// هذا هو الكود الذي كان ناقصاً وتسبب في الخطأ (Unresolved External Symbol)
// =========================================================
void SpreadsheetWindow::toggleTheme()
{
    m_isDarkMode = !m_isDarkMode;
    applyTheme(m_isDarkMode);
}

void SpreadsheetWindow::applyTheme(bool isDark)
{
    if (isDark) {
        // الوضع الداكن
        QString darkStyle = R"(
            QMainWindow, QWidget { background-color: #1e1e1e; color: #ffffff; }
            QTableView { background-color: #252526; gridline-color: #3e3e42; color: #ffffff; selection-background-color: #094771; selection-color: #ffffff; }
            QHeaderView::section { background-color: #2d2d30; color: #cccccc; border: 1px solid #3e3e42; padding: 4px; }
            QLineEdit { background-color: #3c3c3c; color: #ffffff; border: 1px solid #555555; }
            QLabel { color: #ffffff; }
            QToolButton { background: transparent; color: #ffffff; border: 1px solid transparent; border-radius: 3px; padding: 4px; }
            QToolButton:hover { background: #3e3e42; border: 1px solid #555555; }
            QToolButton:pressed { background: #555555; }
            QComboBox { background-color: #3c3c3c; color: #ffffff; border: 1px solid #555555; }
            QComboBox QAbstractItemView { background-color: #3c3c3c; color: #ffffff; }
            QTabWidget::pane { border: 1px solid #3e3e42; background: #1e1e1e; }
            QTabBar::tab { background: #1e1e1e; color: #aaaaaa; padding: 6px 15px; }
            QTabBar::tab:selected { color: #ffffff; border-bottom: 2px solid #0e639c; }
            QStatusBar { background: #1e1e1e; color: #ffffff; border-top: 1px solid #3e3e42; }
            QFrame { border-right: 1px solid #3e3e42; }
            QToolBar { background: #2d2d30; border-bottom: 1px solid #3e3e42; }
        )";
        qApp->setStyleSheet(darkStyle);
    } else {
        // الوضع الفاتح
        QString lightStyle = R"(
            QMainWindow, QWidget { background-color: #f3f2f1; color: #000000; }
            QTableView { background-color: #ffffff; gridline-color: #e1dfdd; color: #000000; selection-background-color: #cce8ff; selection-color: #000000; }
            QHeaderView::section { background-color: #f3f2f1; color: #000000; border: 1px solid #d2d0ce; padding: 4px; }
            QLineEdit { background-color: #ffffff; color: #000000; border: 1px solid #d2d0ce; }
            QLabel { color: #000000; }
            QToolButton { background: transparent; color: #000000; border: 1px solid transparent; border-radius: 3px; padding: 4px; }
            QToolButton:hover { background: #e1dfdd; border: 1px solid #c8c6c4; }
            QToolButton:pressed { background: #d2d0ce; }
            QComboBox { background-color: #ffffff; color: #000000; border: 1px solid #d2d0ce; }
            QComboBox QAbstractItemView { background-color: #ffffff; color: #000000; }
            QTabWidget::pane { border: 1px solid #d2d0ce; background: #f3f2f1; }
            QTabBar::tab { background: #f3f2f1; color: #605e5c; padding: 6px 15px; }
            QTabBar::tab:selected { color: #107c41; border-bottom: 2px solid #107c41; }
            QStatusBar { background: #f3f2f1; color: #000000; border-top: 1px solid #d2d0ce; }
            QFrame { border-right: 1px solid #d2d0ce; }
            QToolBar { background: #f3f2f1; border-bottom: 1px solid #d2d0ce; }
        )";
        qApp->setStyleSheet(lightStyle);
    }
    
    // تحديث الجدول لتطبيق الألوان الجديدة
    m_model->recalculateAll();
}

void SpreadsheetWindow::setupRibbon()
{
    QWidget *homeTab = new QWidget();
    QHBoxLayout *homeLayout = new QHBoxLayout(homeTab);
    homeLayout->setContentsMargins(5, 5, 5, 5);
    homeLayout->setSpacing(10);

    // مجموعة الخطوط (Font Group)
    QFrame *fontFrame = new QFrame();
    QVBoxLayout *fontLayout = new QVBoxLayout(fontFrame);
    fontLayout->setContentsMargins(0, 0, 10, 0);
    fontLayout->setSpacing(2);
    
    QHBoxLayout *fontToolbar = new QHBoxLayout();
    fontToolbar->setSpacing(2);
    QFontComboBox *fontCombo = new QFontComboBox();
    fontCombo->setFixedWidth(150);
    QComboBox *sizeCombo = new QComboBox();
    sizeCombo->addItems({"8", "9", "10", "11", "12", "14", "16", "18", "24", "36"});
    sizeCombo->setCurrentText("11");
    sizeCombo->setFixedWidth(50);

    QToolButton *btnBold = new QToolButton(); btnBold->setText("B"); btnBold->setStyleSheet("font-weight: bold;");
    QToolButton *btnItalic = new QToolButton(); btnItalic->setText("I"); btnItalic->setStyleSheet("font-style: italic;");
    QToolButton *btnUnderline = new QToolButton(); btnUnderline->setText("U"); btnUnderline->setStyleSheet("text-decoration: underline;");
    QToolButton *btnTextColor = new QToolButton(); btnTextColor->setText("A"); btnTextColor->setStyleSheet("color: red; font-weight: bold;");
    QToolButton *btnBgColor = new QToolButton(); btnBgColor->setText("🖌"); 

    fontToolbar->addWidget(fontCombo);
    fontToolbar->addWidget(sizeCombo);
    fontToolbar->addWidget(btnBold);
    fontToolbar->addWidget(btnItalic);
    fontToolbar->addWidget(btnUnderline);
    fontToolbar->addWidget(btnTextColor);
    fontToolbar->addWidget(btnBgColor);
    fontLayout->addLayout(fontToolbar);
    QLabel *fontLabel = new QLabel("Font");
    fontLabel->setStyleSheet("color: gray; font-size: 10px;");
    fontLayout->addWidget(fontLabel, 0, Qt::AlignCenter);

    connect(btnBold, &QToolButton::clicked, this, [this, btnBold]() {
        applyFormat(0, !btnBold->property("active").toBool()); btnBold->setProperty("active", !btnBold->property("active").toBool());
    });
    connect(btnItalic, &QToolButton::clicked, this, [this, btnItalic]() {
        applyFormat(1, !btnItalic->property("active").toBool()); btnItalic->setProperty("active", !btnItalic->property("active").toBool());
    });
    connect(btnUnderline, &QToolButton::clicked, this, [this, btnUnderline]() {
        applyFormat(2, !btnUnderline->property("active").toBool()); btnUnderline->setProperty("active", !btnUnderline->property("active").toBool());
    });
    connect(btnTextColor, &QToolButton::clicked, this, [this]() { pickColor(4); });
    connect(btnBgColor, &QToolButton::clicked, this, [this]() { pickColor(3); });

    // مجموعة المحاذاة (Alignment Group)
    QFrame *alignFrame = new QFrame();
    QVBoxLayout *alignLayout = new QVBoxLayout(alignFrame);
    alignLayout->setContentsMargins(0, 0, 10, 0);
    alignLayout->setSpacing(2);
    QHBoxLayout *alignToolbar = new QHBoxLayout();
    alignToolbar->setSpacing(2);
    
    QToolButton *btnAlignLeft = new QToolButton(); btnAlignLeft->setText("≡");
    QToolButton *btnAlignCenter = new QToolButton(); btnAlignCenter->setText("≡"); btnAlignCenter->setStyleSheet("qproperty-alignment: AlignCenter;");
    QToolButton *btnAlignRight = new QToolButton(); btnAlignRight->setText("≡"); btnAlignRight->setStyleSheet("qproperty-alignment: AlignRight;");
    
    alignToolbar->addWidget(btnAlignLeft);
    alignToolbar->addWidget(btnAlignCenter);
    alignToolbar->addWidget(btnAlignRight);
    alignLayout->addLayout(alignToolbar);
    QLabel *alignLabel = new QLabel("Alignment");
    alignLabel->setStyleSheet("color: gray; font-size: 10px;");
    alignLayout->addWidget(alignLabel, 0, Qt::AlignCenter);

    connect(btnAlignLeft, &QToolButton::clicked, this, [this]() { applyFormat(5, (int)(Qt::AlignLeft | Qt::AlignVCenter)); });
    connect(btnAlignCenter, &QToolButton::clicked, this, [this]() { applyFormat(5, (int)(Qt::AlignCenter)); });
    connect(btnAlignRight, &QToolButton::clicked, this, [this]() { applyFormat(5, (int)(Qt::AlignRight | Qt::AlignVCenter)); });

    // مجموعة الأرقام (Number Group)
    QFrame *numFrame = new QFrame();
    QVBoxLayout *numLayout = new QVBoxLayout(numFrame);
    numLayout->setContentsMargins(0, 0, 10, 0);
    numLayout->setSpacing(2);
    QHBoxLayout *numToolbar = new QHBoxLayout();
    numToolbar->setSpacing(2);
    
    QComboBox *formatCombo = new QComboBox();
    formatCombo->addItems({"General", "Currency", "Percent"});
    formatCombo->setFixedWidth(100);
    
    numToolbar->addWidget(formatCombo);
    numLayout->addLayout(numToolbar);
    QLabel *numLabel = new QLabel("Number");
    numLabel->setStyleSheet("color: gray; font-size: 10px;");
    numLayout->addWidget(numLabel, 0, Qt::AlignCenter);

    connect(formatCombo, &QComboBox::currentTextChanged, this, [this](const QString &text) {
        applyFormat(6, text);
    });

    homeLayout->addWidget(fontFrame);
    homeLayout->addWidget(alignFrame);
    homeLayout->addWidget(numFrame);
    homeLayout->addStretch();

    m_ribbonTabs->addTab(homeTab, "Home");
    m_ribbonTabs->addTab(new QWidget(), "Insert");
    m_ribbonTabs->addTab(new QWidget(), "Page Layout");
    m_ribbonTabs->addTab(new QWidget(), "Formulas");
    m_ribbonTabs->addTab(new QWidget(), "Data");
}

void SpreadsheetWindow::setupStatusBar()
{
    QStatusBar *statusBar = this->statusBar();
    
    QLabel *readyLabel = new QLabel("Ready", this);
    QSlider *zoomSlider = new QSlider(Qt::Horizontal, this);
    zoomSlider->setRange(50, 200);
    zoomSlider->setValue(100);
    zoomSlider->setFixedWidth(150);
    QLabel *zoomLabel = new QLabel("100%", this);

    statusBar->addWidget(readyLabel);
    statusBar->addPermanentWidget(new QLabel("Accessibility: Good to go", this));
    statusBar->addPermanentWidget(zoomSlider);
    statusBar->addPermanentWidget(zoomLabel);

    connect(zoomSlider, &QSlider::valueChanged, this, [this, zoomLabel](int value) {
        zoomLabel->setText(QString::number(value) + "%");
        QFont font = m_tableView->font();
        font.setPointSize(11 * (value / 100.0));
        m_tableView->setFont(font);
        m_tableView->verticalHeader()->setDefaultSectionSize(25 * (value / 100.0));
    });
}

void SpreadsheetWindow::applyFormat(int formatType, const QVariant &value)
{
    QModelIndexList selected = m_tableView->selectionModel()->selectedIndexes();
    if (selected.isEmpty() && m_tableView->currentIndex().isValid()) {
        selected.append(m_tableView->currentIndex());
    }
    m_model->updateCellFormat(selected, formatType, value);
}

void SpreadsheetWindow::pickColor(int formatType)
{
    QColor color = QColorDialog::getColor(Qt::white, this, "اختر اللون");
    if (color.isValid()) {
        applyFormat(formatType, color);
    }
}

void SpreadsheetWindow::updateFormulaBar(const QModelIndex &current)
{
    if (!current.isValid()) return;
    QString col = QString(QChar('A' + current.column()));
    m_cellLabel->setText(col + QString::number(current.row() + 1));
    m_formulaBar->setText(m_model->getRawValue(current));
}

void SpreadsheetWindow::commitFormula()
{
    QModelIndex current = m_tableView->currentIndex();
    if (current.isValid()) {
        m_model->setData(current, m_formulaBar->text(), Qt::EditRole);
    }
}
