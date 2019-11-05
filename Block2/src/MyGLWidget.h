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
    virtual void resizeGL(int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent(QKeyEvent *event);
    // mousePressEvent - Es cridat quan es prem una click
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

  private:
    void creaBuffers();
    void carregaShaders();
    
    // Only one must be loaded at the time
    void loadHomer();
    void loadPatrick();
    // This transform is for homer and patrick
    void modelTransform();

    // Loaded with homer or patrick
    void createTerra();
    // This transform is for terra
    void terraTransform();
    
    // Método para configurar la proyección (óptica)
    void projectTransform();
    // Método para configurar la vista (posición)
    void viewTransform();
    // Gets the center base of patrick and rescales it
    void modelBox();
    // Gets the center & radius of the scene and sets the center of the camera
    void centerRadius();
    // Initializes camera params
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
    GLuint VAO_Patrick, VBO_Patrick[2];
    GLuint VAO_Terra, VBO_Terra[2];
    
    // Program
    QOpenGLShaderProgram *program;
    
    // Internal vars
    float scale, rescale;
    glm::vec3 pos;
    GLfloat rot_angle;
    
    // Scene params
    glm::vec3 min_scene, max_scene;

    // Project transform parameters
    bool perspective;
    float FOV, ra, zNear, zFar, d;  // Perspective
    float left, right, bottom, top;  // Ortogonal
    // View transform parameters
    glm::vec3 OBS, VRP, UP;

    // Radius & center
    glm::vec3 center, modelCenterBase;
    float radius, modelRadius, terraRadius;

    // Euler's & Mouse
    GLint mousePos[2];
    glm::vec3 euler;
};

