#ifndef UNIT_CELL_H
#define UNIT_CELL_H


#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include "Atom.h"
#include "Plane.h"
#include "Geometry.h"
#include "Manager.h"


static const double EPSILON = 0.001;

using namespace std;

double inner_product (double x, double y, double z);
Vec3d ArbitraryRotate(Vec3d point, double theta, Vec3d axis);

class Unit_Cell
{
	public:		
		Unit_Cell()
		{
            active = true;
            geometry.toggle = true;
            geometry.polygons = false;
			geometry.toggle_f = true;
            geometry.isSet = false;
            geometry.polygon_shade = 0;
            chemical_type = "None_Specified";
		}
		

        Geometry geometry;   //specifies geometry of the unit cell
        string chemical_type;   //chemical type - essentially the chemical formula
        bool active;   //is this unit cell active
        vector<Plane> plane;   //array of planes specified on that unit cell, initially zero
        vector<Atom> backup;   //array of atoms inside that unit cell
        vector<int> clicked_atom;   //not used, will be erased soon
		

/**
         * @brief copy creates a copy of the Unit_Cell
         * @return a copy of the created Unit_Cell
         */
        Unit_Cell copy()
        {
            Unit_Cell copy;
            copy.geometry = geometry.copy();
            copy.chemical_type = chemical_type;
            copy.active = active;

            copy.backup.resize(backup.size());
            for( unsigned int i = 0; i < backup.size(); i++)
            {
                copy.backup[i] = backup[i].copy();
                copy.backup[i].clicked = false;
            }

            copy.plane.resize(plane.size());
            for ( unsigned int i = 0; i < plane.size(); i++)
            {
                copy.plane[i] = plane[i].copy();
            }
            return copy;
        }

/**
         * @brief printCell prints a Unit_Cell into a vector of strings
         * @param number position of the given Unit_Cell in the cell array
         * @return a vector of strings
         */
        vector<string> printCell(int number)
        {
            vector<string> output;
            output.push_back("_begin_Unit_Cell   " + SSTR(number));

            if(chemical_type == "")
                output.push_back("chemical_type:   None_specified");
            else
                output.push_back("chemical_type:   " + chemical_type);

            output.push_back("active   " + SSTR(active));

            vector<string> temp_geometry = this->geometry.printGeometry();
            output.insert(output.end(), temp_geometry.begin(), temp_geometry.end());

            output.push_back("_begin_atoms   "  + SSTR(backup.size()));
            for(unsigned int i = 0; i < backup.size(); i++)
                output.push_back(backup[i].printAtom(i));
            output.push_back("_end_atoms");

            output.push_back("_start_all_Planes   " + SSTR(plane.size()));
            for(unsigned int i = 0; i < plane.size(); i++)
            {
                vector<string> temp_plane = plane[i].printPlane(i);
                output.insert(output.end(), temp_plane.begin(), temp_plane.end());
            }
            output.push_back("_end_all_Planes");
            output.push_back("_end_Unit_Cell   " + SSTR(number));
            return output;
        }


/**
         * @brief readCell parses a vector of strings into a unit cell
         * @param input vector of strings that are to be parsed into a Unit_Cell
         */
        void readCell(vector<string> input)
        {
               for(unsigned int i = 0; i < input.size(); i++)
               {
                   if(input[i].find("_begin_atoms") != string::npos)
                   {
                       i += 1;
                       backup.resize(0);
                       while(input[i].find("_end_atoms") == string::npos)
                       {
                           Atom one_temp = Atom();

                           one_temp.readAtom(input[i]);
                           backup.push_back(one_temp);
                           i += 1;
                       }


                   }
                   else if(input[i].find("chemical_type") != string::npos)
                   {
                       vector<string> for_temp = tokenizer(input[i], ":");
                       if(for_temp.size() < 1)
                           chemical_type = "None_specified";
                       else
                            chemical_type = tokenizer(input[i], ":")[1];
                   }
                   else if(input[i].find("active") != string::npos)
                   {
                       active = atoi(tokenizer(input[i], " ")[1].c_str());
                   }

                   else if(input[i].find("_begin_geometry") != string::npos)
                   {
                       vector<string> geometry_input;
                       while(input[i].find("_end_geometry") == string::npos)
                       {
                           geometry_input.push_back(input[i]);
                           i += 1;
                       }
                       geometry.readGeometry(geometry_input);
                   }

                   else if(input[i].find("_start_all_Planes") != string::npos)
                   {
                       plane.resize(0);
                        i += 1;
                        while(input[i].find("_end_all_Planes") == string::npos)
                        {
                            vector<string> temp_plane;
                            while(input[i].find("_end_Plane") == string::npos)
                            {
                                temp_plane.push_back(input[i]);
                                i += 1;
                            }
                            Plane one_plane = Plane();
                            if(temp_plane.size() < 1)
                                break;
                            one_plane.readPlane(temp_plane);
                            plane.push_back(one_plane);
                            i += 1;
                        }
                   }

                   else if(input[i].find("_end_Unit_Cell") != string::npos)
                       return;

               }
        }
		
/**
         * @brief add_atom adds an atom to the unit cell.
         * @param input type of atom
         * @param x
         * @param y
         * @param z
         */
		void add_atom(string input, double x, double y, double z)
		{
			Atom temp;
			temp.type = input;
			temp.x = x;
			temp.y = y;
			temp.z = z;
			backup.push_back(temp);
		}
		

/**
         * @brief getCenter
         * @return the center of the unit cell
         */
		Vec3d getCenter()
		{
            if(geometry.isSet != true)
                setCenter();
            return geometry.center;
		}

/**
         * @brief setCenter calculates the center of the unit cell and sets it in the
         *                  variable 'geometry.center'
         */
        void setCenter()
        {
            unsigned int i;
            Vec3d center = Vec3d(0, 0, 0);
            for(i = 0; i < backup.size(); i++)
            {
                if (backup[i].visible == true)
                {
                    center.x = center.x + backup[i].x;
                    center.y = center.y + backup[i].y;
                    center.z = center.z + backup[i].z;
                }
            }
            double number = (double)backup.size();
            center.x = center.x/number;
            center.y = center.y/number;
            center.z = center.z/number;
            geometry.center = center;
        }

/**
         * @brief setGeometry an outdated method, sets the basis vectors for xyz file of LCO
         * @param input
         * @param a_
         * @param b_
         * @param c_
         * @param alpha
         * @param beta
         * @param gamma
         */
		void setGeometry(string input, double a_, double b_, double c_,  double alpha, double beta, double gamma)
		{	

			double max = 0;
			double temp;
			int hold_i;

			for(unsigned int i = 0; i < backup.size(); i++)
			{
				for(unsigned int j = i + 1; j < backup.size(); j++)
				{
					if(!(backup[i].type.compare(input)) && !(backup[j].type.compare(input)))
					{	
						temp = inner_product(
						backup[i].x - backup[j].x,
						backup[i].y - backup[j].y,
						backup[i].z - backup[j].z);
					
						if (max <= temp)
						{	
							hold_i = i;
							max = temp;
						}
					}
				}
			}

            getCenter();




            unsigned int i;
			unsigned int j;


			geometry.zero_unit = Vec3d(backup[hold_i].x, backup[hold_i].y, backup[hold_i].z);
			
			for( i = 0; i < backup.size(); i++)
			{
				if(!(backup[i].type.compare(input)))
				{
					temp = inner_product(backup[i].x - geometry.zero_unit.x , backup[i].y - geometry.zero_unit.y, backup[i].z - geometry.zero_unit.z);

					if (fabs(a_ - temp) < EPSILON)
					{
						break;
					}
				}
			}

			if (i == backup.size())
			{
				cout << "The Parameters are wrong. Mistake in: " << __LINE__ << '\n';
				cout << "a_ = " << a_ << " is not one of the primitive vectors" << '\n';
				exit(1);
			}
			geometry.a_unit = Vec3d(backup[i].x, backup[i].y, backup[i].z);
            int geometry_a = i;
			
			for( j = 0; j < backup.size(); j++)
			{
                if(!(backup[j].type.compare(input)) && (int)j != geometry_a)
				{	
					temp = inner_product(backup[j].x - geometry.zero_unit.x , backup[j].y - geometry.zero_unit.y, backup[j].z - geometry.zero_unit.z);

					if (fabs(b_ - temp) < EPSILON)
					{
						break;
					}
				}
			}
			
			if (j == backup.size())
			{
				cout << "The Parameters are wrong. Mistake in: " << __LINE__ << '\n';
				cout << "b_ = " << a_ << " is not one of the primitive vectors" << '\n';
				exit(1);
			}
            int geometry_b = j;
			geometry.b_unit = Vec3d(backup[j].x, backup[j].y, backup[j].z);
			
			temp = c_;
			for( i = 0; i < backup.size(); i++)
			{
                if(!(backup[i].type.compare(input)) && (int)i != geometry_a && (int)i != geometry_b)
				{
					
					temp = inner_product(backup[i].x - geometry.zero_unit.x , backup[i].y - geometry.zero_unit.y, backup[i].z - geometry.zero_unit.z);

					if (fabs(c_ - temp) < EPSILON)
					{
						break;
					}
				}
			}
			
			if (i == backup.size())
			{
				cout << "The Parameters are wrong. Mistake in: " << __LINE__ << '\n';
				cout << "c_ = " << a_ << " is not one of the primitive vectors" << '\n';
				exit(1);
			}
			geometry.c_unit = Vec3d(backup[i].x, backup[i].y, backup[i].z);
			geometry.alpha = alpha;
			geometry.beta = beta;
			geometry.gamma = gamma;
            geometry.isSet = true;
		}
		
		
/**
         * @brief translate the Unit_Cell by a specified amount
         * @param x
         * @param y
         * @param z
         */
		void translate(double x, double y, double z)
		{			
			Vec3d move = Vec3d(x, y, z);

			for(unsigned int k = 0; k < backup.size(); k++)
			{
				backup[k].x += x;
				backup[k].y += y;
				backup[k].z += z;
			}

			for(unsigned int m = 0; m < plane.size(); m++)
                plane[m].translate(move);

            geometry.translate(move);
		}

/**
         * @brief rotate_all rotates the Unit_Cell about some direction ('x', 'y', 'z')
         *                   with respect to a given point
         * @param angle by which the unit cell is rotated
         * @param direction about which the Unit_Cell is rotated
         * @param point with respect to which the Unit_Cell is rotated.
         */
		void rotate_all(double angle, char direction, Vec3d point)
		{
            Vec3d direction_vector;
            if (direction == 'x')
                direction_vector = Vec3d(1, 0, 0);
            else if (direction == 'y')
                direction_vector = Vec3d(0, 1, 0);
            else
                direction_vector = Vec3d(0, 0, 1);
			
            rotate_all(angle, direction_vector, point);
		}
		

        void rotate_all(double angle, Vec3d direction, Vec3d center)
        {
            Vec3d point;
            Vec3d result;

            translate(-center.x, -center.y, -center.z);

            for(unsigned int k = 0; k < backup.size(); k++)
            {
                point = Vec3d(backup[k].x, backup[k].y, backup[k].z);
                result = ArbitraryRotate(point, angle, direction);
                backup[k].x = result.x;
                backup[k].y = result.y;
                backup[k].z = result.z;

            }

            for(unsigned int j = 0; j < plane.size(); j++)
            {
                plane[j].rotate(angle, direction);
            }

            geometry.rotate(angle, direction);
            translate(center.x, center.y, center.z);
        }

		void rotate_all(double angle, Vec3d direction)
		{
			Vec3d center = getCenter();
            rotate_all(angle, direction, center);
		}
		
/**
         * @brief setColor for a given atom type, its color is set
         * @param input type of atom, located in atom.type
         * @param paint an array of 3 elements specifying color in RGB
         */
		void setColor(string input, double *paint)
		{

			for(unsigned int i = 0; i < backup.size(); i++)
			{
				if(!(backup[i].type.compare(input)))
				{
					backup[i].paint[0] = paint[0];
					backup[i].paint[1] = paint[1];
					backup[i].paint[2] = paint[2];
				}
				
			}
		}
	
		void setColor(int input, double *paint)
		{
			backup[input].paint[0] = paint[0];
			backup[input].paint[1] = paint[1];
			backup[input].paint[2] = paint[2];
		}
		
   //color is inverted
        void invertColor()
        {
            for(unsigned int i = 0; i < backup.size(); i++)
            {
                backup[i].paint[0] = 1 - backup[i].paint[0];
                backup[i].paint[1] = 1 - backup[i].paint[1];
                backup[i].paint[2] = 1 - backup[i].paint[2];
            }

        }
			
		vector<string> getTypes()
		{
			vector<string> type;
			type.push_back(backup[0].type);
			unsigned int i;
			
			for(unsigned int k = 1; k < backup.size(); k++)
			{
				for(i = 0; i < type.size(); i++)
				{
					if(!backup[k].type.compare(type[i])) break;
				}
			
				if(i == type.size()) type.push_back(backup[k].type);	
			}
			
			return type;
		}

        vector<Atom> getInfo()
        {
            vector<Atom> type;
            type.push_back(backup[0]);
            unsigned int i;

            for(unsigned int k = 1; k < backup.size(); k++)
            {
                for(i = 0; i < type.size(); i++)
                {
                    if(!backup[k].type.compare(type[i].type)) break;
                }

                if(i == type.size()) type.push_back(backup[k]);
            }

            return type;
        }


		void setRadii(string type, double radii)
		{
			for(unsigned int i = 0; i < backup.size(); i++)
				if(backup[i].type.compare(type) == 0) backup[i].radius = radii;	
		}

		
		Vec3d getZero()
		{
			return geometry.zero_unit;
		}
		
		Vec3d getA()
		{
			return geometry.a_unit - geometry.zero_unit;
		}
		
		Vec3d getB()
		{

			return geometry.b_unit - geometry.zero_unit;
		}
		
		Vec3d getC()
		{
			return geometry.c_unit - geometry.zero_unit;
		}
		
		

/**
         * Determines if a given point is inside a parallelipid determined by
         * the basis vectors of this Unit_Cell
         *
         * @param pos the position of the point in x, y, z coordinates
         * @returns true or false
        */

        bool isInside(Vec3d pos)
        {
            Vec3d cross;
            Vec3d a_unit = getA();
            Vec3d b_unit = getB();
            Vec3d c_unit = getC();
            pos = pos - geometry.zero_unit;
            double cosine;
            Vec3d position;
            double epsilon = 0.01;

   //front face
            cross = a_unit.cross(b_unit).normalized();
            position = pos.normalized();
            cosine = cross*position;
            if (cosine < -epsilon)
                return false;

            cross = c_unit.cross(a_unit).normalized();
            position = pos.normalized();
            cosine = cross*position;
            if (cosine < -epsilon)
                return false;

            cross = b_unit.cross(c_unit).normalized();
            position = pos.normalized();
            cosine = cross*pos;
            if (cosine < -epsilon)
                return false;


   //back face;
            cross = (a_unit.cross(b_unit)).normalized();
            position = (pos - c_unit).normalized();
            cosine = cross*position;
            if (cosine > epsilon)
                return false;

            cross = (c_unit.cross(a_unit)).normalized();
            position = (pos - b_unit).normalized();
            cosine = cross*position;
            if (cosine > epsilon)
                return false;

            cross = b_unit.cross(c_unit).normalized();
            position = (pos - a_unit).normalized();
            cosine = cross*position;
            if (cosine > epsilon)
                return false;


            return true;

        }

/**
         * @brief getPlane method to get the number of the selected plane in the unit cell
         * @return the number of the selected plane,
         *         if no plane is selected, first plane in the plane array is returned
         */
		int getPlane()
		{
			if(plane.size() < 1) return -1;

			for(unsigned int i = 0; i < plane.size(); i++)
				if(plane[i].selected == true) return i;

			plane[0].selected = true;
			return 0;
		}
		
};





#endif   //UNIT_CELL_H
