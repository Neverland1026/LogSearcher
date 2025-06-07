#include "LogModel.h"

LogModel::LogModel(QObject *parent) : QAbstractListModel(parent) {}

int LogModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_logs.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_logs.size())
        return QVariant();
    if (role == Qt::DisplayRole || role == Qt::UserRole)
        return m_logs.at(index.row());
    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const {
    return { {Qt::DisplayRole, "display"} };
}

void LogModel::appendLog(const QString &log) {
    beginInsertRows(QModelIndex(), m_logs.size(), m_logs.size());
    m_logs.append(log);
    endInsertRows();
}
