#include "MyLabel.h"

MyLabel::MyLabel(QWidget *parent=0) : QLabel(parent) {
    validation = GROC;
}

void MyLabel::checkPassword(QString s) {
    password = s;
    if (password != passwordConfirmation) validation = VERMELL;
    else if (password.length() < 6) validation = GROC;
    else validation = VERD;
    setColor();
}

void MyLabel::checkPasswordConfirmation(QString s) {
    passwordConfirmation = s;
    if (password != passwordConfirmation) validation = VERMELL;
    else if (password.length() < 6) validation = GROC;
    else validation = VERD;
    setColor();
}

void MyLabel::setValidation(int valid) {
    validation = (validity) valid;
}

void MyLabel::setColor() {
    emit sendValidation(validation);

    QString str;
    if (validation == VERMELL) {
        str = "background:rgb(255, 0, 0);";
    } else if (validation == GROC) {
        str = "background:rgb(255, 255, 0);";
    } else if (validation == VERD) {
        str = "background:rgb(0, 255, 0);";
    }
    setStyleSheet(str);
}

void MyLabel::setMyText() {
    QString str;
    if (validation == VERMELL) {
        str = "Contrassenya no coincident";
    } else if (validation == GROC) {
        str = "Contrassenya de menys de 6 caràcters";
    } else if (validation == VERD) {
        str = "Contrassenya vàlida";
    }
    setText(str);
}
