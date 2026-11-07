#ifndef SPREADSHEETWINDOW_H
#define SPREADSHEETWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QModelIndex>

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

private:
    QTableView *m_tableView;
    SpreadsheetModel *m_model;
    QLineEdit *m_formulaBar;
    QLabel *m_cellLabel;
};

#endif // SPREADSHEETWINDOW_H
