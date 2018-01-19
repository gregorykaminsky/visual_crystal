#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "header.h"
#include <iomanip>
#include <sstream>
#include <QDebug>

#include <stdio.h>
/*
 * This reads cif files, they must end with '.cif'
 */


   //cif files usually contain symmetry operations,
   //this structure saves a single symmetry.
   //struct operation{int place; double sign; double frac;};
struct operation{
    double x;
    double y;
    double z;
    double frac;
};


/*
 * class to manipulate symmetries in a cif file
 */
class Symmetry
{
    operation x_axis;
    operation y_axis;
    operation z_axis;
    string sym;

    public:
        Symmetry (){;}

        Symmetry(string input)
        {
            vector<string> divide = tokenizer(input, " ,");
            parse(divide[0], x_axis);
            parse(divide[1], y_axis);
            parse(divide[2], z_axis);
            sym = input;
        }

        string symString()
        {
            return sym;
        }

        void parse(string input, operation &formula)
        {
            int pos = -1;
            for(unsigned int j = 0; j < input.size(); j++)
                if(input[j] == 'x')
                    pos = j;

            if(pos == -1)
                formula.x = 0;
            else if(pos == 0)
                formula.x = 1;
            else if(input[pos - 1] == '+')
                formula.x = 1;
            else
                formula.x = -1;


            pos = -1;
            for(unsigned int j = 0; j < input.size(); j++)
                if(input[j] == 'y')
                    pos = j;

            if(pos == -1)
                formula.y = 0;
            else if(pos == 0)
                formula.y = 1;
            else if(input[pos - 1] == '+')
                formula.y = 1;
            else
                formula.y = -1;

            pos = -1;
            for(unsigned int j = 0; j < input.size(); j++)
                if(input[j] == 'z')
                    pos = j;

            if(pos == -1)
                formula.z = 0;
            else if(pos == 0)
                formula.z = 1;
            else if(input[pos - 1] == '+')
                formula.z = 1;
            else
                formula.z = -1;


            pos = -1;
            formula.frac = 0;
            for(unsigned int j = 0; j < input.size(); j++)
                if(input[j] == '/')
                    pos = j;

            if(pos != -1)
            {
                if(pos == 0)
                     cout << "this is problematic file:" << __FILE__ << " " << __LINE__ << endl;
                unsigned int i;
                string numer;
                double numerator = 0;
                int sign = -1;
                for(unsigned int j = 0; j < input.size(); j++)
                {
                    if(input[j] == '/')
                        break;
                    else if(isdigit(input[j]) == true)
                        numer.push_back(input[j]);
                    else
                        sign = j;
                }
                numerator = atof(numer.c_str());
                if(sign != -1 && input[sign] == '-')
                    numerator = -numerator;



                numer.resize(0);
                i = pos + 1;
                double denominator = 0;


                while(i < input.size() && isdigit(input[i]) == true)
                {
                    numer.push_back(input[i]);
                    i = i + 1;
                }
                denominator = atof(numer.c_str());
                formula.frac = numerator/denominator;
            }
            else
            {
                pos = -1;
                for(unsigned int j = 0; j < input.size(); j++)
                    if(isdigit(input[j]) == true)
                        pos = j;
                if(pos == -1)
                {
                    formula.frac = 0;
                    return;
                }
                cout << "Shouldn't get here : " << __FILE__ << " " << __LINE__ << endl;

                if(input[pos] != 1)
                    cout << "Problem file: " << __FILE__ << " " << __LINE__ << endl;
                else if(pos != 0)
                {
                    if(input[pos-1] == '-')
                        formula.frac = -1;
                    else
                        formula.frac = 1;
                }
                else
                    formula.frac = 1;
            }

        }


        Vec3d getSymmetry(Vec3d input, Vec3d a_unit, Vec3d b_unit, Vec3d c_unit)
        {
            Vec3d answer;
            vector<Vec3d> hold;
            /*
            answer = x_axis.x*input.x*a_unit + x_axis.y*input.y*b_unit +
                     x_axis.z*input.z*c_unit + x_axis.frac*a_unit +

                     y_axis.x*input.x*a_unit + y_axis.y*input.y*b_unit +
                     y_axis.z*input.z*c_unit + y_axis.frac*b_unit +

                     z_axis.x*input.x*a_unit + z_axis.y*input.y*b_unit +
                     z_axis.z*input.z*c_unit + z_axis.frac*c_unit;
            */
            answer = (x_axis.x*input.x + x_axis.y*input.y +
                     x_axis.z*input.z + x_axis.frac)*a_unit +

                     (y_axis.x*input.x + y_axis.y*input.y +
                     y_axis.z*input.z + y_axis.frac)*b_unit +

                     (z_axis.x*input.x + z_axis.y*input.y +
                     z_axis.z*input.z + z_axis.frac)*c_unit;
            return answer;
        }
};


/**
 * @brief checkInside checks if the pos is inside the Unit_Cell, also generates
 *                    a map of points that are displaces by basis vectors but are still
 *                    inside a unit cell
 * @param pos input point
 * @param cell input Unit_Cell
 * @return
 */
vector<Vec3d> checkInside(Vec3d pos, Unit_Cell &cell)
{
    Vec3d newpos;
    Vec3d change;
    double array[3] = {-1.0, 0, 1.0};
    vector<Vec3d> answer;
    Vec3d a_unit = cell.getA();
    Vec3d b_unit = cell.getB();
    Vec3d c_unit = cell.getC();

    for(unsigned int i = 0; i < 3; i++)
    {
        for(unsigned int j = 0; j < 3; j++)
        {
            for(unsigned int k = 0; k < 3; k++)
            {
                change = a_unit*array[i] + b_unit*array[j] + c_unit*array[k];
                newpos = pos + change;
                if(cell.isInside(newpos) == true)
                {
                    answer.push_back(newpos);
                }
            }
        }
    }

    return answer;
}

/*
 * Unit Cell is filled by atoms, from all sides
 */
void fill_Unit_Cell(Unit_Cell &cell)
{
    Vec3d pos;
    double cell_size = cell.backup.size();


    for(unsigned int i = 0; i < cell_size; i++)
    {
        pos = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);
        vector<Vec3d> options = checkInside(pos, cell);
        for(unsigned int j = 0; j < options.size(); j++)
        {
            bool isfree = true;
            for(unsigned int k = 0; k < cell.backup.size(); k++)
            {
                Vec3d atom = Vec3d(cell.backup[k].x, cell.backup[k].y, cell.backup[k].z);
                if ((options[j] - atom).norm() < 0.01)
                {
                    isfree = false;
                    break;
                }
            }

            if (isfree == true)
            {
                cell.add_atom(cell.backup[i].type, options[j].x, options[j].y, options[j].z);
            }

        }

    }
}

/**
 * @brief remove_ALL_OUTSIDE_CELL atoms outside the basis vectors of the given Unit_Cell
 *                                are removed.
 * @param cell
 */
void remove_ALL_OUTSIDE_CELL(Unit_Cell &cell)
{
    Vec3d pos;
    vector<Vec3d> position;
    vector<string> name;


    for(unsigned int i = 0; i < cell.backup.size(); i++)
    {
        pos = Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z);

        if (cell.isInside(pos) == true)
        {
            name.push_back(cell.backup[i].type);
            position.push_back(Vec3d(cell.backup[i].x, cell.backup[i].y, cell.backup[i].z));
        }
    }

    cell.backup.resize(0);
    for(unsigned int k = 0; k < name.size(); k++)
    {
        cell.add_atom(name[k], position[k].x, position[k].y, position[k].z);
    }

}


/**
 * @brief read_CIF_FILE reads the input cif file
 * @param cell the Unit_Cell that the information about from the cif file is dumped to.
 * @param input_name name of the input cif file, must contain "*.cif".
 * @return true if everything worked, false otherwise
 */
bool read_CIF_FILE(Unit_Cell &cell, string input_name)
{
    ifstream cif_file;
    string sname(GetCurrentWorkingDir());
    cif_file.open ((sname + "/" + input_name).c_str());

    if(cif_file.is_open() == false)
    {
        cout << "Problem in file" + sname + ", File could not be found";
        return false;
    }

    string line;
    vector<string> token_line;
    vector<string> entire_file;
    while(getline(cif_file, line))
    {
        for(unsigned int j = 0; j < line.size(); j++)
            if(line[j] != ' ' || line[j] != '\t')
            {
                entire_file.push_back(line);
                break;
            }
    }
    cif_file.close();

    double a_ = 0;
    double b_ = 0;
    double c_ = 0;

    double alpha = 0;
    double beta = 0;
    double gamma = 0;
    bool openbabel = false;
    vector<Symmetry> all_symmetry;

    string name;
    unsigned int i;
    int start_params = 0;
    for(i = 0; i < entire_file.size(); i++)
    {
        line = entire_file[i];
        bool content = false;
        for(unsigned int j = 0; j < entire_file[i].size(); j++)
            if(entire_file[i][j] != ' ' || entire_file[i][j] != '\t')
                content = true;
        if(content == false)
            continue;
        else if(entire_file[i].find("_cell_length_a") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            a_ = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_cell_length_b") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            b_ = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_cell_length_c") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            c_ = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_cell_angle_alpha") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            alpha = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_cell_angle_beta") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            beta = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_cell_angle_gamma") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            gamma = atof(token_line[0].c_str());
        }
        else if (entire_file[i].find("_chemical_name_structure_type") != string::npos)
        {
            token_line = tokenizer(line);
            token_line = tokenizer(token_line[1], "(");
            name = token_line[0].c_str();
        }
        else if (entire_file[i].find("openbabel")  != string::npos)
        {
            openbabel = true;

        }

        else if(entire_file[i].find("_symmetry_equiv_pos_as_xyz") != string::npos
                && openbabel == false)
        {


            i = i + 1;

            while(entire_file[i].find("_") == string::npos)
            {

                vector<string> token = tokenizer(entire_file[i], "'");
                if(token.size() > 1)
                    all_symmetry.push_back(Symmetry(token[1]));
                else
                    all_symmetry.push_back(Symmetry(token[0]));


                i = i + 1;
            }


        }
        else if(entire_file[i].find("_atom_site_label") != string::npos ||
                entire_file[i].find("_atom_site_type_symbol") != string::npos)
        {

            int k = i;
            while(k > 0 && (entire_file[k].find("loop_") == string::npos))
            {
                k = k - 1;
            }
            start_params = k + 1;
        }
    }

    gamma = gamma*M_PI/180.0;
    beta = beta*M_PI/180.0;
    alpha = alpha*M_PI/180.0;

    cell.geometry.zero_unit = Vec3d(0, 0, 0);
    cell.geometry.a_unit = Vec3d(a_, 0, 0);
    cell.geometry.b_unit = Vec3d(b_*cos(gamma), b_*sin(gamma), 0);
    double cx = c_*cos(beta);
    double cy = (c_*cos(alpha) - c_*cos(beta)*cos(gamma))/sin(gamma);
    double cz = sqrt(c_*c_ - cx*cx - cy*cy);
    cell.geometry.c_unit = Vec3d(cx, cy, cz);
    cell.geometry.center = cell.geometry.a_unit/2.0 + cell.geometry.b_unit/2.0 + cell.geometry.c_unit/2.0;
    cell.geometry.isSet = true;


    vector<string> params;
    int start_atoms;
    for(unsigned int m = start_params; m < entire_file.size(); m++)
    {
        if(entire_file[m].find("_") != string::npos)
            params.push_back(entire_file[m]);
        else
        {
            start_atoms = m;
            break;
        }
    }

    int start_coordinates;
    bool fractional = true;

    for(unsigned int m = 0; m < params.size(); m++)
    {
        if(params[m].find("_atom_site_fract_x") != string::npos)
        {
            fractional = true;
            start_coordinates = m;
            break;
        }

        if(params[m].find("_atom_site_Cartn_x") != string::npos)
        {
            start_coordinates = m;
            fractional = false;
            break;
        }
    }

    vector<string> names;
    vector<Vec3d> positions;
    for(unsigned int m = start_atoms; m < entire_file.size(); m++)
    {
        if(entire_file[m].find("#") != string::npos ||
                entire_file[m].find("loop_") != string::npos)
        {
            break;
        }

        vector<string> arrayed_line = tokenizer(entire_file[m], " ");
        string atom_name = "";
        for(unsigned int f = 0; f < arrayed_line[0].length(); f++)
        {
            if (isdigit(arrayed_line[0][f]) == false)
            {
                atom_name = atom_name + arrayed_line[0][f];
            }
        }


        double x = atof((tokenizer(arrayed_line[start_coordinates], "(")[0]).c_str());
        double y = atof((tokenizer(arrayed_line[start_coordinates + 1], "(")[0]).c_str());
        double z = atof((tokenizer(arrayed_line[start_coordinates + 2], "(")[0]).c_str());
        positions.push_back(Vec3d(x, y, z));
        names.push_back(atom_name);
    }

    Vec3d a_unit = cell.getA();
    Vec3d b_unit = cell.getB();
    Vec3d c_unit = cell.getC();

    for(unsigned int m = 0; m < positions.size(); m++)
    {

        if(fractional == false)
        {
            cell.add_atom(names[m], positions[m].x, positions[m].y, positions[m].z);
        }
        else
        {
            Vec3d result = positions[m].x*a_unit + positions[m].y*b_unit + positions[m].z*c_unit;
            cell.add_atom(names[m], result.x, result.y, result.z);
        }
    }



    for(unsigned int d = 0; d < positions.size(); d++)
    {
        Vec3d pos;
        double diag = (a_unit + b_unit + c_unit).norm();
        for(unsigned int k = 0; k < all_symmetry.size(); k++)
        {
            pos = all_symmetry[k].getSymmetry(positions[d], a_unit, b_unit, c_unit);
            Vec3d test;
            bool too_close = false;
            for(unsigned int m = 0; m < cell.backup.size(); m++)
            {
                test = Vec3d(cell.backup[m].x, cell.backup[m].y, cell.backup[m].z);
                if ((pos - test).norm() < diag/1000.0)
                {
   //cout << __FILE__ << " " << __LINE__ << endl;
                    too_close = true;
                    break;
                }

            }

            if(too_close == false)
            {
                cell.add_atom(names[d], pos.x, pos.y, pos.z);
            }
        }
    }
    fill_Unit_Cell(cell);
    remove_ALL_OUTSIDE_CELL(cell);
    cell.translate(5.0, -3.8, 0);
    addStandardColor(cell.backup);
    bool correction_factors = true;
    addRadius(cell.backup, "Van-der-Waals", correction_factors);
    cell.active = true;
    cell.setCenter();
    return true;

}
