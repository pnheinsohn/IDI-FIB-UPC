#include "MyLabel.h"


MyLabel::MyLabel(QWidget *parent=0) : QLabel(parent) {
}

void MyLabel::setRedColor(int value) {
    redColor = value;
    QString str = "background:rgb(" + QString::number(redColor) + "," + QString::number(greenColor) + "," + QString::number(blueColor) + ");";
    setStyleSheet(str);
}

void MyLabel::setGreenColor(int value) {
    greenColor = value;
    QString str = "background:rgb(" + QString::number(redColor) + "," + QString::number(greenColor) + "," + QString::number(blueColor) + ");";
    setStyleSheet(str);
}

void MyLabel::setBlueColor(int value) {
    blueColor = value;
    QString str = "background:rgb(" + QString::number(redColor) + "," + QString::number(greenColor) + "," + QString::number(blueColor) + ");";
    setStyleSheet(str);
}
