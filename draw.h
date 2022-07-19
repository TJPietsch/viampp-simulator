#ifndef DRAW_H
#define DRAW_H

#include <QGraphicsItem>
#include <QColor>
#include <QDebug>

class draw : public QGraphicsItem
{
public:
    draw(const QColor &color, QString description, int type, QWidget *mppObject);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString description;
    int type;
    QWidget *mppObject;
    QColor color;
    QColor oldColor;

};

class connectElements : public QGraphicsLineItem
{
public:
   connectElements(const QColor &color, QPointF from, QPointF to);

   QRectF boundingRect() const override;
   QPainterPath shape() const override;
   void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:

private:
   QPointF from, to;
   QColor color;
};


#endif // DRAW_H
