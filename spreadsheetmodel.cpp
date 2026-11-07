#include "spreadsheetmodel.h"
#include <QRegularExpression>

SpreadsheetModel::SpreadsheetModel(QObject *parent) : QAbstractTableModel(parent) {}

int SpreadsheetModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_rows; }
int SpreadsheetModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_cols; }

QVariant SpreadsheetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    
    auto it = m_cells.find({index.row(), index.column()});
    CellData cell = (it != m_cells.end()) ? it.value() : CellData();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (role == Qt::EditRole) return cell.value;
        
        if (cell.value.startsWith('=')) {
            QString res = evaluateFormula(cell.value.mid(1), index.row(), index.column());
            return applyNumberFormat(res, cell.format);
        }
        return applyNumberFormat(cell.value, cell.format);
    }
    
    if (role == Qt::BackgroundRole) return cell.bgColor;
    if (role == Qt::ForegroundRole) return cell.textColor;
    if (role == Qt::TextAlignmentRole) return cell.alignment;
    if (role == Qt::FontRole) {
        QFont font("Segoe UI", 11);
        font.setBold(cell.isBold);
        font.setItalic(cell.isItalic);
        font.setUnderline(cell.isUnderline);
        return font;
    }
    
    return QVariant();
}

bool SpreadsheetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;

    auto key = qMakePair(index.row(), index.column());
    if (!m_cells.contains(key)) m_cells.insert(key, CellData());
    
    m_cells[key].value = value.toString();
    recalculateAll(); 
    return true;
}

void SpreadsheetModel::updateCellFormat(const QModelIndexList &indexes, int formatType, const QVariant &value)
{
    for (const QModelIndex &index : indexes) {
        auto key = qMakePair(index.row(), index.column());
        if (!m_cells.contains(key)) m_cells.insert(key, CellData());
        
        switch (formatType) {
            case 0: m_cells[key].isBold = value.toBool(); break;
            case 1: m_cells[key].isItalic = value.toBool(); break;
            case 2: m_cells[key].isUnderline = value.toBool(); break;
            case 3: m_cells[key].bgColor = value.value<QColor>(); break;
            case 4: m_cells[key].textColor = value.value<QColor>(); break;
            case 5: m_cells[key].alignment = static_cast<Qt::Alignment>(value.toInt()); break;
            case 6: m_cells[key].format = value.toString(); break;
        }
    }
    recalculateAll();
}

void SpreadsheetModel::recalculateAll()
{
    if (m_rows > 0 && m_cols > 0) {
        emit dataChanged(index(0, 0), index(m_rows - 1, m_cols - 1));
    }
}

QString SpreadsheetModel::getRawValue(const QModelIndex &index) const {
    return m_cells.value({index.row(), index.column()}).value;
}

Qt::ItemFlags SpreadsheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant SpreadsheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) return QString(QChar('A' + section));
    else return QString::number(section + 1);
}

QString SpreadsheetModel::applyNumberFormat(const QString &rawResult, const QString &format) const {
    if (rawResult.isEmpty() || rawResult.startsWith('#')) return rawResult;
    bool ok;
    double val = rawResult.toDouble(&ok);
    if (!ok) return rawResult;

    if (format == "Currency") return QString("$ %1").arg(val, 0, 'f', 2);
    if (format == "Percent") return QString("%1%").arg(val * 100, 0, 'f', 1);
    return rawResult;
}

QPair<int, int> SpreadsheetModel::addressToCell(const QString &address) const
{
    QRegularExpression re("([A-Z]+)([0-9]+)");
    auto match = re.match(address);
    if (match.hasMatch()) {
        QString colStr = match.captured(1);
        int col = 0;
        for (QChar c : colStr) col = col * 26 + (c.unicode() - 'A' + 1);
        return {match.captured(2).toInt() - 1, col - 1};
    }
    return {-1, -1};
}

double SpreadsheetModel::getCellValue(int row, int col) const
{
    QString val = m_cells.value({row, col}, CellData()).value;
    if (val.isEmpty()) return 0.0;
    if (val.startsWith('=')) {
        bool ok;
        double res = evaluateFormula(val.mid(1), row, col).toDouble(&ok);
        return ok ? res : 0.0;
    }
    bool ok;
    double res = val.toDouble(&ok);
    return ok ? res : 0.0;
}

QString SpreadsheetModel::evaluateFormula(const QString &formula, int currentRow, int currentCol) const
{
    QString expr = formula.trimmed();
    QRegularExpression funcRegex("(SUM|AVERAGE)\\(([A-Z]+[0-9]+):([A-Z]+[0-9]+)\\)");
    auto match = funcRegex.match(expr);
    if (match.hasMatch()) {
        auto start = addressToCell(match.captured(2));
        auto end = addressToCell(match.captured(3));
        if (start.first != -1 && end.first != -1) {
            double sum = 0; int count = 0;
            for (int r = qMin(start.first, end.first); r <= qMax(start.first, end.first); ++r) {
                for (int c = qMin(start.second, end.second); c <= qMax(start.second, end.second); ++c) {
                    if (r == currentRow && c == currentCol) continue;
                    sum += getCellValue(r, c); count++;
                }
            }
            if (match.captured(1) == "AVERAGE" && count > 0) return QString::number(sum / count);
            return QString::number(sum);
        }
    }

    QRegularExpression refRegex("([A-Z]+[0-9]+)");
    QRegularExpressionMatchIterator i = refRegex.globalMatch(expr);
    QString replacedExpr = expr;
    while (i.hasNext()) {
        auto m = i.next();
        auto cellPos = addressToCell(m.captured(1));
        if (cellPos.first != -1) {
            replacedExpr.replace(m.captured(1), QString::number(getCellValue(cellPos.first, cellPos.second)));
        }
    }

    QRegularExpression numOpRegex("([0-9.]+)\\s*([\\+\\-\\*/])\\s*([0-9.]+)");
    while (replacedExpr.contains(numOpRegex)) {
        auto m = numOpRegex.match(replacedExpr);
        double a = m.captured(1).toDouble(), b = m.captured(3).toDouble(), res = 0;
        QString op = m.captured(2);
        if (op == "+") res = a + b; else if (op == "-") res = a - b;
        else if (op == "*") res = a * b; else if (op == "/") res = (b != 0) ? a / b : 0;
        replacedExpr.replace(m.captured(0), QString::number(res));
    }
    return replacedExpr;
}
