#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <vector>
#include <QOpenGLWidget>
#include "mytextbrowser.h"
#include <string>
#include "mainwindow.h"
#include <sstream>
#include "header.h"
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include "lineedit.h"
#include "math.h"
#include <algorithm>




/*
 * This file manages all keypresses in the openGL window
 */


void windowToObjectf(int mouseX, int mouseY, Vec3d &begin, Vec3d &end);
Vec3d ClosestPoint(const Vec3d Begin, const Vec3d End, const Vec3d Point);
int hitAtom(Unit_Cell &cell, int mouseX, int mouseY);
double findAngle(Atom &one, Atom &two, Atom &three);
bool lineCollision(Vec3d zero, Vec3d vector, Vec3d point, Vec3d cross, Vec3d &result);
bool find_Noncollinear_Points(Unit_Cell &cell, int *result, vector<int> &clicked_atom);
void setPlane(Unit_Cell &cell, int *input);
void getRange(Unit_Cell &cell, double *range);
void deletePlane(Unit_Cell &cell);
void upPlane(Unit_Cell &cell);
void downPlane(Unit_Cell &cell);
void togglePlane(Unit_Cell &cell);
void makePlane(Unit_Cell &cell, int *answer, vector<Vec3d> &edge);
bool interPoint(Vec3d v1, Vec3d v2, Vec3d position, Vec3d cross, vector<Vec3d> &initial);
int compare_x (const void * a, const void * b);
int compare_z (const void * a, const void * b);
bool deleteAbove(Unit_Cell &cell, double control, Manager &manage);
bool deleteBelow(Unit_Cell &cell, double control, Manager &manage);

bool moveAtom(Unit_Cell &cell, Vec3d direction, Manager &manage);
void stretchCell(Unit_Cell &cell);
void toggleCell(vector<Unit_Cell> &cell, Manager &manage);

void cloneCell(vector<Unit_Cell> &cell, Manager &manage);
void deleteAtom(vector<Unit_Cell> &cell_array, Manager &manage);
void deleteCell(vector<Unit_Cell> &cell, Manager &manage);
void unSelect(Unit_Cell &cell, Manager &manage);

void mergePlanes(vector<Unit_Cell> &cell, int one, int two);
bool save_screenshot(string filename, unsigned int w, unsigned int h);
void bringTogether(vector<Unit_Cell> &cell, Manager &manage);
void rotateOctahedra(Unit_Cell &cell, Vec3d axis, double theta, Manager &manage);
bool moveOctahedra(Unit_Cell &cell, const Vec3d direction, Manager &manage);
void unDelete(Unit_Cell &cell);
void setVectors(Unit_Cell &cell, Vec3d &x_direct, Vec3d &y_direct, Vec3d &cross);
void undo(vector<Unit_Cell> &cell, Manager &manage, Stack &stack);
Vec3d objectToWindow(Vec3d object);
void manageOctahedra(vector<Unit_Cell> &cell, double theta, Manager &manage);
void colorOctahedra(vector<Unit_Cell> &cell_array, double addition);



/*
 * This method manages all keypresses in the openGL window, called everytime
 * a key in MyGlWidget widget window is pressed
 */
void opengl_keypress(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up)
        cout << (int)event->text().toStdString()[0] << endl;
   //cout << event->text().toStdString()[0] << endl;

    string current_key = event->text().toStdString();
    Singleton& the_s = Singleton::getInstance();

    vector<Unit_Cell>& cell_array = the_s.cell;
    vector<string>& all_keys = the_s.all_keys;
    Manager& manage = the_s.manage;
    Stack& stack = the_s.stack;

    string previous_key;
    if(all_keys.size() < 1)
        previous_key = "null";
    else
        previous_key = all_keys[all_keys.size() - 1];
    all_keys.push_back(current_key);


    if (cell_array.size() < 1 && current_key[0] !=  92 )
        return;
    else if(cell_array.size() < 1 && current_key[0] ==  92)
    {
        undo(cell_array, manage, stack);
        return;
    }


    int selected_cell = manage.getSelected();
    Unit_Cell *cell = NULL;
    cell = &the_s.cell[the_s.manage.getSelected()];

    double angle = 0.3;
    double move_abc = 1;
    double move_xyz = 0.03;
    Vec3d a = cell->getA();
    Vec3d b = cell->getB();
    Vec3d c = cell->getC();

    Vec3d x_direct;
    Vec3d y_direct;
    Vec3d cross;
    setVectors(cell_array[selected_cell], x_direct, y_direct, cross);

    if (current_key == "w")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        for(unsigned int i = 0; i < cell_array.size(); i++)
        {
            cell_array[i].translate(0,0, 0.1);
        }
    }

    else if (current_key == "q")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        for(unsigned int i = 0; i < cell_array.size(); i++)
        {
            cell_array[i].translate(0,0, -0.1);
        }
    }

    else if (current_key == "a")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(a.x*move_abc, a.y*move_abc, a.z*move_abc);
    }

    else if (current_key == "b")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(b.x*move_abc, b.y*move_abc, b.z*move_abc);
    }

    else if (current_key == "c")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(c.x*move_abc, c.y*move_abc, c.z*move_abc);
    }

    else if (current_key == "A")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(-a.x*move_abc, -a.y*move_abc, -a.z*move_abc);
    }

    else if (current_key == "B")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(-b.x*move_abc, -b.y*move_abc, -b.z*move_abc);
    }

    else if (current_key == "C")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(-c.x*move_abc, -c.y*move_abc, -c.z*move_abc);
    }

    else if( current_key == "x")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(move_xyz, 0, 0);
    }

    else if( current_key == "y")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(0, move_xyz, 0);
    }

    else if( current_key == "z")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(0, 0, move_xyz);
    }

    else if( current_key == "X")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(-move_xyz, 0, 0);
    }

    else if( current_key == "Y")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(0, -move_xyz, 0);
    }

    else if( current_key == "Z")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
         cell->translate(0, 0, -move_xyz);
    }

    else if(current_key == "j")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == true)
            cell->translate(move_xyz*x_direct.x, move_xyz*x_direct.y, move_xyz*x_direct.z);
        else manageOctahedra(cell_array, angle, manage);
    }

    else if(current_key == "k")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == true)
            cell->translate(-move_xyz*x_direct.x, -move_xyz*x_direct.y, -move_xyz*x_direct.z);

        else manageOctahedra(cell_array, -angle, manage);
    }

    else if(current_key == "l")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        cell->translate(move_xyz*y_direct.x, move_xyz*y_direct.y, move_xyz*y_direct.z);
    }

    else if(current_key == ";")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        cell->translate(-move_xyz*y_direct.x, -move_xyz*y_direct.y, -move_xyz*y_direct.z);

    }

    else if (current_key == "n")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(move_xyz*cross.x, move_xyz*cross.y, move_xyz*cross.z);

    }

    else if(current_key == "m")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
        cell->translate(-move_xyz*cross.x, -move_xyz*cross.y, -move_xyz*cross.z);
    }

    else if(current_key == "e")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, x_direct*move_xyz*0.1, manage);
        else
            moveAtom(*cell, x_direct*move_xyz*0.1, manage);
    }

    else if(current_key == "r")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, x_direct*move_xyz*(-0.1), manage);
        else
            moveAtom(*cell, x_direct*move_xyz*(-0.1), manage);

    }

    else if(current_key == "g")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, y_direct*move_xyz*(0.1), manage);
        else
            moveAtom(*cell, y_direct*move_xyz*(0.1), manage);


    }

    else if(current_key == "h")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, y_direct*move_xyz*(-0.1), manage);
        else
            moveAtom(*cell, y_direct*move_xyz*(-0.1), manage);

    }

    else if(current_key == ",")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, cross*move_xyz*(0.1), manage);
        else
            moveAtom(*cell, cross*move_xyz*(0.1), manage);
    }

    else if(current_key == ".")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(manage.toggle_question == false)
            moveOctahedra(*cell, cross*move_xyz*(-0.1), manage);
        else
            moveAtom(*cell, cross*move_xyz*(-0.1), manage);

    }

    else if(current_key == "1")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));


        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, x_direct, angle, manage);
        else cell->rotate_all(angle, x_direct);
    }

    else if(current_key == "2")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, x_direct, -angle, manage);
        else cell->rotate_all(-angle, x_direct);
    }

    else if(current_key == "3")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, y_direct, angle, manage);
        else cell->rotate_all(angle, y_direct);

    }

    else if(current_key == "4")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, y_direct, -angle, manage);
        else cell->rotate_all(-angle, y_direct);
    }

    else if(current_key == "5")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, cross, -angle, manage);
        else cell->rotate_all(angle, cross);
    }

    else if(current_key == "6")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        if(the_s.manage.toggle_question == false)  rotateOctahedra(*cell, cross, -angle, manage);
        else cell->rotate_all(-angle, cross);

    }


    else if(current_key == "!")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        rotateOctahedra(*cell, cell->getC(), angle, manage);
    }

    else if(current_key == "@")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        rotateOctahedra(*cell, cell->getC(), -angle, manage);
    }

    else if(current_key == "#")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        rotateOctahedra(*cell, cross, angle, manage);
    }

    else if(current_key == "$")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

        rotateOctahedra(*cell, cross, -angle, manage);
    }

    else if(current_key == "?")
    {
        stack.push(Container(cell_array, manage));\
        if(the_s.manage.toggle_question == true) the_s.manage.toggle_question = false;
        else the_s.manage.toggle_question = true;
    }

    else if(current_key == "f")
    {
        stack.push(Container(cell_array, manage));\
        if(cell->geometry.toggle_f == true) cell->geometry.toggle_f = false;
        else cell->geometry.toggle_f = true;
    }

    else if(current_key == "p")
    {
        stack.push(Container(cell_array, manage));
        createPlane(cell_array, the_s.manage);
    }

    else if(current_key == "[")
    {
        stack.push(Container(cell_array, manage));
        upPlane(*cell);
    }

    else if(current_key == "]")
    {
        stack.push(Container(cell_array, manage));
        downPlane(*cell);
    }

    else if(current_key == "(")
    {
        stack.push(Container(cell_array, manage));
        double control = 0.01;
        deleteAbove(*cell, control, the_s.manage);
    }

    else if(current_key == ")")
    {
        stack.push(Container(cell_array, manage));
        double control = 0.01;
        deleteBelow(*cell, control, the_s.manage);
    }

    else if(current_key == "t")
    {
        stack.push(Container(cell_array, manage));
        togglePlane(*cell);
    }

    else if (current_key == "\t")
    {
        stack.push(Container(cell_array, manage));
        toggleCell(the_s.cell, the_s.manage);
    }

    else if(current_key == "d")
    {
        stack.push(Container(cell_array, manage));
        deletePlane(*cell);
    }

    else if(current_key == "8")
    {
        stack.push(Container(cell_array, manage));
        cloneCell(cell_array, manage);

    }

   //delete key, deletes selected atoms
    else if(current_key[0] == 127)
    {
        stack.push(Container(cell_array, manage));
        deleteAtom(cell_array, manage);
    }

   //here 92 is character '\'
    else if(current_key[0] == 92)
    {
        undo(the_s.cell, the_s.manage, the_s.stack);
    }

    else if(current_key == "U")
    {
        stack.push(Container(cell_array, manage));
        unDelete(*cell);
    }

    else if(current_key == "u")
    {
        stack.push(Container(cell_array, manage));
        unSelect(*cell, the_s.manage);
    }

    else if(current_key == "D")
    {
        stack.push(Container(cell_array, manage));
        deleteCell(the_s.cell, the_s.manage);
    }

    else if(current_key == "+")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));
   //for the octahedra, makes it lighter
        double addition = 0.02;
        colorOctahedra(cell_array, addition);
    }

    else if(current_key == "_")
    {
        if(previous_key != current_key)
            stack.push(Container(cell_array, manage));

   //for the octahedra, makes it darker
        double addition = -0.02;
        colorOctahedra(cell_array, addition);
    }

    else if(current_key == "/")
    {
   //backup is created
        stack.push(Container(cell_array, manage));

   //two unit cells are pushed together along their selected planes
        mergePlanes(the_s.cell, 0, 1);
    }

    else if(current_key == "=")
    {
        stack.push(Container(cell_array, manage));

        if(cell->geometry.polygons == true) cell->geometry.polygons = false;
        else cell->geometry.polygons = true;
    }

    else if(current_key == "-")
    {
        stack.push(Container(cell_array, manage));
        bringTogether(cell_array, manage);
    }
    else
        return;
}


void colorOctahedra(vector<Unit_Cell> &cell_array, double addition)
{
    for(unsigned int i = 0; i < cell_array.size(); i++)
        cell_array[i].geometry.polygon_shade = cell_array[i].geometry.polygon_shade + addition;
    cout << cell_array[0].geometry.polygon_shade << endl;
}

Vec3d objectToWindow(Vec3d object)
{
    double  modelViewMatrix[16], projectionMatrix[16];
    int viewport[4];
    glGetDoublev( GL_MODELVIEW_MATRIX, modelViewMatrix );
    glGetDoublev( GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv( GL_VIEWPORT, viewport );
    Vec3d answer;

    gluProject(object.x, object.y, object.z, modelViewMatrix, projectionMatrix, viewport, &answer.x, &answer.y, &answer.z);
    answer.y = answer.y - viewport[3];
    return answer;
}


double findAngle(Atom &one, Atom &two, Atom &three)
{
    Vec3d A = Vec3d(two.x - one.x, two.y - one.y, two.z - one.z);
    Vec3d B = Vec3d(two.x - three.x, two.y - three.y, two.z - three.z);
    double number = acos( (A * B)/(A.norm() * B.norm()))*180.0/M_PI;
    if (isnan(number) == true)
        return 180.0;
    return acos( (A * B)/(A.norm() * B.norm()))*180.0/M_PI;
}


bool find_Noncollinear_Points(Unit_Cell &cell, int * result, vector<int> &clicked_atom)
{

    int one;
    int two;
    int three;
    double angle;
    if(clicked_atom.size() < 3)
    {
        cout << "Not enough coordinates specified for a plane, less then three atoms clicked" << '\n';
        return false;
    }


    for (unsigned int i = 0; i < clicked_atom.size() - 2; i++)
    {
        for(unsigned int j = i + 1; j < clicked_atom.size() - 1; j++)
        {
            for(unsigned int k = j + 1; k < clicked_atom.size(); k++)
            {

                one = clicked_atom[i];
                two = clicked_atom[j];
                three = clicked_atom[k];
                angle = findAngle(cell.backup[one], cell.backup[two], cell.backup[three]);
                if(fabs(angle) > 1.0 && fabs(angle - 180.0) < 179.0)
                {
                        result[0] = one;
                        result[1] = two;
                        result[2] = three;
                        return true;
                }
            }
        }
    }

    cout << "Three noncollinear points do not exist" << '\n';
    return false;
}




void deletePlane(Unit_Cell &cell)
{
        vector<Plane> hold;
        Plane temp;
        unsigned int index;
        for(unsigned int i = 0; i < cell.plane.size(); i++)
        {
            if(cell.plane[i].selected == false)
            {
                temp = cell.plane[i].copy();
                hold.push_back(temp);
            }

            else
            {
                if(i == cell.plane.size() - 1)
                {
                    index = 0;
                }
                else index = i;
            }
        }

        cell.plane.resize(hold.size());
        for(unsigned int i = 0; i < hold.size(); i++)
        {
            if (i == index)
            {
                hold[i].selected = true;
            }

            cell.plane[i] = hold[i];
        }
}

bool compareFunction(Vec3d one, Vec3d two)
{
    if (one.norm() - two.norm()) return false;
    else return true;
}




void upPlane(Unit_Cell &cell)
{
    int i = cell.getPlane();
    if(cell.geometry.isSet == false)
        return;

    Vec3d first_cross;
    vector<Vec3d> tosort;
    if(i < 0)
    {
   //first_cross = cell.getC().normalized();
   //tosort.push_back(cell.getA().normalized());
        cout << "This Unit_Cell does not have any planes" << endl;
        return;
    }

    else
    {

        tosort.resize(cell.plane[i].edge.size() - 1);
        for(unsigned int j = 0; j < cell.plane[i].edge.size() - 1; j++)
            tosort[j] = cell.plane[i].edge[j] - cell.plane[i].edge[j + 1];
        sort(tosort.begin(), tosort.end(), compareFunction);

        double max = 0;
        int position;
        for(unsigned int j = 0; j < tosort.size() - 1; j++)
        {
            double cross = (tosort[j].cross(tosort[j + 1])).norm();
            if(cross > max)
            {
                  max = cross;
                position = j;
            }
        }
        first_cross = (tosort[position].cross(tosort[position + 1])).normalized();
    }
   //Vec3d first_cross = crossProduct(two - one, three - two).normalized();
    Vec3d up_vector = Vec3d(0, 1, 0);

    if ( up_vector*first_cross > 0.9999)
    {
        return;
    }

    else if (up_vector*first_cross < -0.9999)
    {

         Vec3d second_cross = tosort[0];
         cell.rotate_all(-180.0, second_cross);
         return;
    }

    else
    {
        double theta = acos(up_vector*first_cross)*180/M_PI;
         Vec3d second_cross = crossProduct(up_vector, first_cross).normalized();
         cell.rotate_all(-theta, second_cross);
         return;
    }
}

void downPlane(Unit_Cell &cell)
{
    int i = cell.getPlane();
    if(cell.geometry.isSet == false)
        return;

    Vec3d first_cross;
    vector<Vec3d> tosort;
    if(i < 0)
    {
   //first_cross = cell.getC().normalized();
   //tosort.push_back(cell.getA().normalized());
        cout << "This Unit_Cell does not have any planes" << endl;
        return;
    }


    else
    {
          tosort.resize(cell.plane[i].edge.size() - 1);

          for(unsigned int j = 0; j < cell.plane[i].edge.size() - 1; j++)
               tosort[j] = cell.plane[i].edge[j] - cell.plane[i].edge[j + 1];
          sort(tosort.begin(), tosort.end(), compareFunction);

          double max = 0;
          int position;
          for(unsigned int j = 0; j < tosort.size() - 1; j++)
          {
               double cross = (tosort[j].cross(tosort[j + 1])).norm();
               if(cross > max)
               {
                    max = cross;
                    position = j;
               }
           }
           first_cross = (tosort[position].cross(tosort[position + 1])).normalized();
    }

        Vec3d up_vector = Vec3d(0, 1, 0);

                if ( up_vector*first_cross > 0.9999)
                {
                    Vec3d second_cross = tosort[0];
                    cell.rotate_all(-180.0, second_cross);
                    return;
                }

                else if (up_vector*first_cross < -0.9999)
                {
                    return;
                }

                else
                {
                    double theta = acos(up_vector*first_cross)*180/M_PI;
                    Vec3d second_cross = crossProduct(up_vector, first_cross).normalized();
                    cell.rotate_all(180.0 - theta, second_cross);
                    return;
                }
}

/**
 * @brief togglePlane switches active plane in a given Unit_Cell
 * @param cell the given Unit_Cell.
 */
void togglePlane(Unit_Cell &cell)
{
    if(cell.plane.size() == 1) return;

    if(cell.plane.size() == 0) return;

        for(unsigned int i = 0; i < cell.plane.size(); i++)
        {
            if(cell.plane[i].selected == true)
            {
                if(i < cell.plane.size() - 1)
                {
                    cell.plane[i].selected = false;
                    cell.plane[i+1].selected = true;
                    return;
                }
                else
                {
                    cell.plane[i].selected = false;
                    cell.plane[0].selected = true;
                    return;
                }
            }
        }
        cell.plane[0].selected = true;
        cell.plane[1].selected = false;
}

/*
 * Input is the unit cell,
 * an array answer which contain coordinates of selected atoms to define a plane
 * output is a collection of Vec3d objects to define the edges of the plane
*/
void makePlane(Unit_Cell &cell, int *answer, vector<Vec3d> &edge)
{
    Vec3d one;   //only three noncollinear atoms are necessary to define a plane
    Vec3d two;   //second atom
    Vec3d position;   //third atom
    Vec3d cross;
    Vec3d zero = cell.getZero();
    Vec3d a = cell.getA();
    Vec3d b = cell.getB();
    Vec3d c = cell.getC();


    vector<Vec3d> initial;

   //two vectors that define the plane, answer[0] and answer[1], answer[2] contain the three atoms that define a plane
        one = Vec3d(cell.backup[answer[0]].x - cell.backup[answer[1]].x,
                cell.backup[answer[0]].y - cell.backup[answer[1]].y,
                cell.backup[answer[0]].z - cell.backup[answer[1]].z);

        two = Vec3d(cell.backup[answer[2]].x - cell.backup[answer[1]].x,
                cell.backup[answer[2]].y - cell.backup[answer[1]].y,
                cell.backup[answer[2]].z - cell.backup[answer[1]].z);

   //a point on the plane.
        position = Vec3d(cell.backup[answer[1]].x, cell.backup[answer[1]].y, cell.backup[answer[1]].z);

   //vector normal to the plane
        cross = crossProduct(one, two).normalized();


   //edge positions are calculated.
        interPoint(zero, zero + a, position, cross, initial);
        interPoint(zero, zero + b, position, cross, initial);
        interPoint(zero + a, zero + b + a, position, cross, initial);
        interPoint(zero + b, zero + a + b, position, cross, initial);

        interPoint(zero, zero + c, position, cross, initial);
        interPoint(zero + a, zero + a + c, position, cross, initial);
        interPoint(zero + b, zero + b + c, position, cross, initial);
        interPoint(zero + a + b, zero + a + b + c, position, cross, initial);

        interPoint(zero + c, zero + a + c, position, cross, initial);
        interPoint(zero + c, zero + c + b, position, cross, initial);
        interPoint(zero + c + a, zero + b + a + c, position, cross, initial);
        interPoint(zero + c + b, zero + a + b + c, position, cross, initial);

    Vec3d store = Vec3d(0, 0, 0);


    for(unsigned int i = 0; i < initial.size(); i++)
    {
        store = store + initial[i];
    }
    store = store/initial.size();
    vector<Vec3d> another;

     for(unsigned int i = 0; i < initial.size(); i++)
     {
         bool greater = true;
         for(unsigned int j = 0; j < i; j++)
         {
             if((initial[i] - initial[j]).norm() < a.norm()/100.0)
             {
                greater = false;
             }
         }

         if(greater == true)
             another.push_back(initial[i]);
     }


    insertionSort(another, store, cross);

    edge = another;
}

/*
 * Method to calculate the edge positions of the plane
 * v1, v2 are input vectors define edges of the unit cell, cross is the normal vector to the plane
 *  initial stores the results.
*/
bool interPoint(Vec3d v1, Vec3d v2, Vec3d position, Vec3d cross, vector<Vec3d> &initial)
{

        Vec3d ray = v2 - v1;
        if (fabs(ray*cross) == 0.0)
        {
            return false;
        }

        double t = -( cross*(v1 - position))/(cross*(ray));

        if(t >= -0.001 && t <= 1.001)
        {
            initial.push_back(v1 + t*(ray));
            return true;
        }

        return false;
}




bool deleteAbove(Unit_Cell &cell, double control, Manager &manage)
{
    int index;
    Vec3d center = Vec3d(0, 0, 0);
    Vec3d cross;
    Vec3d temp;


    if(cell.plane.size() < 1) return false;

    for(unsigned int i = 0;  i < cell.plane.size(); i++)
    {
        if(cell.plane[i].selected == true)
        {
            index = i;
            break;
        }
    }

    for(unsigned int i = 0; i < cell.plane[index].edge.size(); i++)
    {
        center = center + cell.plane[index].edge[i];
    }

    center = center/cell.plane[index].edge.size();
    cross = crossProduct(cell.plane[index].edge[0] - center, cell.plane[index].edge[1] - center).normalized();

    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {
        temp = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z) - center;
        if(temp*cross > control)
        {

            if(cell.geometry.toggle == true)
            {
                cell.backup[i].visible = true;
                continue;
            }

            cell.backup[i].visible = false;
            for(unsigned int j = 0; j < clicked_atoms.size(); j++)
            {
                if((unsigned)clicked_atoms[j] == i)
                    manage.removeSelected(i);
            }


            if(cell.backup[i].clicked == true)
            {
   //cell.removeNumber(i);
                cell.backup[i].clicked = false;
            }
        }
    }

    if(cell.geometry.toggle == true) cell.geometry.toggle = false;
    else cell.geometry.toggle = true;

    return true;
}


/**
 * @brief deleteBelow delete all atoms below the active plane in an active Unit Cell
 * @param cell input Unit_Cell
 * @param control a small float value to make sure atoms on the active plane are not deleted
 * @param manage deleting an atom unselects it, thus Manager is necessary here
 *               to unselect all deleted atoms.
 * @return true if the method worked, false otherwise.
 */
bool deleteBelow(Unit_Cell &cell, double control, Manager &manage)
{
    int index;
    Vec3d center = Vec3d(0, 0, 0);
    Vec3d cross;
    Vec3d temp;
    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    if(cell.plane.size() < 1) return false;

    for(unsigned int i = 0;  i < cell.plane.size(); i++)
    {
        if(cell.plane[i].selected == true)
        {
            index = i;
            break;
        }
    }

    for(unsigned int i = 0; i < cell.plane[index].edge.size(); i++)
    {
        center = center + cell.plane[index].edge[i];
    }

    center = center/cell.plane[index].edge.size();
    cross = crossProduct(cell.plane[index].edge[0] - center, cell.plane[index].edge[1] - center).normalized();
    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {
        temp = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z) - center;
        if(temp*cross < control)
        {
            if(cell.geometry.toggle == true)
            {
                cell.backup[i].visible = true;
                continue;
            }

            for(unsigned int j = 0; j < clicked_atoms.size(); j++)
            {
                if((unsigned)clicked_atoms[j] == i)
                    manage.removeSelected(i);
            }

            cell.backup[i].visible = false;
            if(cell.backup[i].clicked == true)
            {
   //cell.removeNumber(i);
                manage.removeSelected(i);
                cell.backup[i].clicked = false;
            }
        }

    }

    if(cell.geometry.toggle == true) cell.geometry.toggle = false;
    else cell.geometry.toggle = true;
    return true;
}





bool moveAtom(Unit_Cell &cell, Vec3d direction, Manager &manage)
{

    if(manage.toggle_question == false)
    {
        return moveOctahedra(cell, direction, manage);
    }

    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    if(clicked_atoms.size() > 1)
    {
        cout << "Danger, you should only move one atom at a time, number selected: " << clicked_atoms.size() << endl;
        return false;
    }

    else if(clicked_atoms.size() < 1)
    {
        cout << "No atoms are selected in the active unit cell" << endl;
        return false;

    }
    else
    {
        int atom = clicked_atoms[0];
        cell.backup[atom].x = cell.backup[atom].x + direction.x;
        cell.backup[atom].y = cell.backup[atom].y + direction.y;
        cell.backup[atom].z = cell.backup[atom].z + direction.z;
        return true;
    }
}



void createPlane(vector<Unit_Cell> &cell, Manager &manage)
{
        vector<int> clicked_atoms = manage.getAtomsSelectedCell();
        int answer[3];
        int selected_cell = manage.getSelected();
        bool found_plane = find_Noncollinear_Points(cell[selected_cell], answer, clicked_atoms);

        if (found_plane == false) return;

        Plane hold;

        makePlane(cell[selected_cell], answer, hold.edge);

        /*
        for(unsigned int j = 0; j < 3; j++)
        {
            place.push_back(answer[j]);
        }
*/
        hold.paint[0] = 0;
        hold.paint[1] = 0;
        hold.paint[2] = 0;

        cell[selected_cell].plane.push_back(hold);

        for(unsigned int i = 0; i < cell[selected_cell].plane.size() - 1; i++)
        {
            cell[selected_cell].plane[i].selected = false;
        }
        cell[selected_cell].plane[cell[selected_cell].plane.size() - 1].selected = true;
}




void toggleCell(vector<Unit_Cell> &cell, Manager &manage)
{
        for( unsigned int i = 0; i < cell.size(); i++)
        {
            if(cell[i].active == true)
            {
                cell[i].active = false;
                if(i == cell.size() - 1)
                {
                    cell[0].active = true;
                    manage.Select(0);
                    break;
                }
                else
                {
                    cell[i+1].active = true;
                    manage.Select(i+1);
                    break;
                }

            }
        }
}




void cloneCell(vector<Unit_Cell> &cell, Manager &manage)
{
        Unit_Cell temp;
        int selected = manage.getSelected();
        temp = cell[selected].copy();

        temp.active = true;
        cell[selected].active = false;
        cell.push_back(temp);
        manage.Select(cell.size() - 1);
        cout << "Cell has just been cloned. Number of cells now: " << cell.size() << endl;
}

void deleteAtom(vector<Unit_Cell> &cell_array, Manager &manage)
{
    Unit_Cell& cell = cell_array[manage.getSelected()];
    vector<int> clicked_array = manage.getAtomsSelectedCell();
    for(unsigned int i = 0; i < clicked_array.size(); i++)
    {
            cell.backup[clicked_array[i]].clicked = false;
            cell.backup[clicked_array[i]].visible = false;
    }
    manage.removeAll(manage.getAtomsSelectedCell());
}

void deleteCell(vector<Unit_Cell> &cell, Manager &manage)
{
        vector<Unit_Cell> temp;
        manage.removeAll(manage.getAtomsSelectedCell());
        for(unsigned int i = 0; i < cell.size(); i++)
        {
            Unit_Cell copy;
            if((unsigned)manage.getSelected() != i)
            {
                copy = cell[i].copy();
                temp.push_back(copy);
            }

        }
        cell = temp;
        manage.Select(cell.size() - 1);
        cell[manage.getSelected()].active = true;
}

/**
 * @brief unSelect all the atoms are unselected in a given Unit_Cell
 * @param cell the input Unit_Cell
 * @param manage contains the array of all selected atoms
 */
void unSelect(Unit_Cell &cell, Manager &manage)
{
        for(unsigned int i = 0; i < cell.backup.size(); i++)
        {
            if(cell.backup[i].clicked == true)
            {
                cell.backup[i].clicked = false;
            }
        }
        manage.removeAll(manage.getAtomsSelectedCell());
        cell.clicked_atom.resize(0);
}



void mergePlanes(vector<Unit_Cell> &cell, int one, int two)
{
    if(cell.size() < 2)
    {
        cout << "Command / cannot be executed. Planes cannot be merged as there are less then two cells present" << endl;
        return;
    }

    int first = cell[one].getPlane();
    int second = cell[two].getPlane();

    if(first < 0)
    {
        cout << "Cell " << one << " has no planes" << endl;
        cout << "Please create a plane in cell " << one << endl;
        return;
    }

    if(second < 0)
    {
        cout << "Cell " << two << " has no planes" << endl;
        cout << "Please create a plane in cell " << two << endl;
        return;
    }


    Plane plane1 = cell[one].plane[cell[one].getPlane()];
    Plane plane2 = cell[two].plane[cell[two].getPlane()];
    Vec3d center1;
    Vec3d center2;

    for(unsigned int j = 0; j < plane1.edge.size(); j++)
    {
        center1 = center1 + plane1.edge[j];
    }
    center1 = center1/plane1.edge.size();

    for(unsigned int j = 0; j < plane2.edge.size(); j++)
    {
        center2 = center2 + plane2.edge[j];
    }
    center2 = center2/plane2.edge.size();

    Vec3d move = center2 - center1;
    cell[one].translate(move.x, move.y, move.z);

}



void bringTogether(vector<Unit_Cell> &cell, Manager &manage)
{
        int i;
        int j;
        int atom1;
        int atom2;

        Vec3d first;
        Vec3d second;
        Vec3d length;

        if(manage.getSize() == 2)
        {
            i = manage.getAtom(0).cell;
            atom1 = manage.getAtom(0).clicked;
            first = Vec3d(cell[i].backup[atom1].x, cell[i].backup[atom1].y, cell[i].backup[atom1].z);

            j = manage.getAtom(1).cell;
            atom2 = manage.getAtom(1).clicked;
            second = Vec3d(cell[j].backup[atom2].x, cell[j].backup[atom2].y, cell[j].backup[atom2].z);


            length = (first - second)/2.0;
            cell[i].backup[atom1].x = cell[i].backup[atom1].x - length.x;
            cell[i].backup[atom1].y = cell[i].backup[atom1].y - length.y;
            cell[i].backup[atom1].z = cell[i].backup[atom1].z - length.z;

            cell[j].backup[atom2].x = cell[j].backup[atom2].x + length.x;
            cell[j].backup[atom2].y = cell[j].backup[atom2].y + length.y;
            cell[j].backup[atom2].z = cell[j].backup[atom2].z + length.z;
        }
        else
        {
            cout << "This command works for only two selected atoms";
            cout << ", you have: " << manage.getSize() << " atoms selected." << endl;
        }


}
void manageOctahedra(vector<Unit_Cell> &cell, double theta, Manager &manage)
{
    if(manage.getSize() == 0)
    {

        cout << "No atoms are selected, please select a single cobalt atom" << endl;
        return;
    }

    if(manage.getSize() > 2)
    {
        cout << "More then two atoms are selected, please unselect an atom" << endl;
        return;
    }

    Selected_atom tmp1 = manage.getAtom(0);
    Atom atom = cell[tmp1.cell].backup[tmp1.clicked];
    Vec3d one = Vec3d(atom.x, atom.y, atom.z);

    Selected_atom tmp2 = manage.getAtom(1);
    Atom atom2 = cell[tmp2.cell].backup[tmp2.clicked];
    Vec3d two = Vec3d(atom2.x, atom2.y, atom2.z);
    Vec3d axis = one - two;
    Vec3d cobalt;
    int cobalt_cell;
    int cobalt_atom;


    if(atom.type.compare("Co") == 0)
    {
        cobalt = one;
        cobalt_cell = tmp1.cell;
        cobalt_atom = tmp1.clicked;
    }
    else if(atom2.type.compare("Co") == 0)
    {
        cobalt = two;
        cobalt_cell = tmp2.cell;
        cobalt_atom = tmp2.clicked;
    }
    else
    {
        cout << "Problem, no cobalt atom is located among the selected atoms" << endl;
        return;
    }

    Vec3d oxygen;
    Vec3d new_oxygen;
    int temp;

    for(unsigned int i = 0; i < cell[cobalt_cell].backup[cobalt_atom].bonds.size(); i++)
    {
        temp = cell[cobalt_cell].backup[cobalt_atom].bonds[i];
        oxygen = Vec3d(cell[cobalt_cell].backup[temp].x, cell[cobalt_cell].backup[temp].y, cell[cobalt_cell].backup[temp].z);
        new_oxygen = ArbitraryRotate(oxygen - cobalt, theta, axis);
        cell[cobalt_cell].backup[temp].x = cobalt.x + new_oxygen.x;
        cell[cobalt_cell].backup[temp].y = cobalt.y + new_oxygen.y;
        cell[cobalt_cell].backup[temp].z = cobalt.z + new_oxygen.z;
    }



}


void rotateOctahedra(Unit_Cell &cell, Vec3d axis, double theta, Manager &manage)
{
    Vec3d oxygen;
    Vec3d cobalt;
    Vec3d new_oxygen;
    int temp;
    int cobalt_temp;
    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    if(clicked_atoms.size() == 0)
    {
        cout << "No atoms are selected for the active Unit Cell, please select a single cobalt atom" << endl;
        return;
    }

    if(clicked_atoms.size() > 1)
    {
        cout << "Number of atoms selected = " << clicked_atoms.size() << ". Please select a single cobalt atom" << endl;
        return;
    }

    if(cell.backup[clicked_atoms[0]].type.compare("Co") != 0)
    {
        cout << "You have selected " << cell.backup[clicked_atoms[0]].type << endl;
        cout << "Please select some Co atom." << endl;
        return;
    }

    cobalt_temp = clicked_atoms[0];
    cobalt = Vec3d(cell.backup[cobalt_temp].x, cell.backup[cobalt_temp].y, cell.backup[cobalt_temp].z);

    for(unsigned int i = 0; i < cell.backup[cobalt_temp].bonds.size(); i++)
    {
        temp = cell.backup[cobalt_temp].bonds[i];
        oxygen = Vec3d(cell.backup[temp].x, cell.backup[temp].y, cell.backup[temp].z);
        new_oxygen = ArbitraryRotate(oxygen - cobalt, theta, axis);
        cell.backup[temp].x = cobalt.x + new_oxygen.x;
        cell.backup[temp].y = cobalt.y + new_oxygen.y;
        cell.backup[temp].z = cobalt.z + new_oxygen.z;
    }

}



bool moveOctahedra(Unit_Cell &cell, const Vec3d direction, Manager &manage)
{
    int cobalt_temp;
    int oxygen_temp;
    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    if(clicked_atoms.size() == 0)
    {
        cout << "You are in the octahedra mode. To exit it, click '?'. " << endl;
        cout << "No atoms are selected for the active Unit Cell, please select a single cobalt atom" << endl;
        return false;
    }

    if(clicked_atoms.size() > 1)
    {

        cout << "You are in the octahedra mode. To exit it, click '?'. " << endl;
        cout << "Number of atoms selected = " << clicked_atoms.size() << ". Please select a single cobalt atom" << endl;
        return false;
    }

    if(cell.backup[clicked_atoms[0]].type.compare("Co") != 0)
    {

        cout << "You are in the octahedra mode. To exit it, click '?'. " << endl;
        cout << "You have selected " << cell.backup[clicked_atoms[0]].type << endl;
        cout << "Please select some Co atom." << endl;
        return false;
    }

    cobalt_temp = clicked_atoms[0];
    cell.backup[cobalt_temp].x += direction.x;
    cell.backup[cobalt_temp].y += direction.y;
    cell.backup[cobalt_temp].z += direction.z;

    for(unsigned i = 0; i < cell.backup[cobalt_temp].bonds.size(); i++)
    {
        oxygen_temp = cell.backup[cobalt_temp].bonds[i];
        cell.backup[oxygen_temp].x += direction.x;
        cell.backup[oxygen_temp].y += direction.y;
        cell.backup[oxygen_temp].z += direction.z;
    }
    return true;
}


void unDelete(Unit_Cell &cell)
{
    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {
        if(cell.backup[i].visible == false)
        {
            cell.backup[i].visible = true;
        }
    }
}



void setVectors(Unit_Cell &cell, Vec3d &x_direct, Vec3d &y_direct, Vec3d &cross)
{

    int i = cell.getPlane();

    if( i < 0)
    {
        if(cell.geometry.isSet == true)
        {
            x_direct = cell.getA();
            y_direct = cell.getB();
            cross = cell.getC();
            return;
        }
        else
        {
            x_direct = Vec3d(1, 0, 0);
            y_direct = Vec3d(0, 1, 0);
            cross = Vec3d(0, 0, 1);
            return;
        }
    }


    Vec3d one = cell.plane[i].edge[0];
    Vec3d two = cell.plane[i].edge[1];
    Vec3d three = cell.plane[i].edge[2];
    x_direct = (one - two).normalized();
    cross = crossProduct(one - two, three - two).normalized();
    y_direct = crossProduct(x_direct, cross).normalized();

}




void undo(vector<Unit_Cell> &cell, Manager &manage, Stack &stack)
{
        if(stack.size() < 1)
        {
            cout << "No more objects on the stack. Cannot undo. " << endl;
            return;
        }

        Container contents = stack.pop();
        cell = contents.cell;
        manage = contents.manage;
}
