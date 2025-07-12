#ifndef CHARTDATAMODEL_H
#define CHARTDATAMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QPair>
#include <QString>

class ChartDataModel : public QAbstractListModel
{
    Q_OBJECT

    // 添加count属性声明
    Q_PROPERTY(int count READ rowCount NOTIFY dataUpdated)

public:

    enum Roles { LabelRole = Qt::UserRole + 1, ValueRole };

    explicit ChartDataModel(QObject *parent = nullptr);

    // 必须声明为Q_INVOKABLE才能在QML调用
    Q_INVOKABLE QVariantMap get(int index) const;

    // QAbstractListModel 接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // 数据操作
    void setData(const QVector<QPair<QString, int>>& data);

signals:
    void dataUpdated();

private:
    QVector<QPair<QString, int>> m_data;
};

#endif // CHARTDATAMODEL_H
