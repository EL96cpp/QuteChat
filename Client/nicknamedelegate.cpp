#include "nicknamedelegate.h"

NicknameDelegate::NicknameDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

void NicknameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    QRect &rect(opt.rect);
    QRect contentRect(rect);
    contentRect.moveTo(rect.x(), rect.y());

    const QPalette &palette(opt.palette);

    QString nickname = index.model()->data(index.model()->index(index.row(), 0)).toString();

    QRadialGradient radialGrad(rect.center(), 200);

    radialGrad.setColorAt(0, QColor("#C0003a6b"));
    radialGrad.setColorAt(0.7, QColor("#901a82d9"));
    radialGrad.setColorAt(1, QColor("#40cce3f7"));

    QPainterPath rounded_rect_path;
    rounded_rect_path.addRoundedRect(rect, 8, 8);
    QPen pen(QColor("#6fecfb"), 1);
    painter->setPen(pen);
    painter->fillPath(rounded_rect_path, radialGrad);

    opt.font.setBold(true);
    painter->setFont(opt.font);
    painter->setPen(QColor(255, 255, 255));
    painter->drawText(rect, Qt::AlignCenter, nickname);


}

QSize NicknameDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    return QSize(opt.rect.width(), opt.fontMetrics.height());
}
