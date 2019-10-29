#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model/model.h"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL();
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL();
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void creaBuffers();
    void carregaShaders();
    void modelTransformHomer();
    void modelTransformTerra();
    // Método para configurar la proyección (óptica)
    void projectTransform();
    // Método para configurar la vista (posición)
    void viewTransform();
    //
    void radioCenter();
    //
    void initCamera();

    // Model
    Model m;
    Model vectorModels[3];
    
    // attribute locations
    GLuint vertexLoc, colorLoc;
    
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    
    // VAO i VBO names
    GLuint VAO_Homer, VBO_Homer[2];
    GLuint VAO_Terra, VBO_Terra[2];
    
    // Program
    QOpenGLShaderProgram *program;
    
    // Internal vars
    float scale;
    glm::vec3 pos;
    GLfloat rot_angle;
    
    // Project transform parameters
    bool perspective;
    float FOV, ra, zNear, zFar, d;  // Perspective
    float left, right, bottom, top;  // Ortogonal
    // View transform parameters
    glm::vec3 OBS, VRP, UP;

    // Radio & center
    float radio;
    glm::vec3 center;
};

