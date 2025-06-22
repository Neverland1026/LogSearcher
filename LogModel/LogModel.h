#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class LogModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit LogModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void appendLog(const int lineNumber, const QString& log);
    void appendLog(const QVector<int>& lineNumbers, const QVector<QString>& logs);
    void clearAll();
    bool updateRow(const int row, const QVariant& newValue, int role = Qt::DisplayRole);
private:
    QList<int> m_lineNumber = {};
    QStringList m_logs = {};
};

#endif // LOGMODEL_H
