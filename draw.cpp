#include "draw.h"

#include <QtWidgets>

draw::draw(const QColor &color, QString description, int type, QWidget *mppObject)
{
    this->description = description;
    this->color = color;
    this->oldColor = color;
    this->type = type;
    this->mppObject = mppObject;
    setZValue(1);
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

QRectF draw::boundingRect() const
{
    switch (type) {
        case 1: return QRectF(0, 0, 40, 40);    // Router
        case 2: return QRectF(0, 0, 40, 50);    // Memory
        case 3: return QRectF(0, 0, 30, 40);    // CPU
        case 4: return QRectF(0, 0, 40, 40);    // Injector
    }
}

QPainterPath draw::shape() const
{
    QPainterPath path;
    switch (type) {
        case 1: path.addRect(0, 0, 40, 40);   // Router
                return path;
        case 2: path.addRect(0, 0, 40, 50);   // Memory
                return path;
        case 3: path.addRect(0, 0, 30, 40);   // CPU
                return path;
        case 4: path.addRect(0, 0, 40, 40);   // Injector
                return path;
    }
}

void draw::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? color.dark(150) : color;
    if (option->state & QStyle::State_MouseOver)
        fillColor = fillColor.light(125);
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if (lod < 0.2) {
        if (lod < 0.125) {
            switch (type) {
                case 1: painter->fillRect(QRectF(0, 0, 40, 40), fillColor);   // Router
                        return;
                case 2: painter->fillRect(QRectF(0, 0, 40, 50), fillColor);   // Memory
                        return;
                case 3: painter->fillRect(QRectF(0, 0, 30, 40), fillColor);   // CPU
                        return;
                case 4: painter->fillRect(QRectF(0, 0, 40, 40), fillColor);   // Injector
            }
        }

        QBrush b = painter->brush();
        painter->setBrush(fillColor);
        switch (type) {
            case 1: painter->drawRect(0, 0, 40, 40);   // Router
                    break;
            case 2: painter->drawRect(0, 0, 40, 50);   // Memory
                    break;
            case 3: painter->drawRect(0, 0, 30, 40);   // CPU
                    break;
            case 4: painter->drawRect(0, 0, 40, 40);   // Injector
        }
        painter->setBrush(b);
        return;
    }
    QPen oldPen = painter->pen();
    QPen pen = oldPen;
    int width = 0;
    if (option->state & QStyle::State_Selected)
        width += 2;

    pen.setWidth(width);
    QBrush b = painter->brush();
    painter->setBrush(QBrush(fillColor.dark(option->state & QStyle::State_Sunken ? 120 : 100)));
    switch (type) {
        case 1: painter->drawRect(QRect(0, 0, 40, 40));    // Router
                break;
        case 2: painter->drawRect(QRect(0, 0, 40, 50));    // Memory
                break;
        case 3: painter->drawRect(QRect(0, 0, 30, 40));    // CPU
                break;
        case 4: painter->drawRect(QRect(0, 0, 40, 40));    // Injector
    }
    painter->setBrush(b);

    // Draw text
    if (lod >= 2) {
        QFont font("Times", 12, 63);
        font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);
        painter->save();
        painter->scale(0.1, 0.1);
        painter->drawText(150, 180, description);
        painter->restore();
    }

    // Draw Shape
    QVarLengthArray<QLineF, 30> lines;
    if (lod >= 0.4) {
        switch (type) {
            case 1: // Router
                    painter->drawEllipse(15, 15, 10,10);
                    lines.append(QLineF(20, 15, 20, 5));
                    lines.append(QLineF(15, 10, 20, 5));
                    lines.append(QLineF(20, 5, 25, 10));
                    lines.append(QLineF(25, 20, 35, 20));
                    lines.append(QLineF(30, 15, 35, 20));
                    lines.append(QLineF(35, 20, 30, 25));
                    lines.append(QLineF(20, 25, 20, 35));
                    lines.append(QLineF(25, 30, 20, 35));
                    lines.append(QLineF(20, 35, 15, 30));
                    lines.append(QLineF(15, 20, 5, 20));
                    lines.append(QLineF(5, 20, 10, 25));
                    lines.append(QLineF(5, 20, 10, 15));
                    lines.append(QLineF(15, 15, 10, 10));
                    lines.append(QLineF(10, 10, 5, 10));
                    lines.append(QLineF(10, 10, 10, 5));
                    lines.append(QLineF(25, 15, 30, 10));
                    lines.append(QLineF(30, 10, 30, 5));
                    lines.append(QLineF(30, 10, 35, 10));
                    lines.append(QLineF(25, 25, 30, 30));
                    lines.append(QLineF(30, 30, 35, 30));
                    lines.append(QLineF(30, 30, 30, 35));
                    lines.append(QLineF(15, 25, 10, 30));
                    lines.append(QLineF(10, 30, 10, 35));
                    lines.append(QLineF(10, 30, 5, 30));
                    break;
            case 2: // Memory
                    lines.append(QLineF(5, 15, 15, 5));
                    lines.append(QLineF(15, 5, 35, 5));
                    lines.append(QLineF(35, 5, 35, 45));
                    lines.append(QLineF(35, 45, 5, 45));
                    lines.append(QLineF(5, 45, 5, 15));
                    lines.append(QLineF(15, 8, 15, 15));
                    lines.append(QLineF(20, 8, 20, 15));
                    lines.append(QLineF(25, 8, 25, 15));
                    lines.append(QLineF(30, 8, 30, 15));
                    break;
            case 3: // CPU
                    lines.append(QLineF(5, 15, 5, 5));
                    lines.append(QLineF(5, 5, 25, 15));
                    lines.append(QLineF(25, 15, 25, 25));
                    lines.append(QLineF(25, 25, 5, 35));
                    lines.append(QLineF(5, 35, 5, 25));
                    lines.append(QLineF(5, 25, 15, 20));
                    lines.append(QLineF(15, 20, 5, 15));
                    break;
            case 4: // Injector
                    painter->drawEllipse(0, 0, 40, 40);
                    painter->drawEllipse(15, 10, 10, 10);
                    lines.append(QLineF(22, 20, 20, 40));
                    lines.append(QLineF(15, 35, 20, 40));
                    lines.append(QLineF(25, 35, 20, 40));
                    lines.append(QLineF(15, 20, 5, 30));
                    lines.append(QLineF(5, 30, 5, 25));
                    lines.append(QLineF(5, 30, 10, 30));
                    lines.append(QLineF(25, 20, 35, 30));
                    lines.append(QLineF(35, 30, 30, 30));
                    lines.append(QLineF(35, 30, 35, 25));
                    lines.append(QLineF(15, 5, 20,10));
                    lines.append(QLineF(20, 10, 25,5));
                    lines.append(QLineF(17, 2, 17, 7));
                    lines.append(QLineF(23, 2, 23, 7));
         }
    }
    painter->drawLines(lines.data(), lines.size());
}

void draw::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    color = oldColor;
    update();
}

void draw::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
//        stuff << event->pos();
        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void draw::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

void draw::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    color = Qt::red;

    mppObject->isVisible() ? mppObject->hide() : mppObject->show();
    update();
    qDebug() <<"mouse double click";
}


connectElements::connectElements(const QColor &color, QPointF from, QPointF to)
{
    this->color = color;
    this->from = from;
    this->to = to;
    setZValue(0);
    setFlags(0);
    setAcceptHoverEvents(false);
}

QRectF connectElements::boundingRect() const
{
    qreal dx = to.x() - from.x();
    qreal dy = to.y() - from.y();
    return QRectF(from.x(), from.y() + dy, dx, dy);
}

QPainterPath connectElements::shape() const
{
    QPainterPath path;

    qreal dx = to.x() - from.x();
    qreal dy = to.y() - from.y();
    path.addRect(from.x(), from.y() + dy, dx, dy);
    return path;
}

void connectElements::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

//    painter->drawLine(from, to);

    QPainterPath path;
    path.moveTo(from);
    qreal dx = from.x() - to.x();
    qreal dy = from.y() - to.y();
//    QPointF ctr1(to.x() + dx * 0.25, to.y() + dy * 0.1);
//    QPointF ctr2(to.x() + dx * 0.75, to.y() + dy * 0.9);
    QPointF ctr1(to.x() + dx * 0.5, to.y() + dy * 0.1);
    QPointF ctr2(to.x() + dx * 0.5, to.y() + dy * 0.9);
    path.cubicTo(ctr1, ctr2, to);
    painter->drawPath(path);

    painter->setPen(QPen());

}
