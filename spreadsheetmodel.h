#ifndef SPREADSHEETMODEL_H
#define SPREADSHEETMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QPair>
#include <QString>
#include <QColor>
#include <QFont>

struct CellData {
    QString value;
    bool isBold = false;
    bool isItalic = false;
    bool isUnderline = false;
    QColor bgColor;   // لون غير محدد افتراضياً (شفاف)
    QColor textColor; // لون غير محدد افتراضياً (يورث من الوضع الحالي)
    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;
    QString format = "General"; 
};

class SpreadsheetModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SpreadsheetModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void recalculateAll(); 
    void updateCellFormat(const QModelIndexList &indexes, int formatType, const QVariant &value);
    QString getRawValue(const QModelIndex &index) const;

private:
    QHash<QPair<int, int>, CellData> m_cells;
    int m_rows = 1000;
    int m_cols = 26;

    QString evaluateFormula(const QString &formula, int currentRow, int currentCol) const;
    double getCellValue(int row, int col) const;
    QPair<int, int> addressToCell(const QString &address) const;
    QString applyNumberFormat(const QString &rawResult, const QString &format) const;
};

#endif
