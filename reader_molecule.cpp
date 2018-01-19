#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include "header.h"
#include <iomanip>
#include <sstream>
#include <QDebug>


#define SSTR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()


/**
 * @brief reader_gregory_files outdated method to read saved text file
 * @param cell input array of Unit_Cells
 * @param input name of the file
 */
void reader_gregory_files(vector<Unit_Cell> &cell, string input)
{

	vector<string> arrayed_line;
	ifstream read(input.c_str());
	string line;
	

	if(!read.is_open())
	{
		cout << "This file could not be opened: " << input << endl;	
		exit(1);
	}	
	
	   //reads the words 'nuclear'	
	getline(read, line);
	bool stop = false;
	

	for(int i = 0; stop == false; i++)
	{
		stop = true;
		Unit_Cell temp;
		cell.push_back(temp);
	
		   //Formula: could be empty
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		if(arrayed_line.size() > 1)  cell[i].chemical_type = arrayed_line[1];

		   //Active:
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		cell[i].active = atoi(arrayed_line[1].c_str());

		   //Edge:
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');


		   //Plane:
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');	
		int number_of_planes = atoi(arrayed_line[1].c_str());
		for(int j = 0; j < number_of_planes; j++)
		{
			Plane hold;
			getline(read, line);
			arrayed_line = tokenizer(line, ' ');
			cell[i].plane.push_back(hold);	



                int broncos = 2;
			while(arrayed_line[broncos].compare("Color:") != 0)
			{
				broncos += 1;
			}


			cell[i].plane[j].paint[0] = atof(arrayed_line[broncos + 1].c_str());
			cell[i].plane[j].paint[1] = atof(arrayed_line[broncos + 2].c_str());
			cell[i].plane[j].paint[2] = atof(arrayed_line[broncos + 3].c_str());
			cell[i].plane[j].selected = atoi(arrayed_line[broncos + 7].c_str());
			
			getline(read, line);
			vector <Vec3d> all_edges;

		
			while(line.find("clicked:"))
			{
				
				arrayed_line = tokenizer(line, ' ');
				all_edges.push_back(Vec3d(
							atof(arrayed_line[2].c_str()),
							atof(arrayed_line[3].c_str()),
							atof(arrayed_line[4].c_str())));
				getline(read, line);
			}
	
			cell[i].plane[j].edge = all_edges;
		}  	
		if(number_of_planes < 1) getline(read, line);
		

		   //Clicked atom: using the line string we discovered previously
		arrayed_line = tokenizer(line, ' ');	
		int size = atoi(arrayed_line[1].c_str());
		if (size > 0)
		{
			vector<int> temp;
			temp.resize(size);
			getline(read, line);
			arrayed_line = tokenizer(line, ' ');
			for(unsigned int j = 0; j < temp.size(); j++)
			{
				temp[j] = atoi(arrayed_line[j].c_str());
			}
		
			cell[i].clicked_atom = temp;
		}
		
		
		
		   //Geometry
		getline(read, line);
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
   //cell[i].geometry.zero = atoi(arrayed_line[1].c_str());
   //cell[i].geometry.a = atoi(arrayed_line[3].c_str());
   //cell[i].geometry.b = atoi(arrayed_line[5].c_str());

   //cell[i].geometry.c = atoi(arrayed_line[7].c_str());
		cell[i].geometry.alpha = atof(arrayed_line[9].c_str());

		cell[i].geometry.beta = atof(arrayed_line[11].c_str());
		cell[i].geometry.gamma = atof(arrayed_line[13].c_str());

		cell[i].geometry.polygons = atoi(arrayed_line[15].c_str());
		cell[i].geometry.center = Vec3d(atof(arrayed_line[17].c_str()), atof(arrayed_line[18].c_str()), atof(arrayed_line[19].c_str()));
		
	
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		cell[i].geometry.zero_unit.x = atof(arrayed_line[1].c_str());
		cell[i].geometry.zero_unit.y = atof(arrayed_line[2].c_str());
		cell[i].geometry.zero_unit.z = atof(arrayed_line[3].c_str());
		
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		cell[i].geometry.a_unit.x = atof(arrayed_line[1].c_str());
		cell[i].geometry.a_unit.y = atof(arrayed_line[2].c_str());
		cell[i].geometry.a_unit.z = atof(arrayed_line[3].c_str());
		
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		cell[i].geometry.b_unit.x = atof(arrayed_line[1].c_str());
		cell[i].geometry.b_unit.y = atof(arrayed_line[2].c_str());
		cell[i].geometry.b_unit.z = atof(arrayed_line[3].c_str());
		
		getline(read, line);
		arrayed_line = tokenizer(line, ' ');
		cell[i].geometry.c_unit.x = atof(arrayed_line[1].c_str());
		cell[i].geometry.c_unit.y = atof(arrayed_line[2].c_str());
		cell[i].geometry.c_unit.z = atof(arrayed_line[3].c_str());
		
		
		   //Atoms:
		getline(read, line);   //Words Atom
		vector<Atom> backup;
		
		while(getline(read, line))
		{
			if(line.find("nuclear") == 0)
			{
				stop = false;
				break;
			}
			
			else if(line.find("Additions") == 0)
			{
				stop = true;
				break;
			}

			else if(line.find("end") == 0)
			{
				stop = true;
				break;
			}
				
	

			Atom store;
			arrayed_line = tokenizer(line, ' ');
			store.type = arrayed_line[1];
			store.x = atof(arrayed_line[2].c_str());
			store.y = atof(arrayed_line[3].c_str());
			store.z = atof(arrayed_line[4].c_str());


			vector<int> bonds;
			store.bonds = bonds;

			int track = 6;
			while(arrayed_line[track].compare("Color:") != 0)
			{
				
				store.bonds.push_back(atoi(arrayed_line[track].c_str()));
				track += 1;
			}





			store.paint[0] = atof(arrayed_line[track + 1].c_str());
			store.paint[1] = atof(arrayed_line[track + 2].c_str());
			store.paint[2] = atof(arrayed_line[track + 3].c_str());



			store.visible = atoi(arrayed_line[track + 5].c_str());
			store.clicked = atoi(arrayed_line[track + 7].c_str());
			
			store.radius = atof(arrayed_line[track + 9].c_str());
			backup.push_back(store);
		}
		cell[i].backup = backup;
	}	


	if(line.find("Additions") == 0)
	{
		for(unsigned int i = 0; i < cell.size(); i++)
		{


			getline(read, line);		
			arrayed_line = tokenizer(line, ' ');
            if(arrayed_line[2].find("yes") == 0) cell[i].geometry.polygons = true;
            else cell[i].geometry.polygons = false;
				
			if(arrayed_line[4].find("yes") == 0) cell[i].geometry.toggle_f = true;
			else cell[i].geometry.toggle_f = false;
			
			if(arrayed_line.size() < 6) continue;
			
            cell[i].geometry.polygon_shade = atof(arrayed_line[6].c_str());
				
	
		}

	}
		
	read.close();



}

/*
 * Prints out the unit cell in an xyz format
*/

void print_cell(Unit_Cell &cell)
{
	ofstream myfile;
	myfile.open ("example.txt");
	myfile << "nuclear " << '\n';
	for (unsigned int i = 0; i < cell.backup.size(); i++)
	{
		myfile << cell.backup[i].type << " " << cell.backup[i].x << " " << cell.backup[i].y << " " << cell.backup[i].z << '\n';
	}
	
	myfile.close();
}



void private_print_cell(const Unit_Cell &cell, ofstream &myfile)
{
	
	myfile << "nuclear " << '\n';
	myfile << "Formula: " << cell.chemical_type << endl;
	myfile << "Active: " << cell.active << endl;
    myfile << "Edge: " << 0 << " " << 0 << endl;
	myfile << "Plane: " << cell.plane.size() << endl;
	
	
	for (unsigned int i = 0; i < cell.plane.size(); i++)
	{
		myfile << i << " atoms: ";
		myfile << " Color: " << setw(5) << round(cell.plane[i].paint[0], 4)
							 << setw(5) << round(cell.plane[i].paint[1], 4)
							 << setw(5) << round(cell.plane[i].paint[2], 4);
        myfile << " by_number: " << 0 << "  selected: " << cell.plane[i].selected << endl;

		for (unsigned int j = 0;  j < cell.plane[i].edge.size(); j++)
		{
			myfile << j << ". " << "Edge: " << setw(9) << round(cell.plane[i].edge[j].x, 4)
							<< setw(9) << round(cell.plane[i].edge[j].y, 4)
							<< setw(9) << round(cell.plane[i].edge[j].z, 4) << endl;
		}
	}
	
	
	
	myfile << "clicked: " << cell.clicked_atom.size() << endl;
	
	for ( unsigned int i = 0; i < cell.clicked_atom.size(); i++)
	{
		myfile << setw(4) << cell.clicked_atom[i];
	}
	if(cell.clicked_atom.size() > 0) myfile << endl;
	
	myfile << "Geometry: " << endl;
    myfile << "zero: " << 0 << " a: " << 0 << " b: " << 0 << " c: " << 0;
	myfile << " alpha: " << cell.geometry.alpha << " beta: " << cell.geometry.beta << " gamma: " << cell.geometry.gamma
		   << " polygons: " << cell.geometry.polygons <<
		      "  Center: " << setw(10) << round(cell.geometry.center.x, 4)
		                   << setw(10) << round(cell.geometry.center.y, 4)
				   << setw(10) << round(cell.geometry.center.z, 4) << endl;
	myfile << " Zero: " << setw(10) << round(cell.geometry.zero_unit.x, 4)
			    << setw(10) << round(cell.geometry.zero_unit.y, 4)
			    << setw(10) << round(cell.geometry.zero_unit.z, 4) << endl;
	
	myfile << " A: " << setw(10) << round(cell.geometry.a_unit.x, 4)
			    << setw(10) << round(cell.geometry.a_unit.y, 4)
			    << setw(10) << round(cell.geometry.a_unit.z, 4) << endl;
	
	myfile << " B: " << setw(10) << round(cell.geometry.b_unit.x, 4)
			    << setw(10) << round(cell.geometry.b_unit.y, 4)
			    << setw(10) << round(cell.geometry.b_unit.z, 4) << endl;
	 			
	myfile << " C: " << setw(10) << round(cell.geometry.c_unit.x, 4)
			    << setw(10) << round(cell.geometry.c_unit.y, 4)
			    << setw(10) << round(cell.geometry.c_unit.z, 4) << endl;
		   									
	
	myfile << "Atoms" << endl;
	for (unsigned int i = 0; i < cell.backup.size(); i++)
	{
		myfile << setw(3) << i << ". " << setw(3) << cell.backup[i].type
		 << setw(11) << round(cell.backup[i].x, 4)
		 << setw(11) << round(cell.backup[i].y, 4)
		 << setw(11) << round(cell.backup[i].z, 4)
		 << "  Bonds: ";
		
		
			for(unsigned int j = 0; j < cell.backup[i].bonds.size(); j++)
			{
				myfile << setw(4) << cell.backup[i].bonds[j] << " ";
			}
		
		   //myfile << '\n';
		
				 myfile << "    Color: " << setw(7) << round(cell.backup[i].paint[0], 3)
								 << setw(7) << round(cell.backup[i].paint[1], 3)
								 << setw(7) << round(cell.backup[i].paint[2], 3);
								
				 myfile << "  Visible: " << cell.backup[i].visible <<
						   "  Clicked: " << cell.backup[i].clicked <<
						   "  Radius: "  << cell.backup[i].radius << endl;
	}
	
		
}	












