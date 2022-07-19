#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPointF>

#include <QDebug>

class myGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    myGraphicsScene(QObject *parent = Q_NULLPTR);

    QGraphicsItem *itemUnderCursor(QPointF cursorPoint);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int currentLineType = 1;
};

#endif // MYGRAPHICSSCENE_H
