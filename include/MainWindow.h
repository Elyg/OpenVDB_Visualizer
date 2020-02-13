#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <OpenGLWindow.h>


namespace Ui {
  class MainWindow;
}

class MainWindow :  public QMainWindow
{
  Q_OBJECT
public:
  
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
private:
  OpenGLWindow *m_openGLWindow;
  Ui::MainWindow *m_ui;
};


#endif // MAINWINDOW_H
