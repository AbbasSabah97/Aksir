#include "spreadsheetwindow.h"
#include "spreadsheetmodel.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>

SpreadsheetWindow::SpreadsheetWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("أكسير - محرر الجداول");
    resize(1000, 700);

    m_model = new SpreadsheetModel(this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setDefaultSectionSize(100);
    m_tableView->verticalHeader()->setDefaultSectionSize(25);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setCurrentIndex(m_model->index(0, 0));

    m_cellLabel = new QLabel("A1", this);
    m_formulaBar = new QLineEdit(this);
    QHBoxLayout *formulaLayout = new QHBoxLayout;
    formulaLayout->addWidget(m_cellLabel);
    formulaLayout->addWidget(m_formulaBar);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(formulaLayout);
    mainLayout->addWidget(m_tableView);
    setCentralWidget(centralWidget);

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
    m_formulaBar->setText(m_model->data(current, Qt::EditRole).toString());
}

void SpreadsheetWindow::commitFormula()
{
    QModelIndex current = m_tableView->currentIndex();
    if (current.isValid()) {
        m_model->setData(current, m_formulaBar->text(), Qt::EditRole);
    }
}
