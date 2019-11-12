#include "MyLineEdit.h"

MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent) {
    // connect(this, SIGNAL(returnPressed()), this, SLOT(responseReturn()));
}

void MyLineEdit::responseReturn() {
    emit sendText(text());
}