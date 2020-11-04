#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    this->setCentralWidget(m_ui->centralwidget);
    
    m_redirect = new QDebugStream(std::cout, m_ui->infoBox);
    
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,3);
    format.setSamples(4);
    
    m_ui->openGLWindow->setRampWidget(m_ui->rampWidget);
    m_ui->openGLWindow->setFormat(format);
    
    m_ui->parmLayout->setAlignment(Qt::AlignTop);
    connect(m_ui->fitMin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_ui->openGLWindow, &OpenGLWindow::fitMinValuesChanged);
    connect(m_ui->fitMax, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_ui->openGLWindow, &OpenGLWindow::fitMaxValuesChanged);
    connect(m_ui->densityMulti, QOverload<double>::of(&QDoubleSpinBox::valueChanged), m_ui->openGLWindow, &OpenGLWindow::densityValuesChanged);
    
    connect(m_ui->filePath, &QLineEdit::textEdited, m_ui->openGLWindow, &OpenGLWindow::vdbPathChanged);
    connect(m_ui->slices, QOverload<int>::of(&QSpinBox::valueChanged), m_ui->openGLWindow, &OpenGLWindow::slicesChanged);
    
    connect(m_ui->browseButton, &QPushButton::clicked, this, &MainWindow::fileBrowse);
}

MainWindow::~MainWindow()
{
  delete m_openGLWindow;
  delete m_redirect;
}

void MainWindow::fileBrowse()
{
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open VDB File"), "../../data", tr("VDB Files (*.vdb)"));
  if(fileName != "")
  {
    m_ui->filePath->setText(fileName);
    m_ui->filePath->textEdited(fileName);
  }
}
