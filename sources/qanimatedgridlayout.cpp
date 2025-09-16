#include "qanimatedgridlayout.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QDebug>



QAnimatedGridLayout::QAnimatedGridLayout(QWidget *parent)
    : QGridLayout(parent), currentRow(0), currentCol(0), columnCountVar(3), widgetCount(0),
    animationDurationMs(250), easing(QEasingCurve::Linear), animationGroup(new QParallelAnimationGroup), zoomedWidgetPtr(nullptr) {

}

QAnimatedGridLayout::~QAnimatedGridLayout()
{
    delete animationGroup;
}

void QAnimatedGridLayout::addWidget(QWidget *widget) {
    if (!widget) return;
    updateColumnCount();

    QGridLayout::addWidget(widget, currentRow, currentCol);


    widgetList.append(new WidgetWrapper(widget, widget->sizePolicy()));
    widgetCount++;

    currentCol++;
    if (currentCol >= columnCountVar) {
        currentCol = 0;
        currentRow++;
    }
    widget->show();
    if (isZoomed()) widget->hide();
    update();
}

void QAnimatedGridLayout::addWidgetList(QList<QWidget *> widgetL)
{
    for(const auto& elt : widgetL){
        addWidget(elt);
    }
}

void QAnimatedGridLayout::removeWidget(QWidget *widget) {
    if (!widget) return;

    // Find the WidgetWrapper pointer corresponding to the widget
    auto it = std::find_if(widgetList.begin(), widgetList.end(), [widget](WidgetWrapper* elt) {
        return elt->widget == widget;
    });

    if (it != widgetList.end()) {
        delete *it;  // Delete the allocated WidgetWrapper
        widgetList.erase(it);
    }

    QGridLayout::removeWidget(widget);
    widget->hide();
    widgetCount--;
    originalGeometries.remove(widget);
    update();
}


void QAnimatedGridLayout::removeAll() {
    while (!widgetList.isEmpty()) {
        QWidget* widget=widgetList.takeFirst()->widget;
        if (!widget) return;
        QGridLayout::removeWidget(widget);
        widget->hide();
        widgetCount--;
        originalGeometries.remove(widget);

        delete widget;
    }
    widgetCount = 0;
    update();
}

void QAnimatedGridLayout::updateColumnCount() {
    if (!parentWidget()) return;
    int width = parentWidget()->width();
    columnCountVar = qMax(1, width / 150);
}

void QAnimatedGridLayout::rearrangeWidgets() {
    if (isZoomed()) return;

    currentRow = 0;
    currentCol = 0;
    updateColumnCount();

    animationGroup= new QParallelAnimationGroup(this);

    for (auto &wrapper : widgetList) {
        QWidget *widget = wrapper->widget;
        QGridLayout::addWidget(widget, currentRow, currentCol);

        originalGeometries[widget] = widget->geometry();

        QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
        anim->setEndValue(originalGeometries[widget]);
        anim->setDuration(50);
        anim->setEasingCurve(easing);
        animationGroup->addAnimation(anim);

        currentCol++;
        if (currentCol >= columnCountVar) {
            currentCol = 0;
            currentRow++;
        }
    }

    connect(animationGroup,&QParallelAnimationGroup::finished,this,[this]{
        update();
    });
    animationGroup->start();
}

void QAnimatedGridLayout::zoomTo(QWidget *widget) {
    if (!widget || isZoomed()) return;

    rearrangeWidgets();
    zoomedWidgetPtr = widget;
    zoomedWidgetPtr->raise();

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = widget->parentWidget()->rect().adjusted(left, top, -right, -bottom);
    QPoint zoomedCenter = fullSize.center();

    for (auto &wrapper : widgetList) {
        QWidget *w = wrapper->widget;
        if (w == widget) continue;

        QPoint widgetCenter = w->geometry().center();
        QPointF direction = QPointF(widgetCenter - zoomedCenter);

        if (qFuzzyIsNull(direction.manhattanLength())) {
            direction = QPointF(1.0, 1.0);
        }

        double length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        direction /= length; // Normalize direction

        int pushDistance = parentWidget()->width() / 1.5 + horizontalSpacing() + left + right;
        QPointF newCenterF = QPointF(widgetCenter) + direction * pushDistance;
        QPoint newCenter = newCenterF.toPoint();

        QRect newGeometry(newCenter - QPoint(w->width() / 2, w->height() / 2), w->size());

        QPropertyAnimation *anim = new QPropertyAnimation(w, "geometry");
        anim->setEndValue(newGeometry);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(easing);
        animGroup->addAnimation(anim);
    }

    QPropertyAnimation *zoomAnim = new QPropertyAnimation(widget, "geometry");
    zoomAnim->setEndValue(fullSize);
    zoomAnim->setDuration(animationDurationMs);
    zoomAnim->setEasingCurve(QEasingCurve::OutQuad);

    animGroup->addAnimation(zoomAnim);

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this, fullSize] {
        zoomedWidgetPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (auto &wrapper : widgetList) {
            if (wrapper->widget != zoomedWidgetPtr) {
                wrapper->widget->hide();
            }
        }
        zoomedWidgetPtr->setGeometry(fullSize);
        zoomedWidgetPtr->updateGeometry();
        update();
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void QAnimatedGridLayout::showAll() {
    if (!zoomedWidgetPtr) return;

    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect fullSize = parentWidget()->rect().adjusted(left, top, -right, -bottom);
    QPoint zoomedCenter = fullSize.center();


    QPropertyAnimation *anim = new QPropertyAnimation(zoomedWidgetPtr, "geometry");
    anim->setStartValue(fullSize);
    anim->setEndValue(originalGeometries[zoomedWidgetPtr]);
    anim->setDuration(animationDurationMs);
    anim->setEasingCurve(QEasingCurve::InQuad);
    animGroup->addAnimation(anim);

    for (auto &wrapper : widgetList) {
        if(wrapper->widget == zoomedWidgetPtr) continue;

        wrapper->widget->show();
        QPoint widgetCenter = wrapper->widget->geometry().center();
        QPoint direction = widgetCenter - zoomedCenter;

        if (direction.manhattanLength() == 0) {
            direction = QPoint(1, 1);
        }
        direction = direction / std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        int pushDistance = parentWidget()->width()/1.5 + horizontalSpacing()+left+right;
        QPoint newCenter = widgetCenter + direction * pushDistance;
        QRect newGeometry = QRect(newCenter - QPoint(wrapper->widget->width() / 2, wrapper->widget->height() / 2),
                                  wrapper->widget->size());

        QPropertyAnimation *anim = new QPropertyAnimation(wrapper->widget, "geometry");
        anim->setStartValue(newGeometry);
        anim->setEndValue(originalGeometries[wrapper->widget]);
        anim->setDuration(animationDurationMs);
        anim->setEasingCurve(easing);
        animGroup->addAnimation(anim);
    }

    connect(animGroup, &QParallelAnimationGroup::finished, this, [this] {
        zoomedWidgetPtr->setGeometry(originalGeometries[zoomedWidgetPtr]);
        auto zoomedWrapper = std::find_if(widgetList.begin(), widgetList.end(), [this](WidgetWrapper* elt) {
            return elt->widget == zoomedWidgetPtr;
        });
        if (zoomedWrapper != widgetList.end()) {
            zoomedWidgetPtr->setSizePolicy((*zoomedWrapper)->originalSizePolicy);
        }

        zoomedWidgetPtr = nullptr;
        rearrangeWidgets();
        // Restore original size policy of the zoomed widget
    });

    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void QAnimatedGridLayout::setAnimationDuration(int duration) {
    animationDurationMs = duration;
}

int QAnimatedGridLayout::animationDuration() const {
    return animationDurationMs;
}

void QAnimatedGridLayout::setEasingCurve(const QEasingCurve &easingCurve) {
    easing = easingCurve;
}

QEasingCurve QAnimatedGridLayout::easingCurve() const {
    return easing;
}

bool QAnimatedGridLayout::isZoomed() const {
    return zoomedWidgetPtr != nullptr;
}

QWidget* QAnimatedGridLayout::zoomedWidget() const {
    return zoomedWidgetPtr;
}
