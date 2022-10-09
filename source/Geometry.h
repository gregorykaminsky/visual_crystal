#include "Vec3d.h"

double round(double x, int number);
double round_to_digits(double value, int digits);
vector<string> tokenizer(string line, string separator);
Vec3d ArbitraryRotate(Vec3d point, double theta, Vec3d axis);

#define SSTR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()




/*
 * Structure of the unit cell,
 * some variables here are no longer used,
 * I cannot remember which ones, though.
 * make_copy file contains a method to make a copy of 'Geometry'
 */

class Geometry
{
    public:
        Geometry()
        {
            alpha = 0;
            beta = 0;
            gamma = 0;
            isSet = false;
            toggle_f = true;
            polygon_shade = 0;
            visible_bonds = true;
        }

   //angles that define the Unit_Cell
        double alpha;
        double beta;
        double gamma;

   //coordinates that define the basis vectors of the Unit_Cell
   // a = a_unit - zero_unit   and so on.
        Vec3d a_unit;
        Vec3d b_unit;
        Vec3d c_unit;
        Vec3d zero_unit;

        Vec3d center;   //center of the Unit_Cell

        bool isSet;   //has the geometry been set
        bool polygons;   //should polygons appear or not
        bool toggle;   //should cell boundaries appear or not

   //neccesary for upPlane() and downPlane() method in file 'opengl_keypress.cpp'
        bool toggle_f;
        double polygon_shade;   //the shading of the polygons
        bool visible_bonds;


        Geometry copy()
        {
            Geometry copy;
            copy.alpha = alpha;
            copy.beta = beta;
            copy.gamma = gamma;
            copy.a_unit = a_unit;
            copy.b_unit = b_unit;
            copy.c_unit = c_unit;
            copy.zero_unit = zero_unit;
            copy.center = center;
            copy.isSet = isSet;
            copy.polygons = polygons;
            copy.toggle_f = toggle_f;
            copy.toggle = toggle;
            copy.polygon_shade = polygon_shade;
            return copy;
        }

        vector<string> printGeometry()
        {
            vector<string> output;
            output.push_back("_begin_geometry");
            output.push_back("alpha = " + SSTR(alpha) + " deg");
            output.push_back("beta = " + SSTR(beta) + " deg");
            output.push_back("gamma = " + SSTR(gamma) + " deg");

            stringstream vec;
            vec << "zero_unit =" << setw(13) << round_to_digits(zero_unit.x, 6)\
                                  << setw(13) << round_to_digits(zero_unit.y, 6)\
                                  << setw(13) << round_to_digits(zero_unit.z, 6) << "    point where basis vectors start";
            output.push_back(vec.str());
            vec.str("");

            vec << "a_unit = " << setw(13) << round_to_digits(a_unit.x, 6)\
                                  << setw(13) << round_to_digits(a_unit.y, 6)\
                                  << setw(13) <<  round_to_digits(a_unit.z, 6);
            output.push_back(vec.str());
            vec.str("");

            vec << "b_unit = " << setw(13) << round_to_digits(b_unit.x, 6)\
                                  << setw(13) << round_to_digits(b_unit.y, 6)\
                                  << setw(13) << round_to_digits(b_unit.z, 6);
            output.push_back(vec.str());
            vec.str("");

            vec << "c_unit = " << setw(13) << round_to_digits(c_unit.x, 6)\
                                  << setw(13) << round_to_digits(c_unit.y, 6)\
                                  << setw(13) << round_to_digits(c_unit.z, 6);
            output.push_back(vec.str());
            vec.str("");

            vec << "center = " << setw(13) << round_to_digits(center.x, 6)\
                                  << setw(13) << round_to_digits(center.y, 6)\
                                  << setw(13) << round_to_digits(center.z, 6) << "   center of the unit cell";
            output.push_back(vec.str());
            vec.str("");

            output.push_back("isSet: " + SSTR(isSet)  + "     Specifies whether geometry has been set");
            output.push_back("polygons: " + SSTR(polygons)    + "     Controls the polygons ");
            output.push_back("toggle: " + SSTR(toggle) + "     Necessary for '(' && ')' commands ");
            output.push_back("toggle_f: " + SSTR(toggle_f) +   "     Specifies the existance of cell boundary lines");
            output.push_back("polygon_shade = " + SSTR(polygon_shade) +   "     Specifies the shading on the polygons");

            output.push_back("_end_geometry");
            return output;
        }

        void readGeometry(vector<string> input)
        {
            for(unsigned int i = 0; i < input.size(); i++)
            {
                if(input[i].find("alpha") != string::npos)
                {
                    string alpha = tokenizer(input[i], " =")[1];
                    alpha = atof(alpha.c_str());
                }

                else if(input[i].find("beta") != string::npos)
                {
                    string beta = tokenizer(input[i], " =")[1];
                    beta = atof(beta.c_str());
                }

                else if(input[i].find("gamma") != string::npos)
                {
                    string gamma = tokenizer(input[i], " =")[1];
                    gamma = atof(gamma.c_str());
                }

                else if(input[i].find("zero_unit") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], " =");
                    zero_unit.x = atof(tokens[1].c_str());
                    zero_unit.y = atof(tokens[2].c_str());
                    zero_unit.z = atof(tokens[3].c_str());
                }

                else if(input[i].find("a_unit") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], " =");
                    a_unit.x = atof(tokens[1].c_str());
                    a_unit.y = atof(tokens[2].c_str());
                    a_unit.z = atof(tokens[3].c_str());
                }

                else if(input[i].find("b_unit") != string::npos)
                {

                    vector<string> tokens = tokenizer(input[i], " =");
                    b_unit.x = atof(tokens[1].c_str());
                    b_unit.y = atof(tokens[2].c_str());
                    b_unit.z = atof(tokens[3].c_str());
                }

                else if(input[i].find("c_unit") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], " =");
                    c_unit.x = atof(tokens[1].c_str());
                    c_unit.y = atof(tokens[2].c_str());
                    c_unit.z = atof(tokens[3].c_str());
                }

                else if(input[i].find("center") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], " =");
                    center.x = atof(tokens[1].c_str());
                    center.y = atof(tokens[2].c_str());
                    center.z = atof(tokens[3].c_str());
                }

                else if(input[i].find("isSet") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ": ");
                    isSet = atoi(tokens[1].c_str());
                }

                else if(input[i].find("polygons:") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ": ");
                    polygons = atoi(tokens[1].c_str());
                }

                else if(input[i].find("toggle:") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ": ");
                    toggle = atoi(tokens[1].c_str());
                }

                else if(input[i].find("toggle_f:") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ": ");
                    toggle_f = atoi(tokens[1].c_str());
                }
                else if(input[i].find("polygon_shade") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ": ");
                    polygon_shade = atoi(tokens[1].c_str());
                }


                else if(input[i].find("_end_geometry") != string::npos)
                    return;
            }
        }

        void translate(Vec3d move)
        {
            center = center + move;
            a_unit = a_unit + move;
            b_unit = b_unit + move;
            c_unit = c_unit + move;
            zero_unit = zero_unit + move;
        }

        void rotate(double angle, Vec3d direction)
        {
            zero_unit = ArbitraryRotate(zero_unit, angle, direction);
            center = ArbitraryRotate(center, angle, direction);

            a_unit = ArbitraryRotate(a_unit, angle, direction);
            b_unit = ArbitraryRotate(b_unit, angle, direction);
            c_unit = ArbitraryRotate(c_unit, angle, direction);
        }


};

