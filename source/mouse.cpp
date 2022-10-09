#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <vector>
#include "mytextbrowser.h"
#include <string>
#include "mainwindow.h"
#include <sstream>
#include "header.h"
#include <QPainter>
#include <QMouseEvent>
#include "Stack.h"

int rightClick(Unit_Cell &cell, int mouseX, int mouseY, Manager &manage);
int leftClick(Unit_Cell &cell, int mouseX, int mouseY, Manager &manage);
int hitAtom(Unit_Cell &cell, int mouseX, int mouseY);
void removeElement(vector<int> &input, int element);

void mousePressRightButton(QMouseEvent *event, QMainWindow *input_window);
void mousePressLeftButton(QMouseEvent *event, QMainWindow *input_window);

void mouseMoveLeftButton(QMouseEvent *event, QMainWindow *input_window);
void mouseMoveRightButton(QMouseEvent *event, QMainWindow *input_window);


void mouseMoveLeftButton(QMouseEvent *event, QMainWindow *input_window)
{
        MainWindow *window = (MainWindow*)input_window;
        Singleton& the_s = Singleton::getInstance();
        if(the_s.cell.size() < 1) return;

        int mouseX = event->pos().x();
        int mouseY = event->pos().y();
        string current_key = "mouse_moved_left_button";
        string previous_key;

        if(the_s.all_keys.size() < 1)
            previous_key = "null";
        else
            previous_key = the_s.all_keys[the_s.all_keys.size() - 1];
        the_s.all_keys.push_back(current_key);


        if (previous_key != current_key)
        {
            the_s.stack.push(Container(the_s.cell, the_s.manage));
            the_s.motion.x = mouseX;
            the_s.motion.y = mouseY;
            return;
        }


        int dx = the_s.motion.x - mouseX;
        int dy = the_s.motion.y - mouseY;



        for(unsigned int i = 0; i < the_s.cell.size(); i++)
        {
            the_s.cell[i].translate(-dx*0.03, dy*0.03, 0);
        }

        window->getMyGLWidget()->update();
        the_s.motion.x = mouseX;
        the_s.motion.y = mouseY;
}

void mouseMoveRightButton(QMouseEvent *event, QMainWindow *input_window)
{
    MainWindow *window = (MainWindow*)input_window;
    Singleton& the_s = Singleton::getInstance();
    if(the_s.cell.size() < 1) return;

    int mouseX = event->pos().x();
    int mouseY = event->pos().y();
    string current_key = "mouse_moved_right_button";
    string previous_key;

    if(the_s.all_keys.size() < 1)
        previous_key = "null";
    else
        previous_key = the_s.all_keys[the_s.all_keys.size() - 1];
    the_s.all_keys.push_back(current_key);

    if (current_key != previous_key)
    {
        the_s.motion.x = mouseX;
        the_s.motion.y = mouseY;
        the_s.stack.push(Container(the_s.cell, the_s.manage));
        return;
    }

    int dx = the_s.motion.x - mouseX;
    int dy = the_s.motion.y - mouseY;

    Vec3d point = Vec3d(0, 0, 0);
    for(unsigned int i = 0; i < the_s.cell.size(); i++)
    {
        point = point + the_s.cell[i].getCenter();
    }

   //point = point/(2.0*the_s.cell.size());
    point = point/(the_s.cell.size());

    for(unsigned int i = 0; i < the_s.cell.size(); i++)
    {

        the_s.cell[i].rotate_all(-dx*0.3, 'y', point);
        the_s.cell[i].rotate_all(-dy*0.3, 'x', point);
    }

    window->getMyGLWidget()->update();
    the_s.motion.x = mouseX;
    the_s.motion.y = mouseY;
}



void mouseMove(QMouseEvent *event, QMainWindow *input_window)
{
        Singleton& the_s = Singleton::getInstance();

        if(the_s.cell.size() < 1) return;

        if(event->buttons() == Qt::LeftButton)
        {
            mouseMoveLeftButton(event, input_window);
            /*


            int dx = the_s.motion.x - mouseX;
            int dy = the_s.motion.y - mouseY;
            if(the_s.save_key.size() < 1)
            {
                the_s.stack.push(Container(the_s.cell, the_s.manage));
            }

            else if(the_s.save_key[the_s.save_key.size() - 1] != 12345)
            {
                the_s.stack.push(Container(the_s.cell, the_s.manage));
            }


            for(unsigned int i = 0; i < the_s.cell.size(); i++)
            {
                the_s.cell[i].translate(-dx*0.03, dy*0.03, 0);
            }

            the_s.save_key.push_back(12345);
            window->getMyGLWidget()->update();
            */

        }

        else if(event->buttons() == Qt::RightButton)
        {
            mouseMoveRightButton(event, input_window);

            /*
            int dx = the_s.motion.x - mouseX;
            int dy = the_s.motion.y - mouseY;

            Vec3d point = Vec3d(0, 0, 0);

            if(the_s.save_key.size() < 1)
            {
                the_s.stack.push(Container(the_s.cell, the_s.manage));
            }

            else if(the_s.save_key[the_s.save_key.size() - 1] != 54321)
            {
                the_s.stack.push(Container(the_s.cell, the_s.manage));
            }


            for(unsigned int i = 0; i < the_s.cell.size(); i++)
            {
                point = point + the_s.cell[i].getCenter();
            }

            //point = point/(2.0*the_s.cell.size());
            point = point/(the_s.cell.size());

            for(unsigned int i = 0; i < the_s.cell.size(); i++)
            {

                the_s.cell[i].rotate_all(-dx*0.3, 'y', point);
                the_s.cell[i].rotate_all(-dy*0.3, 'x', point);
            }

            the_s.save_key.push_back(54321);

            window->getMyGLWidget()->update();
*/
        }

   //the_s.motion.x = mouseX;
   //the_s.motion.y = mouseY;

}


void mousePressLeftButton(QMouseEvent *event, QMainWindow *input_window)
{
    MainWindow *window = (MainWindow*)input_window;
    int mouseX = event->pos().x();
    int mouseY = event->pos().y();

    Singleton& the_s = Singleton::getInstance();
    Manager &manage = the_s.manage;
    if(the_s.cell.size() < 1) return;

    Unit_Cell *cell = NULL;
    cell = &the_s.cell[the_s.manage.getSelected()];

    string current_key = "mouse_press_left_button";
    the_s.all_keys.push_back(current_key);

    int atom_hit = -1;
    atom_hit = hitAtom(*cell, mouseX, mouseY);
    if (atom_hit < 0) return;

    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    bool already_clicked = false;
    for(unsigned int i = 0; i < clicked_atoms.size(); i++)
        if (atom_hit == clicked_atoms[i])
            already_clicked = true;

    if(already_clicked == false)
    {

        the_s.stack.push(Container(the_s.cell, the_s.manage));
        manage.putAtom(atom_hit);

        window->getMyGLWidget()->update();
    }




        string put = cell->backup[atom_hit].type + "(" +
                toString(the_s.manage.getSelected()) + ", " + toString(atom_hit) + ") ";

        put = put + "   x: " + toString(round(cell->backup[atom_hit].x, 3));
        put = put + "   y: " + toString(round(cell->backup[atom_hit].y, 3));
        put = put + "   z: " + toString(round(cell->backup[atom_hit].z, 3));
        window->printText(put );

        put = "Color:    Red=" + toString(round(cell->backup[atom_hit].paint[0],2)) + "   Green=";
        put = put +  toString(round(cell->backup[atom_hit].paint[1], 2)) + "   Blue=";
        put = put +  toString(round(cell->backup[atom_hit].paint[2], 2)) + " ";
        window->printText(put );
        window->printText("");




        if(the_s.manage.getSize() == 2)
        {
            Selected_atom one = the_s.manage.getAtom(0);
            Selected_atom two = the_s.manage.getAtom(1);

            Vec3d first = Vec3d( the_s.cell[one.cell].backup[one.clicked].x,
                         the_s.cell[one.cell].backup[one.clicked].y,
                         the_s.cell[one.cell].backup[one.clicked].z);

            Vec3d second = Vec3d( the_s.cell[two.cell].backup[two.clicked].x,
                         the_s.cell[two.cell].backup[two.clicked].y,
                         the_s.cell[two.cell].backup[two.clicked].z);


            cout << the_s.cell[one.cell].backup[one.clicked].type << "(" << one.cell << "." << one.clicked << ")"
            << " to " <<
                    the_s.cell[two.cell].backup[two.clicked].type << "(" << two.cell << "." << two.clicked << ")"
            << "  distance: " << (first - second).norm() << endl;
            cout << endl;



            string put = the_s.cell[one.cell].backup[one.clicked].type + "(" + toString(one.cell) + ", " + toString(one.clicked)
                         + ")" + " to " +  the_s.cell[two.cell].backup[two.clicked].type + "(" + toString(two.cell) + ", "
                         + toString(two.clicked) + ")" + "  distance: " + toString( round((first - second).norm(), 3));
            window->printText(put);
            window->printText("");



        }

        else if(the_s.manage.getSize() == 3)
        {
            Selected_atom one = the_s.manage.getAtom(0);
            Selected_atom two = the_s.manage.getAtom(1);
            Selected_atom three = the_s.manage.getAtom(2);

            Vec3d first = Vec3d( the_s.cell[one.cell].backup[one.clicked].x,
                         the_s.cell[one.cell].backup[one.clicked].y,
                         the_s.cell[one.cell].backup[one.clicked].z);

            Vec3d second = Vec3d( the_s.cell[two.cell].backup[two.clicked].x,
                         the_s.cell[two.cell].backup[two.clicked].y,
                         the_s.cell[two.cell].backup[two.clicked].z);

            Vec3d third = Vec3d( the_s.cell[three.cell].backup[three.clicked].x,
                         the_s.cell[three.cell].backup[three.clicked].y,
                         the_s.cell[three.cell].backup[three.clicked].z);

            double angle = acos(((first - second).normalized())*((third - second).normalized()));
            cout << the_s.cell[one.cell].backup[one.clicked].type << "(" << one.cell << ", " << one.clicked << ")"
            << "--" <<
                    the_s.cell[two.cell].backup[two.clicked].type << "(" << two.cell << ", " << two.clicked << ")"
            << "--" <<
                the_s.cell[one.cell].backup[three.clicked].type << "(" << three.cell << ", " << three.clicked << ")"
            << "  angle: " <<  angle*180/M_PI << endl;
            cout << endl;


            string put = the_s.cell[one.cell].backup[one.clicked].type + "(" + toString(one.cell) + ", " + toString(one.clicked) +
                    ")" + "--" + the_s.cell[two.cell].backup[two.clicked].type + "(" + toString(two.cell) + ", "
                    + toString(two.clicked) + ")" + "--" + the_s.cell[one.cell].backup[three.clicked].type + "("
                    + toString(three.cell) + ", " + toString(three.clicked) + ")" + "   angle: " +  toString(round(angle*180/M_PI, 3));
             window->printText(put );
             window->printText("");

         }
        the_s.motion.x = mouseX;
        the_s.motion.y = mouseY;
}


void mousePress(QMouseEvent *event, QMainWindow *input_window)
{



    if (event->buttons() == Qt::LeftButton)
    {
        mousePressLeftButton(event, input_window);
        /*
        the_s.button = button;
        atom_hit = leftClick(*cell, mouseX, mouseY, the_s.manage);

        if(atom_hit >= 0)
        {
            string put = cell->backup[atom_hit].type + "(" +
                    toString(the_s.manage.getSelected()) + ", " + toString(atom_hit) + ") ";

            put = put + "   x: " + toString(round(cell->backup[atom_hit].x, 3));
            put = put + "   y: " + toString(round(cell->backup[atom_hit].y, 3));
            put = put + "   z: " + toString(round(cell->backup[atom_hit].z, 3));
            window->printText(put );

            put = "Color:    Red=" + toString(round(cell->backup[atom_hit].paint[0],2)) + "   Green=";
            put = put +  toString(round(cell->backup[atom_hit].paint[1], 2)) + "   Blue=";
            put = put +  toString(round(cell->backup[atom_hit].paint[2], 2)) + " ";
            window->printText(put );
            window->printText("");




            if(the_s.manage.getSize() == 2)
            {
                Selected_atom one = the_s.manage.getAtom(0);
                Selected_atom two = the_s.manage.getAtom(1);

                Vec3d first = Vec3d( the_s.cell[one.cell].backup[one.clicked].x,
                             the_s.cell[one.cell].backup[one.clicked].y,
                             the_s.cell[one.cell].backup[one.clicked].z);

                Vec3d second = Vec3d( the_s.cell[two.cell].backup[two.clicked].x,
                             the_s.cell[two.cell].backup[two.clicked].y,
                             the_s.cell[two.cell].backup[two.clicked].z);


                cout << the_s.cell[one.cell].backup[one.clicked].type << "(" << one.cell << "." << one.clicked << ")"
                << " to " <<
                        the_s.cell[two.cell].backup[two.clicked].type << "(" << two.cell << "." << two.clicked << ")"
                << "  distance: " << (first - second).norm() << endl;
                cout << endl;



                string put = the_s.cell[one.cell].backup[one.clicked].type + "(" + toString(one.cell) + ", " + toString(one.clicked)
                             + ")" + " to " +  the_s.cell[two.cell].backup[two.clicked].type + "(" + toString(two.cell) + ", "
                             + toString(two.clicked) + ")" + "  distance: " + toString( round((first - second).norm(), 3));
                window->printText(put);
                window->printText("");



            }

            else if(the_s.manage.getSize() == 3)
            {
                Selected_atom one = the_s.manage.getAtom(0);
                Selected_atom two = the_s.manage.getAtom(1);
                Selected_atom three = the_s.manage.getAtom(2);

                Vec3d first = Vec3d( the_s.cell[one.cell].backup[one.clicked].x,
                             the_s.cell[one.cell].backup[one.clicked].y,
                             the_s.cell[one.cell].backup[one.clicked].z);

                Vec3d second = Vec3d( the_s.cell[two.cell].backup[two.clicked].x,
                             the_s.cell[two.cell].backup[two.clicked].y,
                             the_s.cell[two.cell].backup[two.clicked].z);

                Vec3d third = Vec3d( the_s.cell[three.cell].backup[three.clicked].x,
                             the_s.cell[three.cell].backup[three.clicked].y,
                             the_s.cell[three.cell].backup[three.clicked].z);

                double angle = acos(((first - second).normalized())*((third - second).normalized()));
                cout << the_s.cell[one.cell].backup[one.clicked].type << "(" << one.cell << ", " << one.clicked << ")"
                << "--" <<
                        the_s.cell[two.cell].backup[two.clicked].type << "(" << two.cell << ", " << two.clicked << ")"
                << "--" <<
                    the_s.cell[one.cell].backup[three.clicked].type << "(" << three.cell << ", " << three.clicked << ")"
                << "  angle: " <<  angle*180/M_PI << endl;
                cout << endl;


                string put = the_s.cell[one.cell].backup[one.clicked].type + "(" + toString(one.cell) + ", " + toString(one.clicked) +
                        ")" + "--" + the_s.cell[two.cell].backup[two.clicked].type + "(" + toString(two.cell) + ", "
                        + toString(two.clicked) + ")" + "--" + the_s.cell[one.cell].backup[three.clicked].type + "("
                        + toString(three.cell) + ", " + toString(three.clicked) + ")" + "   angle: " +  toString(round(angle*180/M_PI, 3));
                 window->printText(put );
                 window->printText("");

             }

            window->getMyGLWidget()->update();
        }
        */
    }


    else if (event->buttons() == Qt::RightButton)
    {

        mousePressRightButton(event, input_window);

        /*
        //the_s.valid = state;
        the_s.button = button;
        atom_hit = rightClick(*cell, mouseX, mouseY, the_s.manage);
        if (atom_hit >= 0)
        {
            window->getMyGLWidget()->update();
        }
        */
    }

}

/*
 * Action when a user presses a right button on a mouse
 */

void mousePressRightButton(QMouseEvent *event, QMainWindow *input_window)
{
    MainWindow *window = (MainWindow*)input_window;
    int mouseX = event->pos().x();
    int mouseY = event->pos().y();

    Singleton& the_s = Singleton::getInstance();
    Manager &manage = the_s.manage;


    if(the_s.cell.size() < 1) return;

    Unit_Cell *cell = NULL;
    cell = &the_s.cell[the_s.manage.getSelected()];

    string current_key = "mouse_press_right_button";
    the_s.all_keys.push_back(current_key);

    int atom_hit = -1;
    atom_hit = hitAtom(*cell, mouseX, mouseY);
    if (atom_hit < 0) return;

    vector<int> clicked_atoms = manage.getAtomsSelectedCell();

    for(unsigned int i = 0; i < clicked_atoms.size(); i++)
        if (atom_hit == clicked_atoms[i])
        {
             the_s.stack.push(Container(the_s.cell, the_s.manage));
             manage.removeSelected(atom_hit);
             window->getMyGLWidget()->update();
             break;
        }

    the_s.motion.x = mouseX;
    the_s.motion.y = mouseY;
}

/*
 * Checks whether a mouse click has hit an atom or not
 * A line is drawn from the x,y mouse coordinates in the z direction
 * and this method determines whether this line intersects any atoms in the unit cell.
 */
int hitAtom(Unit_Cell &cell, int mouseX, int mouseY)
{
    Vec3d begin;
    Vec3d end;
    Vec3d Point;
    Vec3d Q;

    double norm;
    double max;
    int atom_hit = -1;

    windowToObjectf(mouseX, mouseY, begin, end);

    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {


        Point = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);
        Q = ClosestPoint(begin, end, Point);
        norm = (Point - Q).norm();


        if (norm < cell.backup[i].radius && cell.backup[i].visible == true)
        {
            if(atom_hit < 0)
            {
                max = cell.backup[i].z;
                atom_hit = i;
            }
            else if(max < cell.backup[i].z)
            {
                max = cell.backup[i].z;
                atom_hit = i;
            }
        }
    }



    return atom_hit;
}

int leftClick(Unit_Cell &cell, int mouseX, int mouseY, Manager &manage)
{

        int atom_hit = -1;
        atom_hit = hitAtom(cell, mouseX, mouseY);
        if (atom_hit < 0) return atom_hit;

        vector<int> clicked_atoms = manage.getAtomsSelectedCell();

        for(unsigned int i = 0; i < clicked_atoms.size(); i++)
            if (atom_hit == clicked_atoms[i])
                return atom_hit;
        manage.putAtom(atom_hit);

        /*
        if(cell.backup[atom_hit].clicked == false)
        {
            cell.backup[atom_hit].clicked = true;
            cell.clicked_atom.push_back(atom_hit);
            manage.putAtom(atom_hit);
        }

        */

        return atom_hit;
}



int rightClick(Unit_Cell &cell, int mouseX, int mouseY, Manager &manage)
{
    int atom_hit;
    atom_hit = hitAtom(cell, mouseX, mouseY);
    if (atom_hit < 0)
        return -1;

    vector<int> clicked_atoms = manage.getAtomsSelectedCell();
    for(unsigned int i = 0; i < clicked_atoms.size(); i++)
        if(atom_hit == clicked_atoms[i])
        {
            manage.removeSelected(atom_hit);
            return atom_hit;
        }
    return -1;


}

