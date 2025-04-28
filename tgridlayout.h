#ifndef TGRIDLAYOUT_H
#define TGRIDLAYOUT_H

#include <QGridLayout>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QMap>
#include <QPropertyAnimation>
#include <QList>

class TGridLayout : public QGridLayout {
    Q_OBJECT

public:
    explicit TGridLayout(QWidget *parent = nullptr);
    ~TGridLayout();

    void addWidget(QWidget *widget);
    void addWidgetList(QList<QWidget*> widgetL);
    void removeWidget(QWidget *widget);
    void removeAll();
    void zoomTo(QWidget *widget);
    void showAll();
    void setAnimationDuration(int duration);
    int animationDuration() const;
    void setEasingCurve(const QEasingCurve &easing);
    QEasingCurve easingCurve() const;
    bool isZoomed() const;
    QWidget* zoomedWidget() const;
    int rowCount() const;
    int columnCount() const;
    void updateColumnCount();
    void rearrangeWidgets();

private:
    struct WidgetWrapper {
        QWidget *widget;
        QSizePolicy originalSizePolicy;
        WidgetWrapper(QWidget *w, QSizePolicy policy)
            : widget(w), originalSizePolicy(policy) {}
    };

    QList<WidgetWrapper*> widgetList;
    int currentRow;
    int currentCol;
    int columnCountVar;
    int widgetCount;
    int animationDurationMs;

    QEasingCurve easing;
    QParallelAnimationGroup *animationGroup;
    QWidget *zoomedWidgetPtr;
    QMap<QWidget*, QRect> originalGeometries;
    QMap<QWidget*, QRect> targetGeometries;


};

#endif // TGRIDLAYOUT_H
