#ifndef NICKNAMEDELEGATE_H
#define NICKNAMEDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>
#include <QPainter>
#include <QPalette>
#include <QLinearGradient>
#include <QPainterPath>

class NicknameDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NicknameDelegate(QObject *parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const;

};

#endif // NICKNAMEDELEGATE_H
