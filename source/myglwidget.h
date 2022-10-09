#ifndef MYGLWIDGET
#define MYGLWIDGET

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <vector>
#include <QGLWidget>
#include <string>
#include "mainwindow.h"
#include <sstream>
#include "header.h"
#include <QPainter>
#include <QMatrix3x3>
#include <ctime>

#define RATIO 0.7
#define CYLINDER_COLOR 0.6
#define CYLINDER_RES 20
#define SPHERE_RES 20
#define CLICK_COLOR 0.4
#define INACTIVE_COLOR 0.8

typedef Unit_Cell Unit_Cell;
typedef Atom Atom;
void drawSphere(Atom &atom, GLUquadric* quad, bool active, bool clicked);
void drawCylinder(Atom &cobalt, Atom &oxygen, GLUquadric* quad, bool active);
void drawLines(Unit_Cell &cell);
void drawAngles(vector<Unit_Cell> &cell, Manager &manage);
void drawOctahedra(Unit_Cell &cell, string type);
void drawPlanes(Unit_Cell &cell);
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar );
void drawAllFourVectors(Unit_Cell &cell, int choice);


double Red[3] = {1, 0, 0};
double Green[3] = {0, 1, 0};
double Blue[3] = {0, 0, 1};
double Yellow[3] = {1,1,0};
double Cyan[3] = {0, 1, 1};
double Magenta[3] = {1, 0, 1};
double White[3] = {1, 1, 1};
double Black[3] = {0, 0, 0};

typedef string string;

using namespace std;

double X_INIT = -5;
double Y_INIT = 2;
double Z_INIT = -32;

using namespace std;


class MyGLWidget : public QGLWidget
{
public:
    MyGLWidget(QWidget *parent) : QGLWidget(parent)
    {
        this->setFocusPolicy(Qt::ClickFocus);
        this->installEventFilter(this);
    }
    MainWindow *window;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent  *event);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject* obj, QEvent *event);
    void draw_text(double x, double y, double z, QString txt);
    void renderPrimitive(Unit_Cell &cell, vector<bool> &clicked);
    void drawCompass(Unit_Cell &cell);
    void draw_information();
};

void MyGLWidget::initializeGL()
{
       configureColor();
}

void MyGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (float)w/ (float)h;
    perspectiveGL(45, ratio, 1.0, 200);

    glTranslatef(X_INIT, Y_INIT, Z_INIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Singleton& the_s = Singleton::getInstance();
    Manager manage = the_s.manage;
    vector<Selected_atom> all_selected = manage.getAll();
    vector<vector<bool> >  vec2d;
    vec2d.resize(the_s.cell.size());

    for(unsigned int k = 0; k < vec2d.size(); k++)
    {
        vec2d[k].resize(the_s.cell[k].backup.size());
        for(unsigned int m = 0; m < vec2d[k].size(); m++)
            vec2d[k][m] = false;
    }

    int cell;
    int atom;
    for(unsigned int k = 0; k < all_selected.size(); k++)
    {
        cell = all_selected[k].cell;
        atom = all_selected[k].clicked;
        vec2d[cell][atom] = true;
    }


    for(unsigned int i = 0; i < the_s.cell.size(); i++)
    {
        renderPrimitive(the_s.cell[i], vec2d[i]);
    }


    drawAngles(the_s.cell, the_s.manage);
    draw_information();
    this->update();

}


/**
 * @brief MyGLWidget::draw_information draws the text in the upper right corner
 * shows which files are opened and which are imported.
 */
void MyGLWidget::draw_information()
{
    Singleton& the_s = Singleton::getInstance();
    vector<string> file_names = the_s.manage.file_names;

    QString txt = "opened: ";
    unsigned int width = this->size().width();
    unsigned int height = this->size().height();

    Vec3d begin;
    Vec3d end;
    windowToObjectf(width*0.035, height*0.035, begin, end);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    this->qglColor(Qt::black);
    this->renderText(begin.x, begin.y, begin.z, txt, QFont("Arial", 10, QFont::Normal, false) );

    if(file_names.size() > 1)
        this->renderText(begin.x, begin.y - 0.025, begin.z, "imported: ", QFont("Arial", 10, QFont::Normal, false) );

    for(unsigned int i = 0; i < file_names.size(); i++)
        this->renderText(begin.x + 0.11, begin.y - 0.025*i, begin.z, file_names[i].c_str(), QFont("Arial", 10, QFont::Normal, false) );


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}



void MyGLWidget::draw_text(double x, double y, double z, QString txt)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    this->qglColor(Qt::black);
    this->renderText(x, y, z, txt, QFont("Arial", 12, QFont::Bold, false) );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;
    fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;
    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}


void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    mousePress(event, window);
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMove(event, window);
}


bool MyGLWidget::eventFilter(QObject* obj, QEvent *event)
{
        if (obj == this && event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            opengl_keypress(keyEvent);
            this->update();
            return true;
        }
        return false;
}



void MyGLWidget::keyPressEvent(QKeyEvent *event)
{
    opengl_keypress(event);
}


void configureColor()
{
     glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);   // Clear window.
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    GLfloat sun_direction[] = { 0.0, 0.0, 1.0, 0};
   // Specific spot effects

    GLfloat white[] = {0.5f, 0.5f, 0.5f, 1.0f};

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    GLfloat shininess[] = {50};
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);


    glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);



    glEnable(GL_COLOR_MATERIAL);   // Configure glColor().;
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

   //for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glEnable(GL_MULTISAMPLE);

    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_MODELVIEW);
}


/**
 * @brief MyGLWidget::renderPrimitive renders a single unit cell.
 * @param cell the Unit_Cell that is to be rendered
 * @param clicked all the selected atoms, this has to be specified, since they
 *                will be darker then other atoms. The variable 'clicked' in each
 *                atom is not used.
 */
void MyGLWidget::renderPrimitive (Unit_Cell &cell, vector<bool> &clicked) {

    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if(cell.backup.size() > 0)
        {
            drawPlanes(cell);
            drawLines(cell);
            MyGLWidget::drawCompass(cell);

        }

        GLUquadric* quad =  gluNewQuadric();
        for(unsigned int i = 0; i < cell.backup.size(); i++)
        {
            if(cell.backup[i].visible == true)
            {

                for(unsigned int j = 0; j < cell.backup[i].bonds.size(); j++)
                {
                    if (cell.backup[cell.backup[i].bonds[j]].visible == true &&
                            cell.geometry.visible_bonds == true)
                    {

                        drawCylinder(cell.backup[i], cell.backup[cell.backup[i].bonds[j]], quad, cell.active);
                    }
                }
                drawSphere(cell.backup[i], quad, cell.active, clicked[i]);
            }
        }

        gluDeleteQuadric(quad);

        if(cell.geometry.polygons == true && cell.backup.size() > 0)
        {
            drawOctahedra(cell, "Co");
        }


}

void Sphere(double r, int lats, int longs) {
int i, j;
for(i = 0; i <= lats; i++) {
    double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
   double z0  = sin(lat0);
   double zr0 =  cos(lat0);

   double lat1 = M_PI * (-0.5 + (double) i / lats);
   double z1 = sin(lat1);
   double zr1 = cos(lat1);

   glBegin(GL_QUAD_STRIP);
   for(j = 0; j <= longs; j++) {
       double lng = 2 * M_PI * (double) (j - 1) / longs;
       double x = r*cos(lng);
       double y = r*sin(lng);

       glNormal3f(x * zr0, y * zr0, z0);
       glVertex3f(x * zr0, y * zr0, r*z0);
       glNormal3f(x * zr1, y * zr1, z1);
       glVertex3f(x * zr1, y * zr1, r*z1);
   }
   glEnd();
}
}


/**
 * @brief drawSphere a sphere is drawn,
 * @param atom the location of the sphere
 * @param quad
 * @param active specifies whether a cell is active or not, necessary for color of sphere
 * @param clicked specifies whether this atom was clicked or not
 */

void drawSphere(Atom &atom, GLUquadric* quad, bool active, bool clicked)
{

    if (clicked == true && active == true)
    {
        glColor3f(atom.paint[0]*CLICK_COLOR, atom.paint[1]*CLICK_COLOR, atom.paint[2]*CLICK_COLOR);
    }
    else if (clicked == false && active == true)
    {
        glColor3f(atom.paint[0], atom.paint[1], atom.paint[2]);
    }
    else if(clicked == true && active == false)
    {
          glColor3f(atom.paint[0]*CLICK_COLOR*INACTIVE_COLOR, atom.paint[1]*CLICK_COLOR*INACTIVE_COLOR, atom.paint[2]*CLICK_COLOR*INACTIVE_COLOR);
    }
    else
    {
        glColor3f(atom.paint[0]*INACTIVE_COLOR, atom.paint[1]*INACTIVE_COLOR, atom.paint[2]*INACTIVE_COLOR);
    }


    glPushMatrix();
        glLoadIdentity();
        glTranslatef(atom.x, atom.y, atom.z);
        gluSphere(quad, atom.radius, SPHERE_RES, SPHERE_RES);
        //Sphere(atom.radius, SPHERE_RES, SPHERE_RES);
    glPopMatrix();
}


/**
 * @brief drawCylinder draws bonds from atom1 to atom2, here atom1 is called cobalt
 *                     and atom2 is called oxygen, the names are legacy
 * @param cobalt the first atom where bond information is located
 * @param oxygen the second atom where bonds go to
 * @param quad
 * @param active specifies whether a cell is active or not,
 *               necessary for the color of the bond
 */
void drawCylinder(Atom &cobalt, Atom &oxygen, GLUquadric* quad, bool active)
{

    Vec3d line = Vec3d(-cobalt.x + oxygen.x, -cobalt.y + oxygen.y, -cobalt.z + oxygen.z);
    Vec3d up = Vec3d(0, 0, 1);
    Vec3d result = crossProduct(line.normalized(), up);

    double height = sqrt(line*line);
    double angle = acos(line.normalized()*up)*180/M_PI;

    if(active == false)
    {
        glColor3f(cobalt.paint[0], cobalt.paint[1], cobalt.paint[2]);
    }

    else
    {
        glColor3f(cobalt.paint[0]*CYLINDER_COLOR, cobalt.paint[1]*CYLINDER_COLOR, cobalt.paint[2]*CYLINDER_COLOR);
    }

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(cobalt.x, cobalt.y, cobalt.z);
        glRotatef(-angle, result.x, result.y, result.z);
        gluCylinder(quad,  RATIO/6,  RATIO/6,  height,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();

/*
    if(active == false)
    {
        glColor3f(oxygen.paint[0], oxygen.paint[1], oxygen.paint[2]);
    }

    else
    {
        glColor3f(oxygen.paint[0]*CYLINDER_COLOR, oxygen.paint[1]*CYLINDER_COLOR, oxygen.paint[2]*CYLINDER_COLOR);
    }

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(cobalt.x + line.x/2.0, cobalt.y + line.y/2.0, cobalt.z + line.z/2.0);
        glRotatef(-angle, result.x, result.y, result.z);
        gluCylinder(quad,  RATIO/6,  RATIO/6,  height,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();
*/
}

void drawAngles(vector<Unit_Cell> &cell, Manager &manage)
{
    if(cell.size() < 1) return;

    glLineWidth(2.0);
    glColor3f(1, 0, 0);
    Selected_atom temp1;
    Selected_atom temp2;

    for(int i = 1; i < manage.getSize(); i++)
    {
        temp1 = manage.getAtom(i - 1);
        temp2 = manage.getAtom(i);

        glBegin(GL_LINES);
            glVertex3f(cell[temp1.cell].backup[temp1.clicked].x, cell[temp1.cell].backup[temp1.clicked].y, cell[temp1.cell].backup[temp1.clicked].z);
            glVertex3f(cell[temp2.cell].backup[temp2.clicked].x, cell[temp2.cell].backup[temp2.clicked].y, cell[temp2.cell].backup[temp2.clicked].z);
        glEnd();
    }

}


/**
 * @brief drawLines the edges of the Unit_Cell are drawn, based on the basis vectors
 * @param cell the input Unit_Cell
 */
void drawLines(Unit_Cell &cell)
{
    if(cell.geometry.isSet == false) return;
    if(cell.geometry.toggle_f == false) return;

    glLineWidth(2.37);

    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);

    if(cell.active == true)
    {
        glLineWidth(2.0);
        glColor3f(0.6, 0.5, 0.5);
    }

    else
    {
        glColor3f(0, 0, 0);
    }


    Vec3d a_vector = cell.getA();
    Vec3d b_vector = cell.getB();
    Vec3d c_vector = cell.getC();
    Vec3d zero = cell.getZero();
    Vec3d vector;

    glBegin(GL_LINE_STRIP);
        glVertex3f(zero.x, zero.y, zero.z);
        glVertex3f((zero + a_vector).x, (zero + a_vector).y, (zero + a_vector).z);
    glEnd();


    glBegin(GL_LINE_STRIP);
        glVertex3f(zero.x, zero.y, zero.z);
        glVertex3f(zero.x + b_vector.x, zero.y + b_vector.y, zero.z + b_vector.z);
    glEnd();

    vector = zero + c_vector;
    glBegin(GL_LINE_STRIP);
        glVertex3f(zero.x, zero.y, zero.z);
        glVertex3f(vector.x, vector.y, vector.z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + a_vector).x, (zero + a_vector).y, (zero + a_vector).z);
        glVertex3f((zero + a_vector + b_vector).x, (zero + a_vector + b_vector).y, (zero + a_vector + b_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + a_vector).x, (zero + a_vector).y, (zero + a_vector).z);
        glVertex3f((zero + a_vector + c_vector).x, (zero + a_vector + c_vector).y, (zero + a_vector + c_vector).z);
    glEnd();


    glBegin(GL_LINES);

        glVertex3f((zero + b_vector).x, (zero + b_vector).y, (zero + b_vector).z);
        glVertex3f((zero + a_vector + b_vector).x, (zero + a_vector + b_vector).y, (zero + a_vector + b_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + b_vector).x, (zero + b_vector).y, (zero + b_vector).z);
        glVertex3f((zero + b_vector + c_vector).x, (zero + b_vector + c_vector).y, (zero + b_vector + c_vector).z);
    glEnd();


    glBegin(GL_LINES);

        glVertex3f((zero + a_vector + b_vector).x, (zero + a_vector + b_vector).y, (zero + a_vector + b_vector).z);
        glVertex3f((zero + a_vector + b_vector + c_vector).x, (zero + a_vector + b_vector + c_vector).y, (zero + a_vector + b_vector + c_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + c_vector).x, (zero + c_vector).y, (zero + c_vector).z);
        glVertex3f((zero + a_vector + c_vector).x, (zero + a_vector + c_vector).y, (zero + a_vector + c_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + c_vector).x, (zero + c_vector).y, (zero + c_vector).z);
        glVertex3f((zero + b_vector + c_vector).x, (zero + b_vector + c_vector).y, (zero + b_vector + c_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + c_vector + b_vector).x, (zero + c_vector + b_vector).y, (zero + c_vector + b_vector).z);
        glVertex3f((zero + a_vector + c_vector + b_vector).x, (zero + a_vector + c_vector + b_vector).y, (zero + a_vector + c_vector + b_vector).z);
    glEnd();


    glBegin(GL_LINES);
        glVertex3f((zero + c_vector + a_vector).x, (zero + c_vector + a_vector).y, (zero + c_vector + a_vector).z);
        glVertex3f((zero + a_vector + c_vector + b_vector).x, (zero + a_vector + c_vector + b_vector).y, (zero + a_vector + c_vector + b_vector).z);
    glEnd();

    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);


}

/*
 * Draws the oxygen octahedra,
 * this method only applies if in a compound six oxygens exist that
 * form an octahedral formation around the cobalt atom
 * otherwise this method doesn't do much
*/

void drawOctahedra(Unit_Cell &cell, string type)
{

    Vec3d cobalt;
    Vec3d oxy1;
    Vec3d oxy2;
    Vec3d plane;
    vector<int> control;
    vector<Vec3d> parallel;
    vector<Vec3d> perpendicular;
    int temp;
    double cosine;

    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {
        if(cell.backup[i].visible == false || cell.backup[i].type.compare(type) != 0) continue;



        control.clear();
        parallel.clear();
        perpendicular.clear();

        for(unsigned int j = 0; j < cell.backup[i].bonds.size(); j++)
        {
            temp = cell.backup[i].bonds[j];
            if(cell.backup[temp].visible == true)
            {
                control.push_back(temp);
            }
        }

        if(control.size() < 3) continue;

        cobalt = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);
        oxy1 = Vec3d(cell.backup[control[0]].x, cell.backup[control[0]].y, cell.backup[control[0]].z);
        oxy2 = Vec3d(cell.backup[control[1]].x, cell.backup[control[1]].y, cell.backup[control[1]].z);

        cosine = (oxy1 - cobalt)*(oxy2 - cobalt)/(((oxy1 - cobalt).norm())*(oxy2 - cobalt).norm());
        plane = crossProduct(oxy1 - cobalt, oxy2 - cobalt);

        if(cosine > 0.8 || cosine < -0.8)
        {
            oxy2 = Vec3d(cell.backup[control[2]].x, cell.backup[control[2]].y, cell.backup[control[2]].z);
            plane = crossProduct(oxy1 - cobalt, oxy2 - cobalt);
            control[0] = -1;
            control[2] = -1;
        }

        else
        {
            control[0] = -1;
            control[1] = -1;
        }

        parallel.push_back(oxy1);
        parallel.push_back(oxy2);

        for(unsigned int f = 1; f < control.size(); f++)
        {
            if(control[f] == -1) continue;

            Vec3d put = Vec3d(cell.backup[control[f]].x, cell.backup[control[f]].y, cell.backup[control[f]].z);
            cosine = (put - cobalt)*plane/((put - cobalt).norm() * plane.norm());

            if(cosine > 0.8 || cosine < -0.8)  perpendicular.push_back(put);

            else
            {
                parallel.push_back(put);
            }
        }

        if(perpendicular.size() < 1) continue;


        insertionSort(parallel, cobalt, plane);

        glDepthMask(false);





        glColor4f(0,  0, 1.0, 0.5 + cell.geometry.polygon_shade);

        for (unsigned int m = 1; m < parallel.size(); m++)
        {
            glBegin(GL_POLYGON);
                glVertex3f(parallel[m - 1 ].x, parallel[m  - 1].y, parallel[m  - 1].z);
                glVertex3f(parallel[m].x, parallel[m].y, parallel[m].z);
                glVertex3f(perpendicular[0].x, perpendicular[0].y, perpendicular[0].z);
            glEnd();



            if (perpendicular.size() < 2) continue;


            glBegin(GL_POLYGON);
                glVertex3f(parallel[m  - 1].x, parallel[m - 1].y, parallel[m - 1].z);
                glVertex3f(parallel[m].x, parallel[m].y, parallel[m].z);
                glVertex3f(perpendicular[1].x, perpendicular[1].y, perpendicular[1].z);
            glEnd();
        }


            glBegin(GL_POLYGON);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
                glVertex3f(parallel[parallel.size()-1].x, parallel[parallel.size() - 1].y, parallel[parallel.size() - 1].z);
                glVertex3f(perpendicular[0].x, perpendicular[0].y, perpendicular[0].z);
            glEnd();


        if(perpendicular.size() >= 2)
        {

            glBegin(GL_POLYGON);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
                glVertex3f(parallel[parallel.size()-1].x, parallel[parallel.size() - 1].y, parallel[parallel.size() - 1].z);
                glVertex3f(perpendicular[1].x, perpendicular[1].y, perpendicular[1].z);
            glEnd();
        }



        glDepthMask(true);

   //glLineWidth(0.57);
        glDisable(GL_BLEND);
        glLineWidth(0.87);



        glDisable(GL_LIGHTING);
   //glColor3f(0.5,  0.5, 0.5);
        glColor3f(0.87,  0.87, 0.87);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        for (unsigned int m = 1; m < parallel.size(); m++)
        {

            glBegin(GL_LINE_STRIP);
                glVertex3f(parallel[m - 1 ].x, parallel[m  - 1].y, parallel[m  - 1].z);
                glVertex3f(parallel[m].x, parallel[m].y, parallel[m].z);
                glVertex3f(perpendicular[0].x, perpendicular[0].y, perpendicular[0].z);
                glVertex3f(parallel[m - 1 ].x, parallel[m  - 1].y, parallel[m  - 1].z);
            glEnd();


            if (perpendicular.size() >= 2)
            {

                glBegin(GL_LINE_STRIP);
                    glVertex3f(parallel[m - 1 ].x, parallel[m  - 1].y, parallel[m  - 1].z);
                    glVertex3f(parallel[m].x, parallel[m].y, parallel[m].z);
                    glVertex3f(perpendicular[1].x, perpendicular[1].y, perpendicular[1].z);
                    glVertex3f(parallel[m - 1 ].x, parallel[m  - 1].y, parallel[m  - 1].z);
                glEnd();
            }



        }

            glBegin(GL_LINE_STRIP);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
                glVertex3f(parallel[parallel.size()-1].x, parallel[parallel.size() - 1].y, parallel[parallel.size() - 1].z);
                glVertex3f(perpendicular[0].x, perpendicular[0].y, perpendicular[0].z);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
            glEnd();

        if(perpendicular.size() >= 2)
        {
            glBegin(GL_LINE_STRIP);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
                glVertex3f(parallel[parallel.size()-1].x, parallel[parallel.size() - 1].y, parallel[parallel.size() - 1].z);
                glVertex3f(perpendicular[1].x, perpendicular[1].y, perpendicular[1].z);
                glVertex3f(parallel[0].x, parallel[0].y, parallel[0].z);
            glEnd();
        }


    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);

    }

}

/*
 * Draws the planes that exist on the unit cell
*/
void drawPlanes(Unit_Cell &cell)
{
        glDepthMask(false);

        Vec3d point;

        for (unsigned int i = 0; i < cell.plane.size(); i++)
        {
            if(cell.plane[i].selected == true)
            {
                glColor4f(1.00, 0.43, 0.78, 0.7);
            }
            else
            {

                glColor4f(0.847059, 0.847059, 0.74902, 0.4);
            }

            glBegin(GL_POLYGON);
                for(unsigned int j = 0; j < cell.plane[i].edge.size(); j++)
                {
                    point = cell.plane[i].edge[j];
                    glVertex3f(point.x, point.y, point.z);
                }
            glEnd();
        }
        glDepthMask(true);

}


/*
 *this is matrix multiplication used only in the method 'drawAllFourVectors'
 */
Vec3d matrixMult(Vec3d input, Vec3d a, Vec3d b, Vec3d c)
{
    Vec3d answer;
    answer.x = a.x*input.x + b.x*input.y + c.x*input.z;
    answer.y = a.y*input.x + b.y*input.y + c.y*input.z;
    answer.z = a.z*input.x + b.z*input.y + c.z*input.z;
    return answer;
}




/*
 * This is a method currently not used anywhere in the program.
 * It was designed to draw arrows for spins in the cobalt octahedra
*/
void drawAllFourVectors(Unit_Cell &cell, int choice)
{




    GLUquadric* quad =  gluNewQuadric();
    Vec3d center = cell.getCenter() + Vec3d(-10.7, 0, 0);
    Vec3d c_dir = cell.getC().normalized();
    Vec3d a_dir = cell.getA().normalized();

    Vec3d b_dir = crossProduct(a_dir, c_dir);
   //up is the y vector, don't get confused by that.


   // BC2 J = 40  alpha = 7.5
   // Field/4j:  0.481731884058
   // D/4j:  0.546666666667
    Vec3d vec1;
    Vec3d vec2;
    Vec3d vec3;
    Vec3d vec4;

    if(choice == 0)
    {
        vec1 = Vec3d(  0.68069654164    , -0.680696536354     , -0.270747933764);   //Sax
        vec2 = Vec3d(  -0.643281848833    , 0.64328191765     , 0.415182896341);   //Sbx
        vec3 = Vec3d(  -0.53277756849    , 0.532777554965     , 0.657492311327);   //Smx
        vec4 = Vec3d(  0.615499951106    , -0.615499903368     , 0.492259767952);   //Snx

        vec1 = Vec3d(0,0,0);
                vec2 = Vec3d(0,0,0);
                vec3 = Vec3d(0,0,0);
                vec4 = Vec3d(0,0,0);

   //vec1 = Vec3d(  -0.546787588127    , 0.53966644279     , 0.640143315201);  
   //vec2 = Vec3d(  0.601374786458    , -0.633208400489     , -0.487232478149);  
   //vec3 = Vec3d(  0.619572263112    , -0.67825783496     , -0.395090521397);  
   //vec4 = Vec3d(  -0.517820737526    , 0.494581649979     , 0.698033434222);  
    }
    else if(choice == 1)
    {
        vec1 = Vec3d(  -0.998859786228    , -7.89163432932e-08     , 0.0477402079614);   //Sax
        vec2 = Vec3d(  0.998859771986    , 1.6263148685e-06     , 0.0477405061265);   //Sbx
        vec3 = Vec3d(  0.99649736674    , -3.91024679635e-07     , 0.0836241477123);   //Smx
        vec4 = Vec3d(  -0.996497371423    , -2.41826221574e-06     , 0.08362409213);   //Snx


   //vec1 = Vec3d(  0.752074626875    , -0.624452570494     , 0.210814474867);  
   //vec2 = Vec3d(  -0.681344179166    , 0.68135861424     , 0.26743325956);  
   //vec3 = Vec3d(  -0.611723038727    , 0.605364899154     , 0.509242832805);  
   //vec4 = Vec3d(  0.678696659453    , -0.551824313623     , 0.484624361069);  
    }

    else if(choice == 2)
    {
        vec1 = Vec3d(  -0.995319347839    , -0.0396874712245     , 0.0881152679411);   //Sax
        vec2 = Vec3d(  0.995319375568    , -0.0396871312564     , 0.0881151078463);   //Sbx
        vec3 = Vec3d(  0.98595056715    , -0.0695266596633     , 0.151879961592);   //Smx
        vec4 = Vec3d(  -0.985950519435    , -0.0695264604645     , 0.151880362524);   //Snx
    }

    else
    {
        vec1 = Vec3d(  -0.995782996256    , 0.0839924176943     , 0.0368984842337);   //Sax
        vec2 = Vec3d(  0.995783002252    , 0.0839923866672     , 0.0368983930338);   //Sbx
        vec3 = Vec3d(  0.330454049125    , -0.0201036694289     , 0.943607950313);   //Smx
        vec4 = Vec3d(  -0.330453993226    , -0.0201036847825     , 0.943607969561);   //Snx
    }

    vector<Vec3d> iter;
    iter.push_back(vec1);
    iter.push_back(vec2);
    iter.push_back(vec3);
    iter.push_back(vec4);
    for(unsigned int j = 0; j < 4; j++)
    {
        iter[j] = matrixMult(iter[j], a_dir, b_dir, c_dir);
    }

    vector<Vec3d> color;
    color.push_back( Vec3d(1, 0, 0));   //red Sax
    color.push_back( Vec3d(0, 1, 0));   //green Sbx
    color.push_back( Vec3d(0, 0, 1));   //blue Smx
    color.push_back( Vec3d(1, 1, 0));   //yellow Snx

    Vec3d temp;
    double angle;

    for(unsigned int i = 0; i < 4; i++)
    {



        double height = iter[i].norm()*1.2;
        temp = crossProduct(iter[i], Vec3d(0, 0, 1));
        angle = 180*acos(iter[i]*Vec3d(0, 0, 1))/M_PI;
        glColor3f(color[i].x, color[i].y, color[i].z);
        glPushMatrix();
            glLoadIdentity();
            glTranslatef(center.x, center.y, center.z);
            glRotatef(-angle, temp.x, temp.y, temp.z);
            gluCylinder(quad,  RATIO/12.0,  RATIO/12.0,  height,  CYLINDER_RES,  CYLINDER_RES);
        glPopMatrix();

        double measure = 1.0;
        glPushMatrix();
            glLoadIdentity();
            glColor3f(color[i].x*0.6, color[i].y*0.6, color[i].z*0.6);
            glTranslatef(center.x + iter[i].x*measure, center.y + iter[i].y*measure, center.z + iter[i].z*measure);
            glRotatef(-angle, temp.x, temp.y, temp.z);
            gluCylinder(quad,  RATIO/4,  0,  height/2.0,  CYLINDER_RES,  CYLINDER_RES);
        glPopMatrix();
    }

    glColor3f(192/255.0,192/255.0,192/255.0);
    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x, center.y, center.z);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    double extra_dis = 2.4;

    glColor3f(1.0, 0, 0);
    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();


    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
        gluSphere(quad, 0.20, SPHERE_RES, SPHERE_RES);
    glPopMatrix();

    gluDeleteQuadric(quad);

    glBegin(GL_LINES);
        glVertex3f(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
        glVertex3f(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
        glVertex3f(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
        glVertex3f(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
        glVertex3f(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
        glVertex3f(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
        glVertex3f(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
        glVertex3f(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
        glVertex3f(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
        glVertex3f(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
        glVertex3f(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
        glVertex3f(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
        glVertex3f(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
    glEnd();


    glPushAttrib(GL_ENABLE_BIT);
    glLineWidth(1.37);
    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    extra_dis = 4;
    glBegin(GL_LINES);
        glVertex3f(center.x + c_dir.x*extra_dis, center.y + c_dir.y*extra_dis, center.z + c_dir.z*extra_dis);
        glVertex3f(center.x - c_dir.x*extra_dis, center.y - c_dir.y*extra_dis, center.z - c_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x + b_dir.x*extra_dis, center.y + b_dir.y*extra_dis, center.z + b_dir.z*extra_dis);
        glVertex3f(center.x - b_dir.x*extra_dis, center.y - b_dir.y*extra_dis, center.z - b_dir.z*extra_dis);
    glEnd();

    glBegin(GL_LINES);
        glVertex3f(center.x - a_dir.x*extra_dis, center.y - a_dir.y*extra_dis, center.z - a_dir.z*extra_dis);
        glVertex3f(center.x + a_dir.x*extra_dis, center.y + a_dir.y*extra_dis, center.z + a_dir.z*extra_dis);
    glEnd();

    glPopAttrib();


}


/*
 * Draws the directional compass for a, b, c basis vectors that define the unit cell
*/
void  MyGLWidget::drawCompass(Unit_Cell &cell)
{
    if(cell.active == false) return;
    if(cell.geometry.isSet == false) return;
    Vec3d a = (cell.getA()).normalized();
    Vec3d b = (cell.getB()).normalized();
    Vec3d c = (cell.getC()).normalized();

    Vec3d temp;
    double angle;


    GLUquadric* quad =  gluNewQuadric();


    temp = crossProduct(c, Vec3d(0, 0, 1));
    angle = 180*acos(c*Vec3d(0, 0, 1)/c.norm())/M_PI;

   //Values here need to be better adjusted.
    double height = 8;

    unsigned int width_screen = this->size().width();
    unsigned int height_screen = this->size().height();
    Vec3d begin;
    Vec3d end;
    windowToObjectf(((double)width_screen)*0.13, ((double)height_screen)*0.85, begin, end);
    Vec3d initial = Vec3d(end.x/2.0, end.y/2.0, (end.z + begin.z)/2.0);
    double compass_ratio = 2.7;


   //Vec3d initial = Vec3d(-5.9, -10.5, 0);
    unsigned int color1 = 0;
    unsigned int color2 = 1000;
    unsigned int color3 = 1000;
    string first = cell.backup[0].type;
    string second;

    for(unsigned int i = 1; i < cell.backup.size(); i++)
    {
        if(cell.backup[i].type.compare(first) != 0)
        {
            second = cell.backup[i].type;
            color2 = i;
            break;
        }
    }

    if(color2 == 1000)
    {
        color2 = 0;
        color3 = 0;
    }
    else
    {
        for(unsigned int i = 1; i < cell.backup.size(); i++)
        {
            if(cell.backup[i].type.compare(first) != 0 && cell.backup[i].type.compare(second) != 0)
            {
                color3 = i;
                break;
            }

        }
        if(color3 == 1000) color3 = 0;
    }


    glColor3f(cell.backup[color1].paint[0], cell.backup[color1].paint[1], cell.backup[color1].paint[2]);

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x, initial.y, initial.z);
        glRotatef(-angle, temp.x, temp.y, temp.z);
        gluCylinder(quad,  compass_ratio/6.0,  compass_ratio/6.0,  height,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x + height*c.x, initial.y + height*c.y, initial.z + height*c.z);
        draw_text( c.x*height*0.3,  c.y*height*0.3, c.z*height*0.3, QString("c"));
        glColor3f(cell.backup[color1].paint[0], cell.backup[color1].paint[1], cell.backup[color1].paint[2]);
        glRotatef(-angle, temp.x, temp.y, temp.z);
        gluCylinder(quad,  compass_ratio/2.5,  0,  height/4.0,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();




    glColor3f(cell.backup[color2].paint[0], cell.backup[color2].paint[1], cell.backup[color2].paint[2]);
    temp = crossProduct(a, Vec3d(0, 0, 1));
    angle = 180*acos(a*Vec3d(0, 0, 1))/M_PI;


    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x, initial.y, initial.z);
        glRotatef(-angle, temp.x, temp.y, temp.z);
        gluCylinder(quad,  compass_ratio/6.0,  compass_ratio/6.0,  height,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x + a.x*height, initial.y + a.y*height, initial.z + a.z*height);
        draw_text( a.x*height*0.3,  a.y*height*0.3, a.z*height*0.3, QString("a"));
        glColor3f(cell.backup[color2].paint[0], cell.backup[color2].paint[1], cell.backup[color2].paint[2]);
        glRotatef(-angle, temp.x, temp.y, temp.z);
        gluCylinder(quad,  compass_ratio/2.5,  0,  height/4.0,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();



    glColor3f(cell.backup[color3].paint[0], cell.backup[color3].paint[1], cell.backup[color3].paint[2]);
    temp = crossProduct(b, Vec3d(0, 0, 1));
    angle = 180*acos(b*Vec3d(0, 0, 1))/M_PI;

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x, initial.y, initial.z);
        glRotatef(-angle, temp.x, temp.y, temp.z);
        gluCylinder(quad,  compass_ratio/6.0,  compass_ratio/6.0,  height,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x + b.x*height, initial.y + b.y*height, initial.z + b.z*height);
        draw_text( b.x*height*0.3,  b.y*height*0.3, b.z*height*0.3, QString("b"));
        glRotatef(-angle, temp.x, temp.y, temp.z);
        glColor3f(cell.backup[color3].paint[0], cell.backup[color3].paint[1], cell.backup[color3].paint[2]);
        gluCylinder(quad,  compass_ratio/2.5,  0,  height/4.0,  CYLINDER_RES,  CYLINDER_RES);
    glPopMatrix();

    glColor3f(0.752941, 0.752941, 0.752941);

    glPushMatrix();
        glLoadIdentity();
        glTranslatef(initial.x, initial.y, initial.z);
        gluSphere(quad, compass_ratio/2.5, SPHERE_RES, SPHERE_RES);
    glPopMatrix();
}





#endif   // MYGLWIDGET

