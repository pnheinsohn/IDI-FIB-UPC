#include <QApplication>
#include "Contrassenya.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Contrassenya contrassenya;
    contrassenya.show();
    return app.exec();
}
