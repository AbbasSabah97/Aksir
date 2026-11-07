#include "spreadsheetmodel.h"
#include <QRegularExpression>
#include <cmath>

SpreadsheetModel::SpreadsheetModel(QObject *parent) : QAbstractTableModel(parent) {}

int SpreadsheetModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_rows; }
int SpreadsheetModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_cols; }

QVariant SpreadsheetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString rawValue = m_cells.value({index.row(), index.column()}, "");
        if (rawValue.startsWith('=')) {
            return evaluateFormula(rawValue.mid(1), index.row(), index.column());
        }
        return rawValue;
    }
    return QVariant();
}

bool SpreadsheetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) return false;

    if (value.toString().isEmpty())
        m_cells.remove({index.row(), index.column()});
    else
        m_cells.insert({index.row(), index.column()}, value.toString());

    recalculateAll(); // تحديث الجدول بالكامل عند تغيير أي خلية
    return true;
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

void SpreadsheetModel::recalculateAll()
{
    if (m_rows > 0 && m_cols > 0) {
        emit dataChanged(index(0, 0), index(m_rows - 1, m_cols - 1), {Qt::DisplayRole});
    }
}

QString SpreadsheetModel::cellToAddress(int row, int col) const
{
    return QString(QChar('A' + col)) + QString::number(row + 1);
}

QPair<int, int> SpreadsheetModel::addressToCell(const QString &address) const
{
    QRegularExpression re("([A-Z]+)([0-9]+)");
    auto match = re.match(address);
    if (match.hasMatch()) {
        QString colStr = match.captured(1);
        QString rowStr = match.captured(2);
        int col = 0;
        for (QChar c : colStr) col = col * 26 + (c.unicode() - 'A' + 1);
        return {rowStr.toInt() - 1, col - 1};
    }
    return {-1, -1};
}

double SpreadsheetModel::getCellValue(int row, int col) const
{
    QString val = m_cells.value({row, col}, "");
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

    // 1. معالجة الدوال (SUM و AVERAGE)
    QRegularExpression funcRegex("(SUM|AVERAGE)\\(([A-Z]+[0-9]+):([A-Z]+[0-9]+)\\)");
    auto match = funcRegex.match(expr);
    if (match.hasMatch()) {
        QString funcName = match.captured(1);
        auto start = addressToCell(match.captured(2));
        auto end = addressToCell(match.captured(3));
        if (start.first != -1 && end.first != -1) {
            double sum = 0; int count = 0;
            for (int r = qMin(start.first, end.first); r <= qMax(start.first, end.first); ++r) {
                for (int c = qMin(start.second, end.second); c <= qMax(start.second, end.second); ++c) {
                    if (r == currentRow && c == currentCol) continue; // منع التكرار اللانهائي
                    sum += getCellValue(r, c);
                    count++;
                }
            }
            if (funcName == "AVERAGE" && count > 0) return QString::number(sum / count);
            return QString::number(sum);
        }
    }

    // 2. معالجة المراجع العادية (مثل A1 + B2)
    QRegularExpression refRegex("([A-Z]+[0-9]+)");
    QRegularExpressionMatchIterator i = refRegex.globalMatch(expr);
    QString replacedExpr = expr;
    while (i.hasNext()) {
        auto m = i.next();
        QString cellAddr = m.captured(1);
        auto cellPos = addressToCell(cellAddr);
        if (cellPos.first != -1) {
            if (cellPos.first == currentRow && cellPos.second == currentCol) return "#REF!"; // دائرة مغلقة
            double val = getCellValue(cellPos.first, cellPos.second);
            replacedExpr.replace(cellAddr, QString::number(val));
        }
    }

    // 3. حساب المعادلة الرياضية البسيطة (بدون مكتبات خارجية)
    // ملاحظة: هذا محلل بسيط جدًا يدعم + - * / فقط
    try {
        std::string s = replacedExpr.toStdString();
        double result = 0;
        // هنا نستخدم دالة بسيطة للتقييم (لأغراض العرض)
        // في التطبيقات الحقيقية يستخدم Shunting-yard algorithm
        QRegularExpression numOpRegex("([0-9.]+)\\s*([\\+\\-\\*/])\\s*([0-9.]+)");
        while (replacedExpr.contains(numOpRegex)) {
            auto m = numOpRegex.match(replacedExpr);
            double a = m.captured(1).toDouble();
            QString op = m.captured(2);
            double b = m.captured(3).toDouble();
            double res = 0;
            if (op == "+") res = a + b;
            else if (op == "-") res = a - b;
            else if (op == "*") res = a * b;
            else if (op == "/") res = (b != 0) ? a / b : 0;
            replacedExpr.replace(m.captured(0), QString::number(res));
        }
        return replacedExpr;
    } catch (...) {
        return "#ERROR!";
    }
}
