#include "spreadsheetwindow.h"
#include "spreadsheetmodel.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QToolBar>
#include <QAction>
#include <QFont>

SpreadsheetWindow::SpreadsheetWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("أكسير - محرر الجداول");
    resize(1000, 700);

    // 1. إعداد شريط الأدوات (Toolbar)
    QToolBar *toolbar = addToolBar("أدوات");
    toolbar->setMovable(false);
    
    QAction *boldAction = toolbar->addAction("B");
    QFont boldFont = boldAction->font(); boldFont.setBold(true); boldAction->setFont(boldFont);
    
    QAction *italicAction = toolbar->addAction("I");
    QFont italicFont = italicAction->font(); italicFont.setItalic(true); italicAction->setFont(italicFont);
    
    toolbar->addSeparator();
    QAction *alignLeft = toolbar->addAction("محاذاة لليسار");
    QAction *alignCenter = toolbar->addAction("توسيط");

    // ربط الأزرار (مثال: تفعيل الخط العريض)
    connect(boldAction, &QAction::triggered, this, [this]() {
        QModelIndexList selected = m_tableView->selectionModel()->selectedIndexes();
        for (const QModelIndex &index : selected) {
            m_model->setData(index, m_model->data(index, Qt::EditRole), Qt::EditRole); // إعادة حفظ
            // في نسخة كاملة، هنا يتم حفظ التنسيق في نموذج البيانات
        }
    });

    // 2. إعداد الجدول
    m_model = new SpreadsheetModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setDefaultSectionSize(100);
    m_tableView->verticalHeader()->setDefaultSectionSize(25);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setCurrentIndex(m_model->index(0, 0));

    // 3. شريط الصيغة
    m_cellLabel = new QLabel("A1", this);
    m_cellLabel->setMinimumWidth(50);
    m_cellLabel->setAlignment(Qt::AlignCenter);
    
    m_formulaBar = new QLineEdit(this);
    
    QHBoxLayout *formulaLayout = new QHBoxLayout;
    formulaLayout->addWidget(m_cellLabel);
    formulaLayout->addWidget(m_formulaBar);

    // 4. تجميع الواجهة
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(formulaLayout);
    mainLayout->addWidget(m_tableView);
    setCentralWidget(centralWidget);

    // 5. ربط الأحداث
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &SpreadsheetWindow::updateFormulaBar);
    connect(m_formulaBar, &QLineEdit::returnPressed,
            this, &SpreadsheetWindow::commitFormula);
}

void SpreadsheetWindow::updateFormulaBar(const QModelIndex &current)
{
    if (!current.isValid()) return;
    QString col = QString(QChar('A' + current.column()));
    m_cellLabel->setText(col + QString::number(current.row() + 1));
    
    // عرض المعادلة الأصلية أو القيمة
    QString rawValue = m_model->data(current, Qt::EditRole).toString();
    m_formulaBar->setText(rawValue);
}

void SpreadsheetWindow::commitFormula()
{
    QModelIndex current = m_tableView->currentIndex();
    if (current.isValid()) {
        m_model->setData(current, m_formulaBar->text(), Qt::EditRole);
    }
}
