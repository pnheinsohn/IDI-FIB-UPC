#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent), program(NULL) {
  setFocusPolicy(Qt::StrongFocus); // per rebre events de teclat
  scale = 1.0f;
  rot_angle = 0.0f;
}

MyGLWidget::~MyGLWidget() {
  if (program != NULL) delete program;
}

void MyGLWidget::initializeGL() {
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  creaBuffers();

  initCamera();
}

void MyGLWidget::carregaShaders() {
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs(QOpenGLShader::Fragment, this);
  QOpenGLShader vs(QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation(program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation(program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  // Transformacion de proyección
  projLoc = glGetUniformLocation(program->programId(), "proj");
  // Transformacion de punto de vista
  viewLoc = glGetUniformLocation(program->programId(), "view");
}

void MyGLWidget::creaBuffers() {
  ///////////
  // HOMER //
  ///////////

  m.load("./models/HomerProves.obj");

  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  glGenBuffers(1, &VBO_Homer[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_Homer[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  ///////////
  // TERRA //
  ///////////

  glm::vec3 Vertices[6];
  Vertices[0] = glm::vec3(-2.0, -1.0, -2.0);
  Vertices[1] = glm::vec3(2.0, -1.0, -2.0);
  Vertices[2] = glm::vec3(-2.0, -1.0, 2.0);
  Vertices[3] = glm::vec3(-2.0, -1.0, 2.0);
  Vertices[4] = glm::vec3(2.0, -1.0, -2.0);
  Vertices[5] = glm::vec3(2.0, -1.0, 2.0);

  glm::vec3 Colors[6];
  Colors[0] = glm::vec3(1.0, -1.0, 1.0);
  Colors[1] = glm::vec3(1.0, -1.0, 1.0);
  Colors[2] = glm::vec3(1.0, -1.0, 1.0);
  Colors[3] = glm::vec3(1.0, -1.0, 1.0);
  Colors[4] = glm::vec3(1.0, -1.0, 1.0);
  Colors[5] = glm::vec3(1.0, -1.0, 1.0);

  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_Terra[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_Terra[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::initCamera() {
  perspective = false;
  
  radioCenter();

  float zDistance = radio * 3;

  VRP = center;
  // OBS: for Orthogonal, it might be better change y coord to 1.0
  OBS = VRP + glm::vec3(0.0, 0.0, zDistance);
  UP = glm::vec3(0.0, 0.1, 0.0);
  viewTransform();

  ra = 1.0;
  zNear = radio;
  zFar = zDistance + radio;
  
  FOV = asin(radio / zDistance);

  top = radio / 2.0;
  left = -radio / 2.0;
  right = radio / 2.0;
  bottom = -radio / 2.0;

  projectTransform();
}

void MyGLWidget::paintGL() {
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
  #ifdef __APPLE__
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    ample = vp[2];
    alt = vp[3];
  #endif

  // En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
  // useu els paràmetres que considereu (els que hi ha són els de per defecte)
  //  glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(VAO_Homer);
  modelTransformHomer();
  glDrawArrays(GL_TRIANGLES, 0, m.faces().size() * 3);

  glBindVertexArray(VAO_Terra);
  modelTransformTerra();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
}

void MyGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);

  ra = float(w) / float(h);

  if (perspective) {
    // if raV < 1 the model changes and FOV must change
    if (ra < 1.0) FOV = 2.0 * atan(tan(float(M_PI) / 4.0) / ra);
    else FOV = float(M_PI) / 4.0;
  }
  else {
    // if its orthogonal, and ra != 1, then the dimensions must change
    if (ra < 1.0) {
      top = radio / 2.0 / ra + 1;
      left = -radio / 2.0 - 1;
      right = radio / 2.0 + 1;
      bottom = -radio / 2.0 / ra - 1;
    }
    else if (ra > 1.0) {
      top = radio / 2.0 + 1;
      left = -radio / 2.0 * ra - 1;
      right = radio / 2.0 * ra + 1;
      bottom = -radio / 2.0 - 1;
    }
  }

  projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
  makeCurrent();
  switch(event->key()) {
  case Qt::Key_S: { // escalar a més gran
    scale += 0.05;
    break;
  } case Qt::Key_D: { // escalar a més petit
    scale -= 0.05;
    break;
  } case Qt::Key_R: {
    rot_angle += float(M_PI) / 4;
    break;
  } default:
    event->ignore();
    break;
  }
  update();
}

void MyGLWidget::modelTransformHomer() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform, rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformTerra() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform() {
  /** Parámetros perspective
   * FOV: Ángulo de abertura de la cámara (en radianes siempre)
   * ra: Ratio del viewPort al cual se le aplica la vista
   * zNear: distancia mínima a la cual se puede acercar del centro
   * zFar: distancia máxima desde la cual el objeto es observable
   */
  glm::mat4 Proj;
  if (perspective) Proj = glm::perspective(FOV, ra, zNear, zFar);
  else Proj = glm::ortho(left, right, bottom, top, zNear, zFar);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
  /** Parámetros lookAt 
   * OBS: Posición de la cámara
   * VRP: Orientación de la visión de la cámara (punto central al que apunta)
   * UP: Sentido del vector que apunta hacia arriba de la cámara */
  glm::mat4 View = glm::lookAt(OBS, VRP, UP);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::radioCenter() {
  float xmin, xmax, ymin, ymax, zmin, zmax;
  xmin = xmax = m.vertices()[0];
  ymin = ymax = m.vertices()[0];
  zmin = zmax = m.vertices()[0];

  for (unsigned int i = 3; i < m.vertices().size(); i += 3) {
    if (m.vertices()[i] < xmin) xmin = m.vertices()[i];
    if (m.vertices()[i] > xmax) xmax = m.vertices()[i];
    if (m.vertices()[i + 1] < ymin) ymin = m.vertices()[i + 1];
    if (m.vertices()[i + 1] > ymax) ymax = m.vertices()[i + 1];
    if (m.vertices()[i + 2] < zmin) zmin = m.vertices()[i + 2];
    if (m.vertices()[i + 2] > zmax) zmax = m.vertices()[i + 2];
  }

  radio = sqrt(pow(xmax - xmin, 2) + pow(ymax - ymin, 2) + pow(zmax - zmin, 2)) / 2.0;
  center[0] = (xmax + xmin) / 2.0;
  center[1] = (ymax + ymin) / 2.0;
  center[2] = (zmax + zmin) / 2.0;
}
