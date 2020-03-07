#include <QApplication>
#include "MainWindow.h"
#include "OpenGLWindow.h"

int main(int argc, char **argv)
{
    // Set OpenGL Version information
    // Note: This format must be set before show() is called.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,3);
    format.setSamples(4);
    QApplication app(argc, argv);

    // Set the window up
    //MainWindow window;
    //window.setFormat(format);
    
    OpenGLWindow window;
    window.resize(QSize(1024, 720));
    window.setFormat(format);
    window.show();

    return app.exec();
}
