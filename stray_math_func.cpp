#include <vector>
#include <math.h>
#include "header.h"
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <QMatrix4x4>

#include <stdio.h>/* defines FILENAME_MAX */
   // #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


using namespace std;


/**
 * @brief GetCurrentWorkingDir gets the current working directory,
 *                             this method only works in Unix.
 * @return string of the current working directory
 */
string GetCurrentWorkingDir( void )
{
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    string current_working_dir(buff);
    return current_working_dir;
}

/**
 * @brief toString converts an int to a string
 * @param number input int
 * @return a string
 */
string toString(int number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}

/**
 * @brief toString converts an double to a string
 * @param number input double
 * @return a string
 */
string toString(double number)
{
    ostringstream convert;
    convert << number;
    return convert.str();
}


/**
 * @brief makeBonds creates bonds between atoms, will not change the number
 *                  of atoms in the given Unit_Cell
 * @param cell Unit_Cell where bonds will be created
 * @param type1 the atom from which bonds go
 * @param type2 the atom to which bonds go
 * @param bond_length maximal bond length
 */
void makeBondsInside(Unit_Cell &cell, string type1, string type2, double bond_length)
{
	Vec3d one;
	Vec3d two;

	for(unsigned int i = 0; i < cell.backup.size(); i++)
	{
		if(cell.backup[i].type.compare(type1) != 0)  continue;
		
        cell.backup[i].bonds.resize(0);
		for(unsigned int j = 0; j < cell.backup.size(); j++)
		{
			if(cell.backup[j].type.compare(type2) != 0) continue;
			
			one = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);
			two = Vec3d(cell.backup[j].x, cell.backup[j].y, cell.backup[j].z);
			
			if((one - two).norm() < bond_length)
			{
				cell.backup[i].bonds.push_back(j);
			}		
			
		}				

	}			
}


/**
 * @brief makeBonds creates bonds between atoms, checks for bonds outside the
 *                  Unit_Cell if any exist they are created and atoms are added
 *                  to the Unit_Cell. This method can increase the total
 *                  number of atoms, watch out for this effect.
 * @param cell Unit_Cell where bonds will be created
 * @param type1 the atom from which bonds go
 * @param type2 the atom to which bonds go
 * @param bond_length maximal bond length
 * @param outside specifies whether the method should look to make bonds outside.
 */

void makeBonds(Unit_Cell &cell, string type1, string type2,
                      double bond_length, bool outside)
{
    if(outside == false)
    {
        makeBondsInside(cell, type1, type2, bond_length);
        return;
    }

    cout << "got here" << endl;
    int xiter[3] = {-1, 0, 1};
    int yiter[3] = {-1, 0, 1};
    int ziter[3] = {-1, 0, 1};

    Vec3d a_dir = cell.getA();
    Vec3d b_dir = cell.getB();
    Vec3d c_dir = cell.getC();

    Vec3d one;
    Vec3d two;

    const double CLOSEST_APPROACH = 0.0001;

    unsigned int size = cell.backup.size();

    for(unsigned int i = 0; i < size; i++)
    {
        if(cell.backup[i].type.compare(type1) != 0)  continue;

        cell.backup[i].bonds.resize(0);

        for(unsigned int j = 0; j < size; j++)
        {
            if(cell.backup[j].type.compare(type2) != 0) continue;


            one = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);


            for(unsigned int k = 0; k < 3; k++)
                for(unsigned int l = 0; l < 3; l++)
                    for(unsigned int m = 0; m < 3; m++)
                    {

                        two = Vec3d(cell.backup[j].x, cell.backup[j].y, cell.backup[j].z);
                        two = two + a_dir*xiter[k] + b_dir*yiter[l] + c_dir*ziter[m];

                        if((one - two).norm() < bond_length)
                        {
                            if(k == 1 && l == 1 && m == 1)
                            {
                                cell.backup[i].bonds.push_back(j);
                                continue;
                            }

                            bool create = true;
                            for(unsigned int n = 0; n < cell.backup.size(); n++)
                            {
                                Vec3d three = Vec3d(cell.backup[n].x, cell.backup[n].y, cell.backup[n].z);
                                if((three - two).norm() < CLOSEST_APPROACH)
                                {
                                    create = false;
                                    break;
                                }
                            }
                            if(create == false)
                                continue;
                            Atom atom = cell.backup[j].copy();
                            atom.x = two.x;
                            atom.y = two.y;
                            atom.z = two.z;
                            atom.bonds.resize(0);
                            cell.backup.push_back(atom);
                            cell.backup[i].bonds.push_back(cell.backup.size() - 1);
                        }

                    }

        }

    }

}

/**
 * @brief eraseBonds bonds in the Unit_Cell are erased
 * @param cell the input cell.
 */
void eraseBonds(Unit_Cell &cell)
{
	for(unsigned int i = 0; i < cell.backup.size(); i++)
	{
		cell.backup[i].bonds.resize(0);
	}
}



double inner_product (double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

double round(double x, int number)
{
    double power = pow(10, number);
    return double(int(x*power + 0.5))/power;
}



void rotate( double *nX, double *nY, double *nZ, double angle, char direction)
{
	double x = *nX;
	double y = *nY;
	double z = *nZ;
	
	if( direction == 'x')
	{	
		*nX = x;
		*nY = cos(angle)*y - sin(angle)*z;
		*nZ = sin(angle)*y + cos(angle)*z;
	}
	
	else if (direction == 'y')
	{
		
		*nX = cos(angle)*x +  sin(angle)*z;
		*nY = y;
		*nZ = -sin(angle)*x + cos(angle)*z;
	}
	
	else
	{
		*nX = cos(angle)*x - sin(angle)*y;
		*nY = sin(angle)*x + cos(angle)*y;
		*nZ = z;
	}
}


double angle_between(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return acos((x1*x2 + y1*y2 + z1*z2)/((inner_product(x1, y1, z1)*(inner_product(x2, y2, z2)))))*180.0/M_PI;
}

/**
 * @brief angleBetween a
 * @param one
 * @param two
 * @return the angle is returned in radians
 */
double angleBetween(Vec3d one, Vec3d two)
{
    return angle_between(one.x, one.y, one.z, two.x, two.y, two.z);
}

void cross_product(double x1, double y1, double z1, double x2, double y2, double z2, double *result)
{
	result[0] = y1*z2 - z1*y2;
	result[1] = -(x1*z2 - z1*x2);
	result[2] = x1*y2 - y1*x2;
}

Vec3d crossProduct(Vec3d one, Vec3d two)
{
	return Vec3d(one.y*two.z - one.z*two.y, -(one.x*two.z - one.z*two.x), one.x*two.y - one.y*two.x);
}


double norm(double *array)
{
	return sqrt(array[0]*array[0] + array[1]*array[1] + array[2]*array[2]);
}

double dot(double *one, double *two)
{
	return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}

/**
 * @brief normalize a 3 component vector is normalized
 * @param r input vector
 */
void normalize(double *r)
{
	double sum = inner_product(r[0], r[1], r[2]);
	r[0] = r[0]/sum;
	r[1] = r[1]/sum;
	r[2] = r[2]/sum;	
}


/*
   Rotate a point by angle theta around an arbitrary axis
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system. Answer returned in the form of a vector
*/
Vec3d ArbitraryRotate(Vec3d point, double theta, Vec3d axis)
{
	Vec3d result;
	double p[3] = {point.x, point.y, point.z};
	double q[3];
	double r[3] = {axis.x, axis.y, axis.z};
	
	double costheta,sintheta;
	q[0] = 0;
	q[1] = 0;
	q[2] = 0;
   normalize(r);
   costheta = cos(theta*M_PI/180.0);
   sintheta = sin(theta*M_PI/180.0);

   q[0] += (costheta + (1 - costheta) * r[0] * r[0]) * p[0];
   q[0] += ((1 - costheta) * r[0] * r[1] - r[2] * sintheta) * p[1];
   q[0] += ((1 - costheta) * r[0] * r[2] + r[1] * sintheta) * p[2];

   q[1] += ((1 - costheta) * r[0] * r[1] + r[2] * sintheta) * p[0];
   q[1] += (costheta + (1 - costheta) * r[1] * r[1]) * p[1];
   q[1] += ((1 - costheta) * r[1] * r[2] - r[0] * sintheta) * p[2];

   q[2] += ((1 - costheta) * r[0] * r[2] - r[1] * sintheta) * p[0];
   q[2] += ((1 - costheta) * r[1] * r[2] + r[0] * sintheta) * p[1];
   q[2] += (costheta + (1 - costheta) * r[2] * r[2]) * p[2];
	
	
	result.x = q[0];
	result.y = q[1];
	result.z = q[2];
	return result;	
}





/**
 * @brief insertionSort This method is used to sort the edges to lie in a circle around the
 *                      center point.
 * @param initial collection of vectors
 * @param point specifies the center of the circle
 * @param cross vector normal to this circle
 */
void insertionSort(vector<Vec3d> &initial, Vec3d point, Vec3d cross)
{
    unsigned int i, j;
	Vec3d tmp;
	Vec3d calc;
	double angle1;
	double angle2;
    Vec3d unit_1 = (initial[0] - point).normalized();
	Vec3d unit_2 = crossProduct(cross, unit_1).normalized();
		
 	for (i = 1; i < initial.size(); i++)
	{
 		j = i;
 		while (j > 0)
		{	
            calc = initial[j] - point;
			angle1 = arctan(calc*unit_2, calc*unit_1);
			
            calc = initial[j - 1] - point;
			angle2 = arctan(calc*unit_2, calc*unit_1);
			
			if(angle2 > angle1) break;
			
 			tmp = initial[j];
 			initial[j] = initial[j - 1];
 			initial[j - 1] = tmp;
 			j--;
 		}   //end of while loop 
 	}
}

double arctan(double x, double y)
{
	if(x > 0 && y > 0) return atan(y/x);
	else if(x > 0 && y < 0) return 2*M_PI + atan(y/x);
	else if(x < 0 && y < 0) return M_PI + atan(y/x);
	else return M_PI + atan(y/x);
}

/**
 * @brief ClosestPoint calculates the minimal distance between point P and
 *                     vector defined by points A, B.
 * @param A defines a point
 * @param B defines a point
 * @param P defines a point
 * @return minimal distance between point P and vector AB is returned
 *         this method is necessary to determine if the mouse click has
 *         hit an atom.
 */
Vec3d ClosestPoint(const Vec3d A, const Vec3d B, const Vec3d P)
{
    double t;
    Vec3d AB = B - A;
    double ab_square = AB * AB;
    Vec3d AP = P - A;
    double ap_dot_ab = AP * AB;
   // t is a projection param when we project vector AP onto AB
    t = ap_dot_ab / ab_square;
   // calculate the closest point
    Vec3d Q = A + AB * (t);
    return Q;
}

/**
 * @brief windowToObjectf screen coordinates are used to draw a vector from the screen
 *                        inwards to the object. This method is needed to determine whether
 *                        a user has clicked on an atom or not
 * @param mouseX coordinate of the mouse
 * @param mouseY coordinate of the mouse
 * @param begin  initial point of the vector
 * @param end    final point of the vector
 */
void windowToObjectf(int mouseX, int mouseY, Vec3d &begin, Vec3d &end)
{

   //double  modelViewMatrix[16], projectionMatrix[16];
   //glGetDoublev( GL_MODELVIEW_MATRIX, modelViewMatrix );
   //glGetDoublev( GL_PROJECTION_MATRIX, projectionMatrix);

    float  modelViewMatrix[16], projectionMatrix[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modelViewMatrix );
    glGetFloatv( GL_PROJECTION_MATRIX, projectionMatrix);

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );


    double winX = (double)mouseX;

    double winY = viewport[3] - (double)mouseY;


    float objectCoordinate[3];

    glhUnProjectf(winX, winY, 0.0, modelViewMatrix,
                      projectionMatrix,  viewport, objectCoordinate);
    begin.x = objectCoordinate[0];
    begin.y = objectCoordinate[1];
    begin.z = objectCoordinate[2];

    glhUnProjectf(winX, winY, 1.0, modelViewMatrix,
                      projectionMatrix,  viewport, objectCoordinate);
    end.x = objectCoordinate[0];
    end.y = objectCoordinate[1];
    end.z = objectCoordinate[2];


   //gluUnProject(winX, winY, 0.0, modelViewMatrix, projectionMatrix, viewport, &begin.x, &begin.y, &begin.z);
   //gluUnProject(winX, winY, 1.0, modelViewMatrix, projectionMatrix, viewport, &end.x, &end.y, &end.z);

}


/**
 * @brief tokenizer splits the string along a given char
 * @param line input string
 * @param separator the char to separate
 * @return vector of separated strings
 */
vector<string> tokenizer(string line, char separator)
{
    string separ;
    separ.push_back(separator);
    return tokenizer(line, separ);
}


/**
 * @brief tokenizer splits the string along a given collection of chars
 * @param line input string
 * @param separator the chars to separate
 * @return vector of separated strings
 */
vector<string> tokenizer(string line, string separator)
{
    unsigned int temp = 0;
    string place_hold;
    vector<string> result;
    bool divide;

    for(unsigned int i = 0; i < separator.size(); i++)
        if(separator[i] == ' ')
            separator.push_back('\t');

    for(unsigned int i = 0; i < line.length(); i = i + temp + 1)
    {
        divide = false;

        for(temp = 0; (temp + i) < line.length() && divide == false;)
        {
            for(unsigned int j = 0; j < separator.length(); j++)
            {
                if(line[i + temp] == separator[j])
                {
                    divide = true;
                    break;
                }
            }
            if(divide == false) temp++;
        }


        if (temp > 0)
        {
            result.push_back(place_hold.assign(line, i, temp));
        }

    }
    return result;
}



/**
 * @brief tokenizer splits the string along a space
 * @param line input string
 * @return vector of separated strings
 */
vector<string> tokenizer(string line)
{
    string separator;
    separator.push_back(' ');
    separator.push_back('\t');
    return tokenizer(line, separator);
}

/**
 * @brief round_to_digits rounds the input value to the
 *                        specified number of significant digits
 *
 * @param value - input value
 * @param digits - number of digits to retain
 * @return rounded value with significant figures = @param digits
 */
double round_to_digits(double value, int digits)
{
    if (value == 0.0)   // otherwise it will return 'nan' due to the log10() of zero
        return 0.0;

    double factor = pow(10.0, digits - ceil(log10(fabs(value))));
    return round(value * factor) / factor;
}



/**
 * @brief reduceCell eliminates all the deleted atoms in the given unit cell
 * @param input Unit_Cell
 */
void reduceCell(Unit_Cell& input)
{

   //all the bonds connected to invisible atoms are destroyed.
    for(unsigned int i = 0; i < input.backup.size(); i++)
    {
        vector<int> visible_bonds;
        for(unsigned int j = 0; j < input.backup[i].bonds.size(); j++)
        {
            if( input.backup[input.backup[i].bonds[j]].visible == true)
                visible_bonds.push_back(input.backup[i].bonds[j]);
        }
        input.backup[i].bonds = visible_bonds;
    }


   //creates an array of strictly visible atoms.
   //as well as an array of the previous location of those atoms.
    vector<Atom> array_temp;
    vector<int> hold;
    for(unsigned int i = 0; i < input.backup.size(); i++)
    {
        if(input.backup[i].visible == true)
        {
            array_temp.push_back(input.backup[i].copy());
            hold.push_back(i);
        }
    }

   //locations of bonds are corrected, as previous locations point to atoms
   //that are now in a different place.
    for(unsigned int i = 0; i < array_temp.size(); i++)
    {
        for(unsigned int j = 0; j < array_temp[i].bonds.size(); j++)
        {
            int sg_bond = array_temp[i].bonds[j];
            for(unsigned int k = 0; k < hold.size(); k++)
            {
                if(hold[k] == sg_bond)
                {
                    array_temp[i].bonds[j] = k;
                    break;
                }
            }
        }
    }
    input.backup = array_temp;

}


Vec3d convertToBasis(Unit_Cell &cell, Vec3d location)
{
    QMatrix4x4 qmat;
    bool invertible;
    qmat(0, 0) = cell.getA().x;
    qmat(1, 0) = cell.getA().y;
    qmat(2, 0) = cell.getA().z;
    qmat(3, 0) = 0;

    qmat(0, 1) = cell.getB().x;
    qmat(1, 1) = cell.getB().y;
    qmat(2, 1) = cell.getB().z;
    qmat(3, 1) = 0;

    qmat(0, 2) = cell.getC().x;
    qmat(1, 2) = cell.getC().y;
    qmat(2, 2) = cell.getC().z;
    qmat(3, 2) = 0;

    qmat(0, 3) = 0;
    qmat(1, 3) = 0;
    qmat(2, 3) = 0;
    qmat(3, 3) = 1;

    QMatrix4x4 inverse = qmat.inverted(&invertible);
    Vec3d xcolumn = Vec3d(inverse(0, 0), inverse(1, 0), inverse(2, 0));
    Vec3d ycolumn = Vec3d(inverse(0, 1), inverse(1, 1), inverse(2, 1));
    Vec3d zcolumn = Vec3d(inverse(0, 2), inverse(1, 2), inverse(2, 2));
    return xcolumn*location.x + ycolumn*location.y + zcolumn*location.z;
}

Vec3d convertFromBasis(Unit_Cell &cell, Vec3d location)
{
    QMatrix4x4 qmat;
    qmat(0, 0) = cell.getA().x;
    qmat(1, 0) = cell.getA().y;
    qmat(2, 0) = cell.getA().z;
    qmat(3, 0) = 0;

    qmat(0, 1) = cell.getB().x;
    qmat(1, 1) = cell.getB().y;
    qmat(2, 1) = cell.getB().z;
    qmat(3, 1) = 0;

    qmat(0, 2) = cell.getC().x;
    qmat(1, 2) = cell.getC().y;
    qmat(2, 2) = cell.getC().z;
    qmat(3, 2) = 0;

    qmat(0, 3) = 0;
    qmat(1, 3) = 0;
    qmat(2, 3) = 0;
    qmat(3, 3) = 1;

    Vec3d xcolumn = Vec3d(qmat(0, 0), qmat(1, 0), qmat(2, 0));
    Vec3d ycolumn = Vec3d(qmat(0, 1), qmat(1, 1), qmat(2, 1));
    Vec3d zcolumn = Vec3d(qmat(0, 2), qmat(1, 2), qmat(2, 2));
    return xcolumn*location.x + ycolumn*location.y + zcolumn*location.z;
}









