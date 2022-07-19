#include "mygraphicsview.h"

//#include "drawline.h"

myGraphicsView::myGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);

    setStyleSheet(QString("background-color: rgb(53, 53, 53)"));    // change background color in style sheet

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setCacheMode(QGraphicsView::CacheBackground);
}


void myGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
//    qDebug() << "myGraphicsView::contextMenuEvent";
    QPoint eventPos = event->pos();

}

void myGraphicsView::wheelEvent(QWheelEvent *event)
{
//    qDebug() << "myGraphicsView::wheelEvent";

    QPoint delta = event->angleDelta();
    if (delta.y() == 0){
        event->ignore();
        return;
    }

    double const d = delta.y() / std::abs(delta.y());
//    qDebug() << "d: " << d << "modifier: " << event->modifiers();
    if(event->modifiers() & Qt::ShiftModifier){
        if (d > 0.0)
            zoomIn();
        else
            zoomOut();
    } else
        if(event->modifiers() & Qt::ControlModifier){
            if (d > 0.0)
                rotateZCW();
            else
                rotateZCCW();
        }
}

void myGraphicsView::zoomIn()
{
//    qDebug() << "myGraphicsView::zoomIn";
    double const step = 1.2;
    double const factor = std::pow(step,1);

    if (trans.m11() > 5.0)   // max zoom in is set to 5 (2..6 makes sence)
        return;
    scale(factor, factor);
}

void myGraphicsView::zoomOut()
{
//    qDebug() << "myGraphicsView::zoomOut";
    double const step = 1.2;
    double const factor = std::pow(step,-1);

    if (trans.m11() < 0.05)   // max zoom in is set to 5 (2..6 makes sence)
        return;
    scale(factor, factor);
}

void myGraphicsView::rotateZCW()
{
//    qDebug() << "myGraphicsView:rotateZCW";
    double const factor = 1.0;

    rotate(factor);
}

void myGraphicsView::rotateZCCW()
{
//    qDebug() << "myGraphicsView::rotateZCCW";
    double const factor = -1.0;

    rotate(factor);
}

void myGraphicsView::keyPressEvent(QKeyEvent *event)
{
//    qDebug() << "myGraphicsView::keyPressEvent";
    switch(event->key()){
        case Qt::Key_Shift:
        case Qt::Key_Control:
            setDragMode(QGraphicsView::RubberBandDrag);
            break;

        default:
            break;
    }
    QGraphicsView::keyPressEvent(event);
}


void myGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
//    qDebug() << "myGraphicsView::keyReleaseEvent";
    switch(event->key()){
        case Qt::Key_Shift:
        case Qt::Key_Control:
            setDragMode(QGraphicsView::ScrollHandDrag);
            break;

        default:
            break;
    }
    QGraphicsView::keyPressEvent(event);
    setCursor(Qt::ArrowCursor);
}

void myGraphicsView::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "myGraphicsView::mousePressEvent";
    QGraphicsView::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
        _clickPos = mapToScene(event->pos());
}

/*
void myGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << "myGraphicsView::mouseReleaseEvent";

    QGraphicsItem *item = itemAt(event->pos());

    if ((event->button() == Qt::LeftButton) && item) {
        // only draw connection if part or bus
        if (item->type() == QGraphicsItem::UserType + 10) {
            qDebug() << "myGraphicsView::mouseReleaseEvent: line-item";
            ShowPart *part = qgraphicsitem_cast<ShowPart *>(item);
            if (qgraphicsitem_cast<ShowPart *>(item)->getPossibleConnection().active)
            switch (myScene->getLineType()) {       // change later: lineType will be part of
            case 1: new DrawLine(part->getPossibleConnection(), part, myScene);
                break;
            }
        }

        if (item->type() == QGraphicsItem::UserType + 12) {
            qDebug() << "myGraphicsView::mouseReleaseEvent: bus-item";
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}
*/

void myGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "myGraphicsView::mouseMoveEvent";
    QGraphicsView::mouseMoveEvent(event);
    if(scene()->mouseGrabberItem() == nullptr && event->buttons() == Qt::LeftButton)
        // Make sure shift is not being pressed
        if ((event->modifiers() & Qt::ShiftModifier) == 0){
            QPointF difference = _clickPos - mapToScene(event->pos());
            setSceneRect(sceneRect().translated(difference.x(), difference.y()));
        }

/*    QString text;
    QPoint pos = event->pos();
    QPointF posView = this->mapToScene(pos);

    text = QString("%1 X %2").arg(posView.x()).arg(posView.y());
    qDebug() << text;
*/
}

void myGraphicsView::drawBackground(QPainter *painter, const QRectF &rect) {
    if (mGridVisible) {
        QRect wRect = rect.toRect();
        QPointF tl = wRect.topLeft();
        QPointF br = wRect.bottomRight();
        int gridStep;
        double left, right, bottom, top;

        const qreal detail = QStyleOptionGraphicsItem::levelOfDetailFromTransform(transform());
        if (detail > 0.2){      // if to small -> down't draw fine grid
            QPen pfine(QColor(60, 60, 60), 1.0);
            painter->setPen(pfine);
            gridStep = mGridSize;
            left = qFloor(tl.x() / gridStep - 0.5);
            right = qFloor(br.x() / gridStep + 1.0);
            bottom = qFloor(tl.y() / gridStep - 0.5);
            top = qFloor(br.y() / gridStep + 1.0);

            for (int x = int(left); x <= int(right); ++x)
                painter->drawLine(x * gridStep, bottom * gridStep, x * gridStep, top * gridStep);

            for (int y = int(bottom); y <= int(top); ++y)
                painter->drawLine(left * gridStep, y * gridStep, right * gridStep, y * gridStep);
        }
        QPen pcoarse(QColor(25, 25, 25), 1.0);
        painter->setPen(pcoarse);
        gridStep = mGridSize*10;
        left = qFloor(tl.x() / gridStep - 0.5);
        right = qFloor(br.x() / gridStep + 1.0);
        bottom = qFloor(tl.y() / gridStep - 0.5);
        top = qFloor(br.y() / gridStep + 1.0);

        for (int x = int(left); x <= int(right); ++x)
            painter->drawLine(x * gridStep, bottom * gridStep, x * gridStep, top * gridStep);

        for (int y = int(bottom); y <= int(top); ++y)
            painter->drawLine(left * gridStep, y * gridStep, right * gridStep, y * gridStep);
    }
}   /* end drawBackground */

bool myGraphicsView::gridVisible() const
{
    return mGridVisible;
}

void myGraphicsView::setGridVisible(bool gridVisible) {
    mGridVisible = gridVisible;
    resetCachedContent();
}

void myGraphicsView::setGridSize(int size)
{
    mGridSize = size;
    resetCachedContent();
}

int myGraphicsView::getGridSize() const
{
    return mGridSize;
}

void myGraphicsView::mySetScene(myGraphicsScene *scene)
{
    myScene = scene;
    QGraphicsView::setScene(myScene);

}

