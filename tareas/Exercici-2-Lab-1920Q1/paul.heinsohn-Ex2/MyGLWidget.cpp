#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniEscena ()   // Cal modificar aquest mètode...
{
  creaBuffersTerra();
  creaBuffersPilota();
  creaBuffersPatricio();

  centreEsc = glm::vec3(10, 3, 10);
  radiEsc = distance(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 6.0f, 20.0f)) / 2;

  centreBaseArbre = glm::vec3(-0.5, -0.35, 0.0);
  posPilota = glm::vec3(10, 0, 10);
}

void MyGLWidget::iniCamera()
{
  thirdPerson = true;
  angleX = 0.0;
  angleY = float(M_PI / 6.0f);
  obs = glm::vec3(2.0f, 6.5f, 2.0f);  // For !thirdPerson
  vrp = centreEsc;  // For thirdPerson
  up = glm::vec3(0,1,0);  // For !thirdPerson
  
  ra = 1.0;
  zn = radiEsc;
  zf = 3 * radiEsc;
  fov = float(M_PI / 3.0f);

  projectTransform();
  viewTransform();
}

void MyGLWidget::paintGL()
{
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
  
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra 
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Activem el VAO per a pintar la Pilota
  glBindVertexArray (VAO_Pil);
  modelTransformPilota ();
  glDrawArrays(GL_TRIANGLES, 0, pil.faces().size()*3);
  
  // // Activem el VAO per a pintar l'arbre
  // glBindVertexArray(VAO_Arbre);
  // modelTransformArbre ();
  // glDrawArrays(GL_TRIANGLES, 0, 12);

  glBindVertexArray(VAO_Patricio);
  modelTransformPatricio();
  glDrawArrays(GL_TRIANGLES, 0, patricio.faces().size()*3);

  glBindVertexArray(0);
}

void MyGLWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);

  ra = float(w) / float(h);
  if (ra < 1.0f) fov = 2.0f * atan(tan(float(M_PI / 3.0f)) / ra);

  projectTransform();
}

void MyGLWidget::modelTransformPilota ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG, posPilota);
  TG = glm::scale(TG, glm::vec3 (escalaPil, escalaPil, escalaPil));
  TG = glm::translate(TG, -centreBasePil);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformArbre () 
{
  glm::mat4 TG (1.0f); // Matriu de transformació
  TG = glm::translate(TG, glm::vec3(2.0, 0.0, 2.0));
  TG = glm::scale(TG, glm::vec3(escalaArbre, escalaArbre, escalaArbre));
  TG = glm::translate(TG, -centreBaseArbre);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio ()
{
  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, glm::vec3(2.0f, 0.0f, 2.0f));
  TG = glm::rotate(TG, float(M_PI) / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
  TG = glm::scale(TG, glm::vec3(1.0f, 6 * scalePatrick, 1.0f));
  TG = glm::translate(TG, -centreBase);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform()
{
  glm::mat4 Proj(1.0f);  // Matriu de projecció

  if (thirdPerson) Proj = glm::perspective(fov, ra, zn, zf);
  else Proj = glm::perspective(float(120.0f * M_PI / 180.0f), ra, 0.1f, distance(glm::vec3(2.0f, 6.5f, 2.0f), glm::vec3(20.0f, 0.0f, 20.0f)));

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform()
{
  glm::mat4 View(1.0f);  // Matriu de posició i orientació

  if (thirdPerson) {
    View = glm::translate(View, glm::vec3(0.0f, 0.0f, -2 * radiEsc));
    View = glm::rotate(View, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, -angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    View = glm::translate(View, -vrp);
  } else {
    View = glm::lookAt(obs, posPilota, up);
  }

  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Left: { 
	    posPilota[0] -= 0.5;
      viewTransform();
      break;
    }
    case Qt::Key_Right: { 
	    posPilota[0] += 0.5;
      viewTransform();
      break;
    }
    case Qt::Key_Up: { 
	    posPilota[2] -= 0.5;
      viewTransform();
      break;
    }
    case Qt::Key_Down: { 
	    posPilota[2] += 0.5;
      viewTransform();
      break;
    }
    case Qt::Key_C: { 
	    thirdPerson = !thirdPerson;
      viewTransform();
      projectTransform();
      break;
    }
    default: event->ignore(); break;
  }

  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE)
  {
    angleX += 0.1 * (e->y() - yClick) / 25;
    angleY -= 0.1 * (e->x() - xClick) / 25;
    viewTransform();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersPilota ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  pil.load("./models/Pilota.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (pil, escalaPil, centreBasePil);
  
  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Pil);
  glBindVertexArray(VAO_Pil);

  // Creació dels buffers del model patr
  GLuint VBO_Pil[2];
  // Buffer de posicions
  glGenBuffers(2, VBO_Pil);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Pil[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pil.faces().size()*3*3, pil.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de component difusa usada com a color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Pil[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pil.faces().size()*3*3, pil.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersArbre () 
{
  // Dades de les coordenades dels vèrtexs
  glm::vec3 Vertices[12];  
  Vertices[0] = glm::vec3(-0.55, -0.35, 0.0);  // tronc de l'arbre
  Vertices[1] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[2] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[3] = glm::vec3(-0.55, 0.05, 0.0);
  Vertices[4] = glm::vec3(-0.45, -0.35, 0.0);
  Vertices[5] = glm::vec3(-0.45, 0.05, 0.0);
  Vertices[6] = glm::vec3(-0.65, 0.05, 0.01);  // fulles de l'arbre
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

  escalaArbre = 10.0/0.7;  // volem l'arbre d'alçada 10
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO_Arbre);
  glBindVertexArray(VAO_Arbre);

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

void MyGLWidget::creaBuffersTerra ()
{
  // Dades del terra
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[6] = {
	glm::vec3(0.0, 0.0, 20.0),
	glm::vec3(20.0, 0.0, 20.0),
	glm::vec3(0.0, 0.0, 0.0),
	glm::vec3(0.0, 0.0, 0.0),
	glm::vec3(20.0, 0.0, 20.0),
	glm::vec3(20.0, 0.0, 0.0),
  }; 

  glm::vec3 color(0.0,0.6,0.0);

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 colterra[6] = {
	color, color, color, color, color, color
  };

  // Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersPatricio ()
{
  patricio.load("./models/Patricio.obj");

  calculaCapsaModel(patricio, scalePatrick, centreBase);

  glGenVertexArrays(1, &VAO_Patricio);
  glBindVertexArray(VAO_Patricio);

  GLuint VBO_Patricio[2];
  glGenBuffers(2, VBO_Patricio);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patricio.faces().size()*3*3, patricio.VBO_vertices(), GL_STATIC_DRAW);

  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patricio.faces().size()*3*3, patricio.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray(0);  
}
 
void MyGLWidget::carregaShaders()
{
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

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
}

