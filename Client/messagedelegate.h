#ifndef MESSAGEDELEGATE_H
#define MESSAGEDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class MessageDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MessageDelegate(QObject* parent);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    void setContentsMargins(const int& left, const int& top, const int& right, const int& bottom);

    int horizontalSpacing() const;
    void setHorizontalSpacing(const int& spacing);

    int verticalSpacing() const;
    void setVerticalSpacing(const int& spacing);

private:

    QMargins margins;
    int horizontal_spacing;
    int vertical_spacing;

};

#endif // MESSAGEDELEGATE_H
