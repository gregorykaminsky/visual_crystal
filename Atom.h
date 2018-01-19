#include "Vec3d.h"
#include <sstream>
#include <iomanip>

#ifndef __ATOM_H_INCLUDE
#define __ATOM_H_INCLUDE
double round(double x, int number);
double round_to_digits(double value, int digits);
vector<string> tokenizer(string line, string separator);

#define SSTR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()


class Atom
{
    public:
        Atom()
        {
            visible = true;   //atom is visible
            clicked = false;   //atom has not been clicked yet
        }
        double x, y, z;   //coordinates of the atom


        /*
         * specifies whether an atom is visible or not
         * when an atom is deleted, it is made invisible
         * when the 'save' is invoked, all the deleted atoms are permanently removed
         */
        bool visible;


        bool clicked;   //specifies whether an atom has been clicked or not
        vector<int> bonds;   //specifies the bonds of the atom
        string type;   //type of the atom
        double paint[3];   //color of the atom
        double radius;   //radius of the atom

/**
         * @brief copy of the atom is created
         * @return this copy is returned
         */
        Atom copy()
        {
            Atom copy;
            copy.visible = visible;
            copy.clicked = clicked;
            copy.x = x;
            copy.y = y;
            copy.z = z;
            copy.type = type;
            copy.radius = radius;
            copy.paint[0] = paint[0];
            copy.paint[1] = paint[1];
            copy.paint[2] = paint[2];

            copy.bonds.resize(bonds.size());
            for(unsigned int i = 0; i < bonds.size(); i++)
            {
                copy.bonds[i] = bonds[i];
            }
            return copy;
        }



/**
         * @brief printAtom converts atom information into a string
         * @param number position of the atom in the unit cell 'backup' array
         * @return string specifying information about the atom is returned
         */
        string printAtom(int number)
        {
            string output;
            stringstream stream;
            string out_bonds;

            for (unsigned int i = 0; i < bonds.size(); i++)
            {
                out_bonds = out_bonds + SSTR(bonds[i]) + ",";
            }

            int number_space;
            int type_space;

            if(number > 999)
            {
                number_space = 6;
                cout << "check your file, there should not be this many atoms in the unit cell" << endl;
                cout << "number of atoms: " << number << endl;
            }

            else
                number_space = 4;

            if(type.size() > 3)
                type_space = type.size() + 1;
            else
                type_space = 4;

            stream << setw(number_space) << SSTR(number) + "." << setw(type_space) << type\
                   << setw(13) << round_to_digits(x, 7)\
                   << setw(13) << round_to_digits(y, 7)\
                   <<  setw(13) << round_to_digits(z, 7) << ";"\

                   << "    Color(Red:" << setprecision(3) << setw(4) << paint[0]\
                   << ", Blue:" << setprecision(3) << setw(4) << paint[1]\
                   << ", Green:" << setprecision(3) << setw(4) << paint[2] << ");"\

                   << "    Radius" << setw(11) << round_to_digits(radius, 6) << ";"\

                   << "    Bonds[" <<  out_bonds << "];";
            output = stream.str();
            return output;
        }

/**
         * @brief readAtom in input string is parsed into an atom
         * @param input_atom an input string to be parsed into an atom
         */
        void readAtom(string input_atom)
        {
            vector<string> tokens = tokenizer(input_atom, " ");
            type = tokens[1];
            x = atof(tokens[2].c_str());
            y = atof(tokens[3].c_str());
            z = atof(tokens[4].c_str());

            tokens = tokenizer(input_atom, ":,");
            paint[0] = atof(tokens[1].c_str());
            paint[1] = atof(tokens[3].c_str());
            paint[2] = atof(tokens[5].c_str());

            tokens = tokenizer(input_atom, "[]");
            vector<string> all_bonds;

            if(tokens.size() > 2)
                all_bonds = tokenizer(tokens[1], ", ");

            for(unsigned int i = 0; i < all_bonds.size(); i++)
            {
                int bond = atoi(all_bonds[i].c_str());
                bonds.push_back(bond);
            }

            tokens = tokenizer(input_atom, ";");
            tokens = tokenizer(tokens[2], " ");
            radius = atof(tokens[1].c_str());
            visible = true;
            clicked = false;
        }


};



#endif   //ATOM_H
