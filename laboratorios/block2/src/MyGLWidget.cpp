#include "MyGLWidget.h"
#include <iostream>


MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent), program(NULL) {
  setFocusPolicy(Qt::StrongFocus); // per rebre events de teclat
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
  
  initEscena();
  initCamera();
}

void MyGLWidget::initEscena() {
  
  creaBuffersTerra();
  creaBuffersPatrick();
  // creaBuffersHomer();
  
  min_scene = glm::vec3(-2.5f, 0.0f, -2.5f);
  max_scene = glm::vec3(2.5f, 6.0f / 4.0f, 2.5f);
  centerEsc = (min_scene + max_scene) / glm::vec3(2.0f);
  radiusEsc = distance(min_scene, max_scene) / 2.0f;

  scaleEsc = 1.0f;

  scaleHomer = 1.0f;
  rotAngleHomer = 0.0f;
  
  scalePatrick = 0.25;
  rotAnglePatrick = 0.0f;
}

void MyGLWidget::initCamera() {
  perspective = true;
  euler = glm::vec3(0.0f, 0.0f, 0.0f);
  obs = vrp + glm::vec3(0.0f, 1.0f, 2 * radiusEsc);  // Not used
  vrp = centerEsc;
  up = glm::vec3(0.0f, 1.0f, 0.0f);  // Not used

  ra = 1.0f;
  zn = radiusEsc;
  zf = 3 * radiusEsc;
  fov = float(M_PI / 3.0f);

  top = radiusEsc;
  left = -radiusEsc;
  right = radiusEsc;
  bottom = -radiusEsc;

  projectTransform();
  viewTransform();
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

  glBindVertexArray(VAO_Patrick);
  modelTransformPatrick();
  glDrawArrays(GL_TRIANGLES, 0, patrick.faces().size() * 3);

  // glBindVertexArray(VAO_Homer);
  // modelTransformHomer();
  // glDrawArrays(GL_TRIANGLES, 0, homer.faces().size() * 3);

  glBindVertexArray(VAO_Terra);
  modelTransformTerra();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray(0);
}

void MyGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);

  ra = float(w) / float(h);
  if (perspective && ra < 1.0f) fov = 2.0 * atan(tan(float(M_PI / 3.0f)) / ra);
  else if (perspective) fov = float(M_PI / 3.0f);
  // if its orthogonal, and ra != 1, then the dimensions must change
  else if (ra < 1.0f) {
    top = radiusEsc / ra;
    left = -radiusEsc;
    right = radiusEsc;
    bottom = -radiusEsc / ra;
  } else if (ra > 1.0f) {
    top = radiusEsc;
    left = -radiusEsc * ra;
    right = radiusEsc * ra;
    bottom = -radiusEsc;
  }

  projectTransform();
}

void MyGLWidget::modelTransformTerra() {
  glm::mat4 TG(1.0f);
  TG = glm::scale(TG, glm::vec3(scaleEsc));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformHomer() {
  glm::mat4 TG(1.0f);
  TG = glm::scale(TG, glm::vec3(scaleEsc));
  TG = glm::scale(TG, glm::vec3(scaleHomer));
  TG = glm::rotate(TG, rotAngleHomer, glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::translate(TG, -centerBaseHomer);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatrick() {
  glm::mat4 TG(1.0f);
  TG = glm::scale(TG, glm::vec3(scaleEsc));
  TG = glm::scale(TG, glm::vec3(scalePatrick));
  TG = glm::rotate(TG, rotAnglePatrick, glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::translate(TG, -centerBasePatrick);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform() {
  /** Parámetros perspective
   * fov: Ángulo de abertura de la cámara (en radianes siempre)
   * ra: Ratio del viewPort al cual se le aplica la vista
   * zn: distancia mínima a la cual se puede acercar del centro
   * zf: distancia máxima desde la cual el objeto es observable
   */
  glm::mat4 Proj;

  if (perspective) Proj = glm::perspective(fov, ra, zn, zf);
  else Proj = glm::ortho(left, right, bottom, top, zn, zf);

  glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
  /** Parámetros lookAt 
   * OBS: Posición de la cámara
   * vrp: Orientación de la visión de la cámara (punto central al que apunta)
   * up: Sentido del vector que apunta hacia arriba de la cámara
  */
  // glm::mat4 View = glm::lookAt(OBS, vrp, up);
  // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);

  /** Rotación angulos de euler
   *  Recordar que los radianes giran positivamente en orientación antihorario
  */
  glm::mat4 View(1.0f);
  // -2 * radiusEsc coz zDistance is 2 * radiusEsc
  View = glm::translate(View, glm::vec3(0.0f, 0.0f, - 2 * radiusEsc));
  View = glm::rotate(View, - euler.z, glm::vec3(0.0, 0.0, 1.0));
  View = glm::rotate(View, euler.x, glm::vec3(1.0, 0.0, 0.0));
  View = glm::rotate(View, - euler.y, glm::vec3(0.0, 1.0, 0.0));
  View = glm::translate(View, -vrp);
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent *event) {
  makeCurrent();
  switch(event->key()) {
  case Qt::Key_O: {
    perspective = !perspective;
    viewTransform();
    projectTransform();
    break;
  } case Qt::Key_S: { // escalar a més gran
    scaleEsc += 0.05;
    break;
  } case Qt::Key_D: { // escalar a més petit
    scaleEsc -= 0.05;
    break;
  } case Qt::Key_R: {
    rotAnglePatrick += float(M_PI) / 4;
    break;
  } case Qt::Key_Z: {
    fov -= 0.1;
    break;
  } case Qt::Key_X: {
    fov += 0.1;
    break;
  } default:
    event->ignore();
    break;
  }
  // Calls for paintGL
  update();
}

void MyGLWidget::mousePressEvent(QMouseEvent *event) {
  xClick = event->x();
  yClick = event->y();

  if (event->button() & Qt::LeftButton &&
      ! (event->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *event) {
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event) {
  makeCurrent();
  if (DoingInteractive == ROTATE) {
    euler.x += 0.1 * (event->y() - yClick) / 25;
    euler.y -= 0.1 * (event->x() - xClick) / 25;
    viewTransform();
  }
  
  xClick = event->x();
  yClick = event->y();
  update();
}

void MyGLWidget::getModelBox(Model &m, float &scale, glm::vec3 &centerBase) {
  glm::vec3 mins, maxs;

  mins.x = maxs.x = m.vertices()[0];
  mins.y = maxs.y = m.vertices()[1];
  mins.z = maxs.z = m.vertices()[2];

  for (unsigned int i = 3; i < m.vertices().size(); i += 3) {
    if (m.vertices()[i + 0] < mins.x) mins.x = m.vertices()[i + 0];
    if (m.vertices()[i + 0] > maxs.x) maxs.x = m.vertices()[i + 0];
    
    if (m.vertices()[i + 1] < mins.y) mins.y = m.vertices()[i + 1];
    if (m.vertices()[i + 1] > maxs.y) maxs.y = m.vertices()[i + 1];
    
    if (m.vertices()[i + 2] < mins.z) mins.z = m.vertices()[i + 2];
    if (m.vertices()[i + 2] > maxs.z) maxs.z = m.vertices()[i + 2];
  }

  // std::cout << maxs.y - mins.y << std::endl;

  scale = 1.0f;
  centerBase = glm::vec3((mins.x + maxs.x) / 2, mins.y, (mins.z + maxs.z) / 2);
}

void MyGLWidget::creaBuffersTerra() {
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

  GLuint VBO_Terra[2];
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

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersHomer() {
  homer.load("./models/HomerProves.obj");

  getModelBox(homer, scaleHomer, centerBaseHomer);

  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  GLuint VBO_Homer[2];
  glGenBuffers(2, VBO_Homer);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * homer.faces().size() * 3 * 3, homer.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * homer.faces().size() * 3 * 3, homer.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersPatrick() {
  patrick.load("./models/Patricio.obj");

  getModelBox(patrick, scalePatrick, centerBasePatrick);

  glGenVertexArrays(1, &VAO_Patrick);
  glBindVertexArray(VAO_Patrick);

  GLuint VBO_Patrick[2];
  glGenBuffers(2, VBO_Patrick);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patrick[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patrick.faces().size() * 3 * 3, patrick.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patrick[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patrick.faces().size() * 3 * 3, patrick.VBO_matdiff(), GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
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