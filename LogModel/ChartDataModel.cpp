#include "ChartDataModel.h"

ChartDataModel::ChartDataModel(QObject *parent) : QAbstractListModel(parent) {}

int ChartDataModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.size();
}

QVariant ChartDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_data.size())
        return QVariant();

    const auto &item = m_data[index.row()];
    switch (role) {
    case LabelRole: return item.first;
    case ValueRole: return item.second;
    default: return QVariant();
    }
}

QVariantMap ChartDataModel::get(int index) const
{
    QVariantMap map;
    if (index >= 0 && index < m_data.size()) {
        map["label"] = m_data[index].first;
        map["value"] = m_data[index].second;
    }
    return map;
}

QHash<int, QByteArray> ChartDataModel::roleNames() const
{
    return {
        {LabelRole, "label"},
        {ValueRole, "value"}
    };
}

void ChartDataModel::setData(const QVector<QPair<QString, int>>& data)
{
    beginResetModel();
    m_data = data;
    endResetModel();
}
