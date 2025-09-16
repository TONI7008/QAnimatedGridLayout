#ifndef QFLOWLAYOUT_H
#define QFLOWLAYOUT_H

#include <QGridLayout>
#include <QWidget>
#include <QEasingCurve>
#include <QParallelAnimationGroup>


class QFlowLayout : public QGridLayout
{
    Q_OBJECT
public:
    struct cellWrapper {
        short row = -1;
        short column = -1;
        short rowSpan = 1;
        short columnSpan = 1;

        // Default constructor
        cellWrapper() = default;

        // Parameterized constructor
        cellWrapper(short _row, short _column, short _rowSpan, short _columnSpan)
            : row(_row), column(_column), rowSpan(_rowSpan), columnSpan(_columnSpan) {}

        // Copy constructor
        cellWrapper(const cellWrapper&) = default;

        // Copy assignment operator
        cellWrapper& operator=(const cellWrapper&) = default;

        // Equality operator
        bool operator==(const cellWrapper& other) const {
            return row == other.row &&
                   column == other.column &&
                   rowSpan == other.rowSpan &&
                   columnSpan == other.columnSpan;
        }
    };

    struct WidgetWrapper {
        cellWrapper cell;
        QWidget *widget = nullptr;
        QSizePolicy originalSizePolicy;

        WidgetWrapper() = default;

        WidgetWrapper(cellWrapper _cell, QWidget *w, QSizePolicy policy)
            : cell(_cell), widget(w), originalSizePolicy(policy) {}

        bool isValid() const {
            return cell.row >= 0 && cell.column >= 0 &&
                   cell.rowSpan > 0 && cell.columnSpan > 0;
        }

        // Equality operator
        bool operator==(const WidgetWrapper &other) const {
            return cell == other.cell &&
                   widget == other.widget &&
                   originalSizePolicy == other.originalSizePolicy;
        }

        // Assignment operator
        WidgetWrapper &operator=(const WidgetWrapper &other) {
            if (this != &other) {
                cell = other.cell;
                widget = other.widget;
                originalSizePolicy = other.originalSizePolicy;
            }
            return *this;
        }
    };


    QFlowLayout(QWidget* parent=nullptr);
    ~QFlowLayout();
    void addWidget(QWidget *widget,short _row=-1,short _column=-1,short _rowSpan=1,short columnSpan=1, Qt::Alignment align=Qt::Alignment());
    bool isZoomed() const;
    void removeWidget(QWidget*);

    void setEasingCurve(const QEasingCurve &easingCurve);
    QEasingCurve easingCurve() const;
    void setAnimationDuration(int duration);
    int animationDuration() const;
    void showAll();
    void zoomTo(QWidget *widget);

    QWidget* zoomedWidget();

    bool isAnimating() const;

private:

    QMap<QWidget*, QRect> originalGeometries;

    QList<WidgetWrapper> widgetList;

    QEasingCurve m_easing;
    QWidget* zoomedWidgetPtr;
    short m_duration;
    bool m_isAnimating=false;
    QRect m_parentRectAtRepopulate;  // Store parent rect when geometries were captured

    void updateOriginalGeometriesForResize();
    cellWrapper getWidgetPosition(QWidget *widget) const;
    void repopulate();
    bool contains(QWidget *widget) const;
};



#endif // QFLOWLAYOUT_H
