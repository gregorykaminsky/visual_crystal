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
 * Reads files that end with '.xyz'
*/

bool molecule_reader_for_xyz(Unit_Cell &cell, string name)
{
    vector<string> arrayed_line;
    string sname(GetCurrentWorkingDir());
    sname = sname + "/" + name;

    ifstream read(sname.c_str());
    string line;
    if(read.is_open() == false)
        return false;

    while(getline(read, line))
        if(!(line.find("nuclear")))
             break;

    while (getline(read, line))
    {
        arrayed_line = tokenizer(line, ' ');
        double x = atof(arrayed_line[1].c_str());
        double y = atof(arrayed_line[2].c_str());
        double z = atof(arrayed_line[3].c_str());
        cell.add_atom(arrayed_line[0], x, y, z);

    }

    read.close();

    addStandardColor(cell.backup);
    bool correction_factors = true;
    addRadius(cell.backup, "Van-der-Waals Radius", correction_factors);
    cell.active = true;
    cell.setCenter();
    return true;
}

