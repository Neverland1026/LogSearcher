#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class LogModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum Roles {
        VisibleRole = Qt::UserRole + 1,
    };

    explicit LogModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void appendLog(const int lineNumber, const QString& log);
    void appendLog(const QVector<int>& lineNumbers, const QVector<QString>& logs);
    void clearAll();
    bool updateRow(const int row, const QVariant& newValue, int role = Qt::DisplayRole);
    bool showRow(const int row);
    bool hideRow(const int row);
    bool removeRow(const int row);

private:

    struct LogDataItem {
        int lineIndex = -1;
        QString content = "";
        bool isVisible = true;
        LogDataItem(){}
        LogDataItem(const int index_, const QString& content_, const bool isVisible_)
        {
            this->lineIndex = index_;
            this->content = content_;
            this->isVisible = isVisible_;
        }
    };
    QVector<LogDataItem> m_logDataList;
};

#endif // LOGMODEL_H
