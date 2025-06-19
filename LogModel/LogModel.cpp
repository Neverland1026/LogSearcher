#include "LogModel.h"

LogModel::LogModel(QObject *parent) : QAbstractListModel(parent) {}

int LogModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_logs.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_logs.size())
        return QVariant();
    if (role == Qt::UserRole)
        return (m_lineNumber.size() == m_logs.size()) ? m_lineNumber.at(index.row()) : index.row();
    if (role == Qt::DisplayRole)
        return m_logs.at(index.row());
    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const {
    return { {Qt::UserRole, "lineNumber"}, {Qt::DisplayRole, "lineContent"} };
}

void LogModel::appendLog(const int lineNumber, const QString& log) {
    beginInsertRows(QModelIndex(), m_logs.size(), m_logs.size());
    if(lineNumber >= 0)
    {
        m_lineNumber.append(lineNumber);
    }
    m_logs.append(log);
    endInsertRows();
}

void LogModel::appendLog(const QVector<int>& lineNumbers, const QVector<QString>& logs)
{
    beginInsertRows(QModelIndex(), m_logs.size(), m_logs.size() + qMin(lineNumbers.size(), logs.size()));
    for(int i = 0; i < lineNumbers.size() && i < logs.size(); ++i)
    {
        if(lineNumbers[i] >= 0)
        {
            m_lineNumber.append(lineNumbers[i]);
        }
        m_logs.append(logs[i]);
    }
    endInsertRows();
}

void LogModel::clearAll()
{
    beginResetModel();
    m_lineNumber.clear();
    m_logs.clear();
    endResetModel();
}
