#ifndef HEADER_H
#define HEADER_H

#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include "math.h"
#include "Unit_Cell.h"
#include <deque>
#include <QMouseEvent>
#include <QMainWindow>


#define MAX_STRING_SIZE 10
#define COBALT_OXYGEN_BOND_TOLERANCE 0.01


using namespace std;
typedef string string;
typedef Atom Atom;
typedef Vec3d Vec3d;
typedef Plane Plane;
typedef Geometry Geometry;

vector<string> tokenizer(string line, string separator);
vector<string> tokenizer(string line);
vector<string> tokenizer(string line, char separator);
void addRadius(vector<Atom> &input_atom, string radius_type, bool correction_factors);
void addStandardColor(vector<Atom> &input_atom);
double angleBetween(Vec3d one, Vec3d two);
double norm(double *array);
double dot(double *one, double *two);
void normalize(double *r);
double round(double x, int number);
double round_to_digits(double value, int digits);
string toString(int number);
string toString(double number);

void mouseClick(int button, int state, int x, int y);
void mouseMotion(int mouseX, int mouseY);

void mousePress(QMouseEvent *event, QMainWindow *input_window);
void mouseMove(QMouseEvent *event, QMainWindow *input_window);
void opengl_keypress(QKeyEvent *event);
Vec3d ClosestPoint(const Vec3d A, const Vec3d B, const Vec3d P);

void display(void);
void configureColor();
void copy_Atom(Atom &copy, Atom &original);
void copyPlane(Plane &copy, Plane &original);
void copyCell(Unit_Cell &copy, Unit_Cell &original);
Vec3d crossProduct(Vec3d one, Vec3d two);


   //all methods that accept Unit_Cell as input. 
typedef Unit_Cell Unit_Cell;
bool molecule_reader_for_xyz(Unit_Cell &cell, string name);
bool read_CIF_FILE(Unit_Cell &cell, string input_name);
string GetCurrentWorkingDir( void );

void print_cell(Unit_Cell &cell);
void private_print_cell(const Unit_Cell &cell, ofstream &myfile);
void reader_gregory_files(vector<Unit_Cell> &cell, string input);
void insertionSort(vector<Vec3d> &initial, Vec3d point, Vec3d cross);
double arctan(double x, double y);

void optimize(Unit_Cell &moved, Unit_Cell &stationary, vector<string> &type);
void makeBonds(Unit_Cell &cell, string type1, string type2, double bond_length, bool outside);
void eraseBonds(Unit_Cell &cell);
Vec3d objectToWindow(Vec3d object);
void windowToObjectf(int mouseX, int mouseY, Vec3d &begin, Vec3d &end);

string printAtom(const Atom &input_atom, int number);
Atom readAtom(string input_atom);
vector<string> printGeometry(const Geometry& geometry);
Geometry readGeometry(vector<string> input);
vector<string> printPlane(const Plane& plane, int number);
Plane readPlane(vector<string> input);

   //move to opengl_keypress later.
void createPlane(vector<Unit_Cell> &cell, Manager &manage);
void reduceCell(Unit_Cell& input);
Vec3d convertToBasis(Unit_Cell &cell, Vec3d location);
Vec3d convertFromBasis(Unit_Cell &cell, Vec3d location);

void algorithm(Unit_Cell &cell);
#include "Stack.h"
#include "Singleton.h"


int glhProjectf(float objx, float objy, float objz, float *modelview,
                float *projection, int *viewport, float *windowCoordinate);
int glhUnProjectf(float winx, float winy, float winz, float *modelview,
                  float *projection, int *viewport, float *objectCoordinate);



#endif   //HEADER_H
