#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "header.h"
#include <iomanip>
#include <sstream>
#include <QDebug>


/*
 * This file contains methods to set standard color and radii for the atoms
*/



/*
 * All atoms are colored in standard color,
 * file 'atomic_colors' contains all standard colors in RGB format (borrowed from jmol)
*/

void addStandardColor(vector<Atom> &input_atom)
{
    ifstream color_data;
    string sname(GetCurrentWorkingDir());
    string cname = sname + "/atomic_colors.txt";
    color_data.open(cname.c_str());
    if(color_data.is_open() == false)
    {
        cout << "Problem in file 'standard_Atom', File with atomic color doesn't exist.";
        return;
    }

    string line;
    vector<string> token_line;
    vector<string> atom_name;
    vector<vector<double> > color;
    while(getline(color_data, line))
    {
        token_line = tokenizer(line);
        atom_name.push_back(token_line[1]);
        token_line = tokenizer(token_line[2], '[');
        token_line = tokenizer(token_line[0], ']');
        token_line = tokenizer(token_line[0], ',');
        vector<double> temp_color;
        temp_color.push_back(atoi(token_line[0].c_str())/255.0);
        temp_color.push_back(atoi(token_line[1].c_str())/255.0);
        temp_color.push_back(atoi(token_line[2].c_str())/255.0);
        color.push_back(temp_color);
    }

    for(unsigned int i = 0; i < input_atom.size(); i++)
    {
        for(unsigned int j = 0; j < atom_name.size(); j++)
        {
            if (input_atom[i].type == atom_name[j])
            {
                input_atom[i].paint[0] = color[j][0];
                input_atom[i].paint[1] = color[j][1];
                input_atom[i].paint[2] = color[j][2];
                break;
            }

        }
    }
    color_data.close();

}


/**
 * @brief addRadius sets the radius of the input atoms. The table of radii for
 *                  different atoms is contained in the file 'atomic_radii.txt'
 * @param input_atom the vector of input atoms.
 * @param radius_type specifies whether the radius of the
 *                    atoms should be Covalent, Atomic, etc.
 * @param correction_factors specifies whether correction factors should be used,
 *                           reduces atomic size and creates a nicer picture,
 *                           but not as realistic.
 *
 */

void addRadius(vector<Atom> &input_atom, string radius_type, bool correction_factors)
{
    double radius_position;
    if(radius_type.find("Van-der-Waals") == 0)
        radius_position = 3;
    else if(radius_type.find("Covalent") == 0)
        radius_position = 2;
    else if(radius_type.find("Atomic") == 0)
        radius_position = 0;
    else if(radius_type.find("Ionic") == 0)
        radius_position = 1;
    else if(radius_type.find("Crystal") == 0)
        radius_position = 4;
    else
        cout << "Problem: " << radius_type <<
                " does not exist, assuming Van-der-Waals radius" << endl;

    double correction1 = 1;
    double correction2 = 1;

    if(correction_factors == true)
    {
        correction1 = 0.2;
        correction2 = 0.35;
    }


    ifstream radius_data;
    string sname(GetCurrentWorkingDir());
    string aname = sname + "/atomic_radii.txt";
    radius_data.open (aname.c_str());
    if(radius_data.is_open() == false)
    {
        cout << "Problem in file 'standard_Atom', File with atomic radii doesn't exist.";
        return;
    }
    string line;
    vector<string> token_line;
    vector<string> atom_name;
    vector<vector<double> > radius;
    while(getline(radius_data, line))
    {
        if (line.find("//") == false)
        {
            continue;
        }
        token_line = tokenizer(line);
        atom_name.push_back(token_line[1]);
        vector <double> temp_radius;
        temp_radius.push_back(atof(token_line[2].c_str()));
        temp_radius.push_back(atof(token_line[3].c_str()));
        temp_radius.push_back(atof(token_line[4].c_str()));
        temp_radius.push_back(atof(token_line[5].c_str()));
        temp_radius.push_back(atof(token_line[6].c_str()));
        radius.push_back(temp_radius);
    }

    for(unsigned int i = 0; i < input_atom.size(); i++)
    {
        for(unsigned int j = 0; j < atom_name.size(); j++)
        {
            if (input_atom[i].type == atom_name[j])
            {
                input_atom[i].radius = radius[j][radius_position]*correction1;
                if (radius[j][3] == -1)
                {
                    for(unsigned int k = 0; k < radius[j].size(); k++)
                    {
                        if (radius[j][k] != -1)
                        {
                            input_atom[i].radius = radius[j][k]*correction2;
                            break;
                        }
                    }
                }
                break;
            }

        }
    }
    radius_data.close();
}

