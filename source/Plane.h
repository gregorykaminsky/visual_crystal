#include "Vec3d.h"

double round(double x, int number);
double round_to_digits(double value, int digits);
vector<string> tokenizer(string line, string separator);
Vec3d ArbitraryRotate(Vec3d point, double theta, Vec3d axis);


#define SSTR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()


class Plane
{
    public:
        Plane(){}
        vector<Vec3d> edge;   //edges of the plane
        double paint[3];   //color of the plane
        bool selected;   //specifies whether the plane is selected

/**
         * @brief copy creates a copy of the Plane object
         * @return the copy of the Plane object.
         *         This copy contains no shared arrays or objects
         */
        Plane copy()
        {
            Plane copy;
            copy.paint[0] = paint[0];
            copy.paint[1] = paint[1];
            copy.paint[2] = paint[2];
            copy.edge.resize(edge.size());
            for(unsigned int i = 0; i < edge.size(); i++)
            {
                copy.edge[i] = edge[i];
            }

            copy.selected = selected;
            return copy;
        }

/**
         * @brief printPlane converts a Plane object into a collection of strings
         * @param number the position of the given plane in the array of planes
         * @return the vector string that contains all the information of this Plane object
         */
        vector<string> printPlane(int number)
        {
            vector<string> output;
            output.push_back("_begin_Plane  " + SSTR(number));
            for(unsigned int i = 0; i < edge.size(); i++)
            {
                stringstream one_edge;
                one_edge << SSTR(i) + ". _edge" << setw(15) << round_to_digits(edge[i].x, 7)\
                        << setw(15) << round_to_digits(edge[i].y, 7)\
                        << setw(15) << round_to_digits(edge[i].z, 7);
                output.push_back(one_edge.str());
            }

            stringstream color;
            color << "Color(Red:" << setprecision(3) << setw(4) << paint[0]\
            << ", Blue:" << setprecision(3) << setw(4) << paint[1]\
            << ", Green:" << setprecision(3) << setw(4) << paint[2] << ")";
            output.push_back(color.str());
            output.push_back("Active  " + SSTR(selected));
            output.push_back("_end_Plane");
            return output;
        }

/**
         * @brief readPlane converts the vector of strings into a Plane object
         * @param input a vector of strings that contain the information
         *        to create a vector object.
         */
        void readPlane(vector<string> input)
        {
            for(unsigned int i = 0; i < input.size(); i++)
            {
                if(input[i].find("_edge") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], "  ");
                    Vec3d one_edge;
                    one_edge.x = atof(tokens[2].c_str());
                    one_edge.y = atof(tokens[3].c_str());
                    one_edge.z = atof(tokens[4].c_str());
                    edge.push_back(one_edge);
                }
                else if(input[i].find("Active") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], " ");
                    selected = atoi(tokens[1].c_str());
                }

                else if(input[i].find("Color") != string::npos)
                {
                    vector<string> tokens = tokenizer(input[i], ":,");
                    paint[0] = atof(tokens[1].c_str());
                    paint[1] = atof(tokens[3].c_str());
                    paint[2] = atof(tokens[5].c_str());
                }

                else if(input[i].find("_end_Plane") != string::npos)
                    return;
            }
        }

/**
         * @brief translate edges of the plane are translated
         * @param move input vector specifying the amount of translation
         */
        void translate(Vec3d move)
        {
            for(unsigned int i = 0; i < edge.size(); i++)
                edge[i] = edge[i] + move;
        }

/**
         * @brief rotate rotates the edges of the plane
         * @param angle of rotation
         * @param direction vector about which the plane is rotated
         */
        void rotate(double angle, Vec3d direction)
        {
            for(unsigned int i = 0; i < edge.size(); i++)
                edge[i] = ArbitraryRotate(edge[i], angle, direction);
        }
};
