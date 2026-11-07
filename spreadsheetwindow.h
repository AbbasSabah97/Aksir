#ifndef SPREADSHEETWINDOW_H
#define SPREADSHEETWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QModelIndex>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QSlider>

class QTableView;
class SpreadsheetModel;

class SpreadsheetWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpreadsheetWindow(QWidget *parent = nullptr);

private slots:
    void updateFormulaBar(const QModelIndex &current);
    void commitFormula();
    void applyFormat(int formatType, const QVariant &value);
    void pickColor(int formatType);

private:
    void setupRibbon();
    void setupStatusBar();

    QTableView *m_tableView;
    SpreadsheetModel *m_model;
    QLineEdit *m_formulaBar;
    QLabel *m_cellLabel;
    QTabWidget *m_ribbonTabs;
};

#endif
