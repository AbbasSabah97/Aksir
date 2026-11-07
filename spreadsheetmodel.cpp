#include "spreadsheetmodel.h"

SpreadsheetModel::SpreadsheetModel(QObject *parent) : QAbstractTableModel(parent) {}

int SpreadsheetModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_rows;
}

int SpreadsheetModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_cols;
}

QVariant SpreadsheetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return m_cells.value({index.row(), index.column()}, QString());
    }
    return QVariant();
}

bool SpreadsheetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (value.toString().isEmpty())
        m_cells.remove({index.row(), index.column()});
    else
        m_cells.insert({index.row(), index.column()}, value.toString());

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags SpreadsheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant SpreadsheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return QString(QChar('A' + section));
    } else {
        return QString::number(section + 1);
    }
}
