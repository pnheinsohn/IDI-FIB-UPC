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
    MyGLWidget(QWidget *parent=0);
    ~MyGLWidget();

  signals:
    void sendFOV(int value);
    void sendScale(double value);
    void toggleChangeCamera();

  public slots:
    void changeModel();
    void changeCamera();
    void setFOV(int value);
    void setScale(double value);

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
    virtual void mouseMoveEvent(QMouseEvent *event);

  private:
    void creaBuffers();
    void creaBuffersTerra();
    void creaBuffersHomer();  // Solo homer o patricio al mismo tiempo
    void creaBuffersPatrick();
    void creaBuffersLegoman();
    void carregaShaders();
    // Método para configurar la proyección (óptica)
    void projectTransform();
    // Método para configurar la vista (posición)
    void viewTransform();
    void modelTransformHomer();
    void modelTransformPatrick(int patrickId);
    void modelTransformLegoman();
    void modelTransformTerra();
    
    // Gets the center base of patrick and rescales it
    void getModelBox(Model &m, float &scale, glm::vec3 &centerBase);
    // Initializes camera params
    void initEscena();
    void initCamera();

    // Program
    QOpenGLShaderProgram *program;
    // VAO names
    GLuint VAO_Homer, VAO_Patrick[3], VAO_Legoman, VAO_Terra;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, colorLoc;

    // Model
    bool showPatricks;
    Model homer, patrick, legoman;
    
    // Radius & center
    glm::vec3 centerBaseHomer, centerBasePatrick, centerBaseLegoman;
    
    // Internal vars
    glm::vec3 pos;
    float scaleEsc, scaleHomer, scalePatrick, scaleLegoman;
    GLfloat rotAngleHomer, rotAnglePatrick, rotAngleLegoman;

    // Camera and Scene Params
    bool perspective;
    glm::vec3 min_scene, max_scene;
    glm::vec3 centerEsc, obs, vrp, up;
    float radiusEsc, ra, fov, zn, zf;  // Radius + perspective

    float left, right, bottom, top;  // Ortogonal

    // Euler's & Mouse
    glm::vec3 euler;

    typedef enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
};

