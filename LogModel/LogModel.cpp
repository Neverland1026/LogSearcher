#include "LogModel.h"

LogModel::LogModel(QObject *parent) : QAbstractListModel(parent) {}

int LogModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_logDataList.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_logDataList.size())
        return QVariant();
    if (role == Qt::UserRole)
        return m_logDataList.at(index.row()).lineIndex;
    if (role == Qt::DisplayRole)
        return m_logDataList.at(index.row()).content;
    if (role == VisibleRole) {
        return m_logDataList[index.row()].isVisible;
    }
    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const
{
    return { { Qt::UserRole, "lineNumber" }, { Qt::DisplayRole, "lineContent" }, { VisibleRole, "isVisible" } };
}

void LogModel::appendLog(const int lineNumber, const QString& log)
{
    beginInsertRows(QModelIndex(), m_logDataList.size(), m_logDataList.size());
    if(lineNumber >= 0)
    {
        m_logDataList.emplace_back(lineNumber, log, true);
    }
    endInsertRows();
}

void LogModel::appendLog(const QVector<int>& lineNumbers, const QVector<QString>& logs)
{
    beginInsertRows(QModelIndex(), m_logDataList.size(), m_logDataList.size() + qMin(lineNumbers.size(), logs.size()));
    for(int i = 0; i < lineNumbers.size() && i < logs.size(); ++i)
    {
        if(lineNumbers[i] >= 0)
        {
            m_logDataList.emplace_back(lineNumbers[i], logs[i], true);
        }
    }
    endInsertRows();
}

void LogModel::clearAll()
{
    beginResetModel();
    m_logDataList.resize(0);
    endResetModel();
}

bool LogModel::updateRow(const int row, const QVariant& newValue, int role /*= Qt::DisplayRole*/)
{
    if (row < 0 || row >= m_logDataList.size())
        return false;

    m_logDataList[row].content = newValue.toString();

    QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, { role });

    return true;
}

bool LogModel::showRow(int row)
{
    if (row < 0 || row >= m_logDataList.size())
        return false;

    m_logDataList[row].isVisible = true;
    emit dataChanged(index(row), index(row), { VisibleRole });

    return true;
}

bool LogModel::hideRow(int row)
{
    if (row < 0 || row >= m_logDataList.size())
        return false;

    m_logDataList[row].isVisible = false;
    emit dataChanged(index(row), index(row), { VisibleRole });

    return true;
}

bool LogModel::removeRow(int row)
{
    if (row < 0 || row >= m_logDataList.size())
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    m_logDataList.removeAt(row);
    endRemoveRows();

    return true;
}
