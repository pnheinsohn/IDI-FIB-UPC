#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent), program(NULL) {
  setFocusPolicy(Qt::StrongFocus); // per rebre events de teclat
  scale = 1.0f;
  rot_angle = 0.0f;
  perspective = true;
  min_scene = glm::vec3(-2.5, 0.0, -2.5);
  max_scene = glm::vec3(2.5, 4.0, 2.5);
  euler = glm::vec3(0.0, 0.0, 0.0);
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
  modelBox();
  centerRadius();

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
  // loadHomer();
  loadPatrick();
  createTerra();
  glBindVertexArray(0);
}

void MyGLWidget::loadHomer() {
  m.load("./models/HomerProves.obj");

  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  glGenBuffers(2, VBO_Homer);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::loadPatrick() {
  m.load("./models/Patricio.obj");

  glGenVertexArrays(1, &VAO_Patrick);
  glBindVertexArray(VAO_Patrick);

  glGenBuffers(2, VBO_Patrick);
  
  // Activem l'atribut vertexLoc
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patrick[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Activem l'atribut colorLoc
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patrick[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::createTerra() {
  glm::vec3 Vertices[6];
  Vertices[0] = glm::vec3(-2.5, 0.0, -2.5);
  Vertices[1] = glm::vec3(2.5, 0.0, -2.5);
  Vertices[2] = glm::vec3(-2.5, 0.0, 2.5);
  Vertices[3] = glm::vec3(-2.5, 0.0, 2.5);
  Vertices[4] = glm::vec3(2.5, 0.0, -2.5);
  Vertices[5] = glm::vec3(2.5, 0.0, 2.5);

  glm::vec3 Colors[6];
  Colors[0] = glm::vec3(1.0, -1.0, 1.0);
  Colors[1] = glm::vec3(1.0, -1.0, 1.0);
  Colors[2] = glm::vec3(1.0, -1.0, 1.0);
  Colors[3] = glm::vec3(1.0, -1.0, 1.0);
  Colors[4] = glm::vec3(1.0, -1.0, 1.0);
  Colors[5] = glm::vec3(1.0, -1.0, 1.0);

  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(2, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
}

void MyGLWidget::initCamera() {
  float zDistance = radius * 2;

  VRP = center;
  // OBS: for Orthogonal, it might be better change y coord to 1.0
  OBS = VRP + glm::vec3(0.0, 1.0, zDistance);
  UP = glm::vec3(0.0, 0.1, 0.0);
  viewTransform();

  ra = 1.0f;
  zNear = zDistance - radius;
  zFar = zDistance + radius;
  FOV = asin(radius / zDistance);

  top = radius / 2;
  left = -radius / 2;
  right = radius / 2;
  bottom = -radius / 2;

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

  // glBindVertexArray(VAO_Homer);
  glBindVertexArray(VAO_Patrick);

  viewTransform();
  projectTransform();
  modelTransform();
  glDrawArrays(GL_TRIANGLES, 0, m.faces().size() * 3);

  glBindVertexArray(VAO_Terra);
  terraTransform();
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
      top = radius / 2.0 / ra;
      left = -radius / 2.0;
      right = radius / 2.0;
      bottom = -radius / 2.0 / ra;
    }
    else if (ra > 1.0) {
      top = radius / 2.0;
      left = -radius / 2.0 * ra;
      right = radius / 2.0 * ra;
      bottom = -radius / 2.0;
    }
  }
  projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
  makeCurrent();
  switch(event->key()) {
  case Qt::Key_O: {
    perspective = !perspective;
    break;
  } case Qt::Key_S: { // escalar a més gran
    scale += 0.05;
    break;
  } case Qt::Key_D: { // escalar a més petit
    scale -= 0.05;
    break;
  } case Qt::Key_R: {
    rot_angle += float(M_PI) / 4;
    break;
  } case Qt::Key_Z: {
    FOV -= 0.1;
    break;
  } case Qt::Key_X: {
    FOV += 0.1;
    break;
  } default:
    event->ignore();
    break;
  }
  // Calls for paintGL
  update();
}

void MyGLWidget::mousePressEvent(QMouseEvent *event) {
  mousePos[0] = event->x();
  mousePos[1] = event->y();
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (abs(event->x() - mousePos[0]) > 25) euler.x -= 0.1 * ((event->x() - mousePos[0]) / 50);
  if (abs(event->y() - mousePos[1]) > 25) euler.y -= 0.1 * ((event->y() - mousePos[1]) / 50);
  update();
}

void MyGLWidget::modelTransform() {
  glm::mat4 transform(1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::scale(transform, glm::vec3(rescale));
  transform = glm::rotate(transform, rot_angle, glm::vec3(0.0f, 1.0f, 0.0f));
  transform = glm::translate(transform, -modelCenterBase);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::terraTransform() {
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
   * UP: Sentido del vector que apunta hacia arriba de la cámara
  */
  // glm::mat4 View = glm::lookAt(OBS, VRP, UP);
  // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);

  /** Rotación angulos de euler
   *  Recordar que los radianes giran positivamente en orientación antihorario
  */
  glm::mat4 View(1.0f);
  // -2 * radius coz zDistance is 2 * radius
  View = glm::translate(View, glm::vec3(0.0f, 0.0f, - 2 * radius));
  View = glm::rotate(View, - euler.z, glm::vec3(0.0, 0.0, 1.0));
  View = glm::rotate(View, euler.x, glm::vec3(1.0, 0.0, 0.0));
  View = glm::rotate(View, - euler.y, glm::vec3(0.0, 1.0, 0.0));
  View = glm::translate(View, -VRP);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::modelBox() {
  glm::vec3 mins, maxs;

  mins.x = maxs.x = m.vertices()[0];
  mins.y = maxs.y = m.vertices()[1];
  mins.z = maxs.z = m.vertices()[2];

  for (unsigned int i = 3; i < m.vertices().size(); i += 3) {
    if (m.vertices()[i] < mins.x) mins.x = m.vertices()[i];
    else if (m.vertices()[i] > maxs.x) maxs.x = m.vertices()[i];
    
    if (m.vertices()[i + 1] < mins.y) mins.y = m.vertices()[i + 1];
    else if (m.vertices()[i + 1] > maxs.y) maxs.y = m.vertices()[i + 1];
    
    if (m.vertices()[i + 2] < mins.z) mins.z = m.vertices()[i + 2];
    else if (m.vertices()[i + 2] > maxs.z) maxs.z = m.vertices()[i + 2];
  }

  rescale = 0.25f;
  modelCenterBase = glm::vec3((mins.x + maxs.x) / 2, mins.y, (mins.z + maxs.z) / 2);
}

void MyGLWidget::centerRadius() {
  center = (min_scene + max_scene) / glm::vec3(2.0);
  radius = distance(min_scene, max_scene) / 2.0;
}
