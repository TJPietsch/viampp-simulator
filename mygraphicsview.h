#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
//#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <math.h>
#include <QDebug>
#include <QtMath>
#include <QContextMenuEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTransform>
#include <QMenu>
#include <QBrush>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItemIterator>
#include <QHeaderView>
#include <QStringList>
#include <QLineEdit>
#include <QWidgetAction>
#include <QStringList>

#include "mygraphicsscene.h"

class myGraphicsView : public QGraphicsView {

  Q_OBJECT

public:
    myGraphicsView(QWidget *parent = Q_NULLPTR);


public slots:
    void zoomIn();
    void zoomOut();
    void rotateZCW();
    void rotateZCCW();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    bool gridVisible() const;
    void setGridVisible(bool gridVisible);
    int getGridSize() const;
    void setGridSize(int size);
    void mySetScene(myGraphicsScene *scene);

private:
    int mGridSize = 15;
    QColor mGridColor = 0x0a8affu;
    bool mGridVisible = true;
    QPointF _clickPos;
    QTransform trans = transform();
    myGraphicsScene *myScene;
};

#endif // MYGRAPHICSVIEW_H

