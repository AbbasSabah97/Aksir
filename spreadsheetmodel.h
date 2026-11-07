#ifndef SPREADSHEETMODEL_H
#define SPREADSHEETMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QPair>
#include <QString>

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

private:
    QHash<QPair<int, int>, QString> m_cells;
    int m_rows = 100;
    int m_cols = 26;
};

#endif // SPREADSHEETMODEL_H
