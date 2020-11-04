#include <QApplication>
#include "MainWindow.h"
#include "OpenGLWindow.h"

int main(int argc, char **argv)
{

    QApplication app(argc, argv);
    MainWindow window;
    window.resize(QSize(1024, 720));
    window.show();

    return app.exec();
}
