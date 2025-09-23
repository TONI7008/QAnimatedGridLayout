#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <qflowlayout.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget* widget=new QWidget();
    QFlowLayout *layout = new QFlowLayout(widget);

    layout->setContentsMargins(10,10,10,10);

    for(short row=0;row<4;row++){
        for (int column = 0; column < 3; ++column) {
            QPushButton* button=new QPushButton();
            if(row==0&&column==0){
                button->setText("Zoomed");
                button->setStyleSheet("background-color: red");
                layout->addWidget(button,row,column,1,2);
            } else {
                if(row==0 && column==1){
                    continue; // already occupied by the zoomed button
                }
                button->setStyleSheet("background-color: lightgray");
                button->setText(QString("(%1,%2)").arg(row).arg(column));
                layout->addWidget(button,row,column);
            }
            QObject::connect(button,&QPushButton::clicked,&a,[layout,button]{
                if(layout->isZoomed()){
                    layout->showAll();
                    return;
                }
                layout->zoomTo(button);
            });
        }
    }

    widget->setLayout(layout);
    widget->show();

    short b=a.exec();
    widget->deleteLater();

    return b;
}
