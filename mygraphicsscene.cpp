#include "mygraphicsscene.h"

myGraphicsScene::myGraphicsScene(QObject *parent) : QGraphicsScene (parent)
{

}

void myGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() <<"myGraphicsScene::mousePressEvent";
    QGraphicsScene::mousePressEvent(event);
}

void myGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() <<"myGraphicsScene::mouseMoveEvent";
      QGraphicsScene::mouseMoveEvent(event);
}


void myGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "myGraphicsScene::mouseReleaseEvent";
    QGraphicsScene::mouseReleaseEvent(event);
}


void myGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() <<"myGraphicsScene::mouseDoubleClickEvent";
    QGraphicsScene::mouseDoubleClickEvent(event);
}

QGraphicsItem *myGraphicsScene::itemUnderCursor(QPointF cursorPoint) //, myGraphicsScene &scene)
{
//    qDebug() << "myGraphicsScene::itemUnderCursor";

    return this->itemAt(cursorPoint, this->views()[0]->transform()); //viewTransform);

}
