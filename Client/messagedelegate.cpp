#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QStaticText>
#include <QTextOption>
#include <QStandardItem>
#include <QTransform>

#include "messagedelegate.h"

MessageDelegate::MessageDelegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

void MessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    QRect &rect(opt.rect);

    const QPalette &palette(opt.palette);

    if (index.model()->data(index.model()->index(index.row(), 1)).toString().isEmpty()) {

        QString user_info = index.model()->data(index.model()->index(index.row(), 0)).toString();

        const QRect &contentRect(rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom()));

        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(rect, 10, 10);
        QPen pen(QColor("#6fecfb"), 1);
        painter->setPen(pen);
        painter->fillPath(path, QColor("#1779c9"));
        painter->drawPath(path);

        opt.font.setBold(true);
        painter->setFont(opt.font);
        painter->setPen(QColor(255, 255, 255));
        painter->drawText(contentRect, Qt::AlignCenter, user_info);

    } else {

        // Get text from model

        QString nickname = index.model()->data(index.model()->index(index.row(), 0)).toString() + ":";
        QString message = index.model()->data(index.model()->index(index.row(), 1)).toString();
        QString time = index.model()->data(index.model()->index(index.row(), 2)).toString();

        // Get bounding rectangles

        QRect nickname_rect = opt.fontMetrics.boundingRect(nickname).adjusted(0, 0, 2, 2);

        QStaticText message_text(message);
        QTextOption message_option;
        message_option.setWrapMode(QTextOption::WordWrap);
        message_text.prepare(QTransform(), opt.font);
        message_text.setTextOption(message_option);
        message_text.setTextWidth(rect.width() - nickname_rect.width() - horizontal_spacing - margins.left() - margins.right());
        QSizeF message_size = message_text.size();
        QRect message_rect(QPoint(0, 0), message_size.toSize());

        QRect time_rect = opt.fontMetrics.boundingRect(time);

        // Create main rectangles
        rect.adjust(0, 0, 0, margins.top() + message_rect.height() + vertical_spacing + time_rect.height() - rect.height());

        const QRect &contentRect(rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom()));

        // Set positions for rectangles
        nickname_rect.moveTo(margins.left(), contentRect.top());

        message_rect.moveTo(nickname_rect.right() + horizontal_spacing + margins.left(), contentRect.top());

        time_rect.moveTo(nickname_rect.right() + message_text.size().width() +
                             vertical_spacing - time_rect.width(), contentRect.bottom() - time_rect.height());


        //Draw main rect
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(rect, 10, 10);
        QPen pen(QColor("#6fecfb"), 1);
        painter->setPen(pen);
        painter->fillPath(path, QColor("#1779c9"));
        painter->drawPath(path);


        opt.font.setBold(true);
        painter->setFont(opt.font);
        painter->setPen(QColor(255, 255, 255));
        painter->drawText(nickname_rect, Qt::TextSingleLine, nickname);


        // width of message_rect contentRect.width() - nickname_rect.width()



        opt.font.setBold(false);
        painter->setFont(opt.font);
        painter->drawText(message_rect, Qt::TextWordWrap, message);


        painter->drawText(time_rect, Qt::TextSingleLine, time);

    }

}

QSize MessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    if (index.model()->data(index.model()->index(index.row(), 1)).toString().isEmpty()) {

        return QSize(opt.rect.width(), opt.fontMetrics.height() + margins.top() + margins.bottom());

    } else {

        // Get text from model
        QString nickname = index.model()->data(index.model()->index(index.row(), 0)).toString() + ":";
        QString message = index.model()->data(index.model()->index(index.row(), 1)).toString();
        QString time = index.model()->data(index.model()->index(index.row(), 2)).toString();

        // Create bounding rectangles to get sizes
        QRect &rect(opt.rect);
        QRect nickname_rect = opt.fontMetrics.boundingRect(nickname).adjusted(0, 0, 1, 1);

        QStaticText message_text(message);
        QTextOption message_option;
        message_option.setWrapMode(QTextOption::WordWrap);
        message_text.prepare(QTransform(), opt.font);
        message_text.setTextOption(message_option);
        message_text.setTextWidth(rect.width() - nickname_rect.width() - horizontal_spacing - margins.left() - margins.right());
        QSizeF message_size = message_text.size();
        QRect message_rect(QPoint(0, 0), message_size.toSize());

        QRect time_rect = opt.fontMetrics.boundingRect(time);


        return QSize(opt.rect.width(), margins.top() + message_rect.height() +
                                       time_rect.height() + margins.bottom());

    }

}

void MessageDelegate::setContentsMargins(const int &left, const int &top, const int &right, const int &bottom)
{
    margins = QMargins(left, top, right, bottom);
}

int MessageDelegate::horizontalSpacing() const
{
    return horizontal_spacing;
}

void MessageDelegate::setHorizontalSpacing(const int &spacing)
{
    horizontal_spacing = spacing;
}

int MessageDelegate::verticalSpacing() const
{
    return vertical_spacing;
}

void MessageDelegate::setVerticalSpacing(const int &spacing)
{
    vertical_spacing = spacing;
}
