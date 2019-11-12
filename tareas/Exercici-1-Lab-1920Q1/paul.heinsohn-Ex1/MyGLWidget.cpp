#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget(QWidget* parent) : QOpenGLWidget(parent), program(NULL) {
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget() {
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL() {
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersArbre();
  creaBuffersEstel();
  anglegirArbre = 0.0;
}

void MyGLWidget::pintaArbre() {
  // Activem l'Array a pintar 
  glBindVertexArray(VAO1);
  // Aplicamos TG
  modelTransformArbre(anglegirArbre);
  // Pintem
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

void MyGLWidget::pintaEstel() {
  // Activem l'Array a pintar 
  glBindVertexArray(VAO2);
  // Aplicamos TG
  modelTransformEstel();
  // Pintem
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MyGLWidget::paintGL() {
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  glClear(GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Pintem tots dos quadrats cadascú amb la seva transformació
  pintaArbre();
  pintaEstel();

 
  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::modelTransformArbre(float anglegir) {
  glm::mat4 TG(1.0);  // Identidad
  TG = glm::translate(TG, glm::vec3(-0.50f, 0.0f, 0.0f));
  TG = glm::rotate(TG, anglegir, glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::translate(TG, glm::vec3(0.50f, 0.0f, 0.0f));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformEstel() {
  glm::mat4 TG(1.0); // Identidad
  TG = glm::translate(TG, glm::vec3 (0.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::resizeGL(int w, int h) {
  ample = w;
  alt = h;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_R: { // increment angle rotació arbre respecte eix Y
      anglegirArbre += float(M_PI/12);
      modelTransformArbre(anglegirArbre);
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffersArbre() {
  // Dades de les coordenades dels vèrtexs
  glm::vec3 Vertices[12];
  Vertices[0] = glm::vec3(-0.55, -0.35, 0.0);  // tronc de l'arbre
  Vertices[1] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[2] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[3] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[4] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[5] = glm::vec3(-0.45, 0.05, 0.0);
  Vertices[6] = glm::vec3(-0.65, 0.05, 0.01);  // copa de l'arbre
  Vertices[7] = glm::vec3(-0.35, 0.05, 0.01);
  Vertices[8] = glm::vec3(-0.5, 0.35, 0.01);
  Vertices[9] = glm::vec3(-0.35, 0.25, 0.01);
  Vertices[10] = glm::vec3(-0.65, 0.25, 0.01);
  Vertices[11] = glm::vec3(-0.5, -0.05, 0.01);
  
  // Dades de les colors dels vèrtexs
  glm::vec3 Colors[12];
  Colors[0] = glm::vec3(1.0, 0.6, 0.3);  // marró
  Colors[1] = glm::vec3(1.0, 0.6, 0.3);
  Colors[2] = glm::vec3(1.0, 0.6, 0.3);
  Colors[3] = glm::vec3(1.0, 0.6, 0.3);
  Colors[4] = glm::vec3(1.0, 0.6, 0.3);
  Colors[5] = glm::vec3(1.0, 0.6, 0.3);
  Colors[6] = glm::vec3(0.0, 1.0, 0.0);  // verd
  Colors[7] = glm::vec3(0.0, 1.0, 0.0);
  Colors[8] = glm::vec3(0.0, 1.0, 0.0);
  Colors[9] = glm::vec3(0.0, 1.0, 0.0);
  Colors[10] = glm::vec3(0.0, 1.0, 0.0);
  Colors[11] = glm::vec3(0.0, 1.0, 0.0);
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO1);
  glBindVertexArray(VAO1);

  GLuint VBOs[2];
  glGenBuffers(2, VBOs);
  // Creació del buffer amb les coordenades dels vèrtexs
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Creació del buffer amb els colors dels vèrtexs
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Desactivem el VAO
  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersEstel() {
  // Construcció del model de l'estel
  glm::vec3 Vertices[6];

  // estrella base 3 y altura 4
  // Vertices[0] = glm::vec3(-0.15, 0.47, 0.00);
  // Vertices[1] = glm::vec3(0.15, 0.47, 0.00);
  // Vertices[2] = glm::vec3(0.00, 0.87, 0.00);
  // Vertices[3] = glm::vec3(-0.15, 0.73, 0.00);
  // Vertices[4] = glm::vec3(0.15, 0.73, 0.00);
  // Vertices[5] = glm::vec3(0.00, 0.33, 0.00);

  // estrella base 3 y altura 3
  Vertices[0] = glm::vec3(-0.15, 0.50, 0.00);
  Vertices[1] = glm::vec3(0.15, 0.50, 0.00);
  Vertices[2] = glm::vec3(0.00, 0.80, 0.00);
  Vertices[3] = glm::vec3(-0.15, 0.70, 0.00);
  Vertices[4] = glm::vec3(0.15, 0.70, 0.00);
  Vertices[5] = glm::vec3(0.00, 0.40, 0.00);

  // Dades de les colors dels vèrtexs
  glm::vec3 Colors[6];
  Colors[0] = glm::vec3(1.0, 1.0, 1.0);  // White
  Colors[1] = glm::vec3(1.0, 1.0, 1.0);
  Colors[2] = glm::vec3(1.0, 1.0, 1.0);
  Colors[3] = glm::vec3(1.0, 1.0, 1.0);
  Colors[4] = glm::vec3(1.0, 1.0, 1.0);
  Colors[5] = glm::vec3(1.0, 1.0, 1.0);

  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);

  GLuint VBOs[2];
  glGenBuffers(2, VBOs);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::carregaShaders() {
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("./shaders/basicShader.frag");
  vs.compileSourceFile("./shaders/basicShader.vert");
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
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Obtenemos identificador para el atributo "TG" del vertex shader
  transLoc = glGetUniformLocation(program->programId(), "TG");
}
