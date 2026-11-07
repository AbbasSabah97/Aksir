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

SpreadsheetWindow::SpreadsheetWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("أكسير - محرر الجداول");
    resize(1200, 800);
    setStyleSheet("QMainWindow { background-color: #f3f2f1; } "
                  "QTableView { background-color: white; gridline-color: #e1dfdd; selection-background-color: #cce8ff; selection-color: black; } "
                  "QHeaderView::section { background-color: #f3f2f1; border: 1px solid #d2d0ce; padding: 2px; }");

    m_model = new SpreadsheetModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setDefaultSectionSize(100);
    m_tableView->verticalHeader()->setDefaultSectionSize(25);
    m_tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_tableView->setCurrentIndex(m_model->index(0, 0));

    // 1. إعداد شريط التبويبات (Ribbon Tabs)
    m_ribbonTabs = new QTabWidget(this);
    m_ribbonTabs->setStyleSheet("QTabWidget::pane { border: 1px solid #d2d0ce; background: #f3f2f1; } "
                                "QTabBar::tab { padding: 6px 15px; background: #f3f2f1; border: none; } "
                                "QTabBar::tab:selected { color: #107c41; border-bottom: 2px solid #107c41; }");
    setupRibbon();

    // 2. شريط الصيغة (Formula Bar)
    QWidget *formulaWidget = new QWidget(this);
    formulaWidget->setStyleSheet("background-color: white; border-bottom: 1px solid #d2d0ce;");
    QHBoxLayout *formulaLayout = new QHBoxLayout(formulaWidget);
    formulaLayout->setContentsMargins(5, 2, 5, 2);
    
    m_cellLabel = new QLabel("A1", this);
    m_cellLabel->setMinimumWidth(60);
    m_cellLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *fxLabel = new QLabel("fx", this);
    fxLabel->setStyleSheet("color: gray; font-style: italic;");
    m_formulaBar = new QLineEdit(this);
    m_formulaBar->setStyleSheet("border: none; font-size: 14px;");
    
    formulaLayout->addWidget(m_cellLabel);
    formulaLayout->addWidget(fxLabel);
    formulaLayout->addWidget(m_formulaBar);

    // 3. التجميع الرئيسي
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_ribbonTabs);
    mainLayout->addWidget(formulaWidget);
    mainLayout->addWidget(m_tableView);
    setCentralWidget(centralWidget);

    setupStatusBar();

    // 4. الأحداث
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &SpreadsheetWindow::updateFormulaBar);
    connect(m_formulaBar, &QLineEdit::returnPressed,
            this, &SpreadsheetWindow::commitFormula);
}

void SpreadsheetWindow::setupRibbon()
{
    // --- تبويب Home ---
    QWidget *homeTab = new QWidget();
    QHBoxLayout *homeLayout = new QHBoxLayout(homeTab);
    homeLayout->setContentsMargins(5, 5, 5, 5);
    homeLayout->setSpacing(10);

    // مجموعة الخطوط (Font Group)
    QFrame *fontFrame = new QFrame();
    fontFrame->setStyleSheet("QFrame { border-right: 1px solid #d2d0ce; }");
    QVBoxLayout *fontLayout = new QVBoxLayout(fontFrame);
    fontLayout->setContentsMargins(0, 0, 10, 0);
    
    QHBoxLayout *fontToolbar = new QHBoxLayout();
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

    // ربط أحداث الخطوط
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
    connect(fontCombo, &QFontComboBox::currentFontChanged, this, [this](const QFont &font) {
        // يمكن إضافة تغيير الخط هنا (يحتاج تعديل النموذج لدعم العائلة)
    });

    // مجموعة المحاذاة (Alignment Group)
    QFrame *alignFrame = new QFrame();
    alignFrame->setStyleSheet("QFrame { border-right: 1px solid #d2d0ce; }");
    QVBoxLayout *alignLayout = new QVBoxLayout(alignFrame);
    alignLayout->setContentsMargins(0, 0, 10, 0);
    QHBoxLayout *alignToolbar = new QHBoxLayout();
    
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
    QHBoxLayout *numToolbar = new QHBoxLayout();
    
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
    statusBar->setStyleSheet("QStatusBar { background: #f3f2f1; border-top: 1px solid #d2d0ce; }");
    
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
