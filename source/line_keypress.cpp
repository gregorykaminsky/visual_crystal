#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "GL/freeglut.h"
#include "GL/gl.h"
#include <vector>
#include <QGLWidget>
#include "mytextbrowser.h"

#include <sstream>
#include "header.h"
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include <QTextBrowser>
#include "lineedit.h"
#include <ostream>

#include <QFile>
#include <QTextStream>
#include <QString>


#include <sys/types.h>
#include <sys/stat.h>


   //saves the file into a screenshot and a text file.
bool saveAll(MainWindow *window, vector<Unit_Cell> &cell, vector<string> input);

   //saves a screenshot of the image of the openGl window
bool save_screenshot(string filename, unsigned int w, unsigned int h);

   //saves the file into a text file that could be later read.
void saveFile(MainWindow* window, vector<Unit_Cell>& cell_array, string filename);

   //reads a file, returns an array of Unit_Cells and prints notes to the TextEdit window.
bool readFile(MainWindow* window, vector<Unit_Cell> &cell_array, string filename);

   //prints the output of a shell to the TextBrowser window
void outputShell(MainWindow* window, string put);

   //aligns the cell so that the vector connecting the two atoms points up
void alignCell(MainWindow *window, vector<Unit_Cell> &cell_array,
              Manager &manage, vector<string> input);

   //opens a file with name stored in input[1], whatever was opened previously is erased
void openFile(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              string put);

   //imports a file with name stored in input[1], different from openFile command,
   //the previous information is not erased but rather appended.
void importFile(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              vector<string> input);

   //operations on a single specified cell
void cellOperations(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              string input);

   //legacy code, shouldn't be used.
int switchFourVector(int four_vector);

   //prints the explanation corresponding to a key command
void manKeyRead(MainWindow *window, string command);

   //prints the explanation corresponding to a line command
void manLineRead(MainWindow *window, string command);

   //Managers the 'help' and 'man' commands.
void manCommand(MainWindow *window, vector<string> input);
/*
 * Acts on the input from lineEditor widget
 *
 * @param window the main window that all the widgets reside in, can be used to call other widgets
 * @param put the string input to lineEdit widget
*/
void line_keypress(MainWindow *window, string put)
{

    Singleton& the_s = Singleton::getInstance();
    vector<string> bash = tokenizer(put, " ");
    vector<string> input = tokenizer(put, " [](),->;\"");
    the_s.all_keys.push_back(put);
    the_s.lineEdit_input.push_back(put);
    the_s.lineEdit_input.setIndex(the_s.lineEdit_input.size() - 1);
    vector<Unit_Cell> &cell_array = the_s.cell;

    if(put == "")
        return;
    else if(bash[0][0] == '!')
    {
        outputShell(window, put);
    }

    else if(input[0].compare("save") == 0)
    {
        saveAll(window, cell_array, input);
    }

    else if(input[0].compare("align") == 0)
    {
        the_s.stack.push(Container(the_s.cell, the_s.manage));
        alignCell(window, cell_array, the_s.manage, input);
    }

    else if(input[0].compare("open") == 0)
    {
       the_s.stack.push(Container(cell_array, the_s.manage));
       if(input.size() < 2)
       {
            window->printText("Failed Command: " + input[0]);
            window->printText("A name of the file must be a second variable ");
            return;
       }
       openFile(window, cell_array, the_s.manage, input[1]);
    }

    else if(input[0].compare("import") == 0)
    {
       the_s.stack.push(Container(cell_array, the_s.manage));
       importFile(window, cell_array, the_s.manage, input);
    }


    /*
     * Color of the atoms in the unit cell are inverted
     */
    else if (input[0].compare("invert") == 0)
    {
        the_s.stack.push(Container(cell_array, the_s.manage));
        cell_array[the_s.manage.getSelected()].invertColor();
    }

    /*
     *
     * This is not finished
    */
    else if(input[0].find("cell") == 0)
    {
        the_s.stack.push(Container(cell_array, the_s.manage));
        cellOperations(window, cell_array, the_s.manage, put);
    }
    else if(input[0].find("makeBonds") == 0)
    {
        the_s.stack.push(Container(cell_array, the_s.manage));
        string type1 = input[1];
        string type2 = input[2];
        double bond_length = atof(input[3].c_str());
        bool outside;

        if(input.size() > 4 && input[4].find("true") == 0)
           outside = true;
        else
           outside = false;

        makeBonds(cell_array[the_s.manage.getSelected()], type1, type2, bond_length, outside);

    }

    else if(input[0].find("setColor") == 0)
    {
        the_s.stack.push(Container(cell_array, the_s.manage));
        string type = input[1];
        double rbg[3];

        rbg[0] = atof(input[2].c_str());
        rbg[1] = atof(input[3].c_str());
        rbg[2] = atof(input[4].c_str());
        cell_array[the_s.manage.getSelected()].setColor(type, rbg);
    }


    else if(input[0].find("setAtomColor") == 0)
    {
        the_s.stack.push(Container(cell_array, the_s.manage));
        int type = atoi(input[1].c_str());
        double rbg[3];

        rbg[0] = atof(input[2].c_str());
        rbg[1] = atof(input[3].c_str());
        rbg[2] = atof(input[4].c_str());
        cell_array[the_s.manage.getSelected()].setColor(type, rbg);
    }

    else if(input[0].find("eraseBonds") == 0)
    {
        eraseBonds(the_s.cell[the_s.manage.getSelected()]);
    }

    else if(input[0].find("visibleBonds") == 0)
    {

        if(input[1].find("true") != string::npos || input[1].find("True") != string::npos)
            the_s.cell[the_s.manage.getSelected()].geometry.visible_bonds = true;
        else
            the_s.cell[the_s.manage.getSelected()].geometry.visible_bonds = false;

    }

    else if(input[0].compare("all") == 0) {cout << "Does nothing yet" << endl;}
    else if(input[0].compare("merge") == 0) {cout << "Does nothing yet" << endl;}
    else if(input[0].compare("optimize") == 0) {cout << "Does nothing yet" << endl;}

   //this shouldn't be used
    else if(input[0].compare("switch") == 0)
    {
        the_s.four_vector_draw = switchFourVector(the_s.four_vector_draw);
    }

   //man and help commands, prints out the line corresponding to a
   //given command.
    else if(input[0].compare("man") == 0 || input[0].compare("help") == 0)
    {
        manCommand(window, input);
    }
}



/**
 * @brief manCommand managers the man command
 * @param window MainWindow
 * @param input line input broken up into tokens.
 */
void manCommand(MainWindow *window, vector<string> input)
{
    if (input.size() < 2)
    {
        window->printFileToBrowser("man_explained.txt");
        return;
    }

    string line_token = tokenizer(input[1], "\" ")[0];

    if(line_token.size() < 2)
        manKeyRead(window, input[1]);
    else
        manLineRead(window, input[1]);
}


/**
 * @brief manKeyRead prints out the action corresponding to the given key command.
 * @param window the main window
 * @param command input command, in this case it should be a single key.
 */
void manKeyRead(MainWindow *window, string command)
{
    ifstream read2("man_commands.txt");
    string line;
    vector<string> man_commands;
    while(getline(read2, line))
    {
        man_commands.push_back(line);
    }
    read2.close();

    for(unsigned int i = 0; i < man_commands.size(); i++)
    {
        if(man_commands[i].find("current_key") != string::npos)
        {
            vector<string> file_tokens = tokenizer(man_commands[i], " ");
            if(file_tokens[2].find(command) != string::npos)
            {
                for(unsigned int j = i + 1; j < man_commands.size(); j++)
                {
                    if(man_commands[j].find("current_key") != string::npos)
                        return;
                    window->printText(man_commands[j]);
                }

                return;
            }
        }
    }
    window->printText("This key is not used: " + command);
}



/**
 * @brief manLineRead prints out the action corresponding to the given line command.
 * @param window the main window
 * @param command input command, in this case it should be a single key.
 */
void manLineRead(MainWindow *window, string command)
{
    ifstream read2("man_commands.txt");
    string line;
    vector<string> man_commands;
    while(getline(read2, line))
    {
        man_commands.push_back(line);
    }
    read2.close();

    for(unsigned int i = 0; i < man_commands.size(); i++)
    {
        if(man_commands[i].find("line_command ==") != string::npos)
        {
            vector<string> file_tokens = tokenizer(man_commands[i], "== ");
            if(file_tokens[1].find(command) != string::npos)
            {
                window->printText(man_commands[i]);
                for(unsigned int j = i + 1; j < man_commands.size(); j++)
                {
                    if(man_commands[j].find("line_command ==") != string::npos)
                        return;
                    window->printText(man_commands[j]);
                }

                return;
            }
        }
    }
    window->printText("This line command does not exist: " + command);
}

/*
 * Don't use this
*/
int switchFourVector(int four_vector)
{
    if (four_vector == 0)
    {
        return 1;
    }
    else if (four_vector == 1)
    {
        return 2;
    }
    else if (four_vector == 2)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}


/*
 * Saves a screenshot of the screen into a *.tga file
 * Most of this code was taken and modified
 * from an anonymous source on stack exchange.
*/
bool save_screenshot(string filename, unsigned int w, unsigned int h)
{
   //This prevents the images getting padded
   // when the width multiplied by 3 is not a multiple of 4
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  int nSize = w*h*3;
   // First let's create our buffer, 3 channels per Pixel
  char* dataBuffer = (char*)malloc(nSize*sizeof(char));

  if (!dataBuffer) return false;

   // Let's fetch them from the backbuffer
   // We request the pixels in GL_BGR format, thanks to Berzeger for the tip
   glReadPixels((GLint)0, (GLint)0,
        (GLint)w, (GLint)h,
         GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

   //Now the file creation
   FILE *filePtr = fopen(filename.c_str(), "wb");
   if (!filePtr) return false;

   unsigned char rem_w = (unsigned char)(w%256);
   unsigned char rem_h = (unsigned char)(h%256);
   unsigned char div_w = (unsigned char)(w/256);
   unsigned char div_h = (unsigned char)(h/256);


   unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
   unsigned char header[6] = { rem_w, div_w,
                   rem_h, div_h,
                   24,0};
   // write the headers
   fwrite(TGAheader,	sizeof(unsigned char),	12,	filePtr);
   fwrite(header,	sizeof(unsigned char),	6,	filePtr);
   // finally our image data
   fwrite(dataBuffer,	sizeof(GLubyte),	nSize,	filePtr);
   fclose(filePtr);

  return true;
}






   //saves everything on  screen into a text file that could be read from later.
bool saveAll(MainWindow* window, vector<Unit_Cell> &cell, vector<string> input)
{
    if(input.size() < 2)
    {
        window->printText("Failed Command: " + input[0]);
        window->printText("A name of the file must be a second variable ");
        return false;
    }
    vector<string> hold = tokenizer(input[1], '.');
    hold[0] = hold[0];

    /*
    struct stat info;
    if( stat( "Saved_files", &info ) != 0 )
        system("mkdir Saved_files");
    else if( info.st_mode & S_IFDIR );  // S_ISDIR() doesn't exist on my windows
        //printf( "%s is a directory\n",  "Saved_files");
        //means the file exists and nothing should be done.
    else
        system("mkdir Saved_files");
*/
    saveFile(window, cell, hold[0] + ".txt");

    unsigned int width = window->getMyGLWidget()->size().width();
    unsigned int height = window->getMyGLWidget()->size().height();
    save_screenshot(hold[0] + ".tga", width, height);
    return true;
}


void saveFile(MainWindow* window, vector<Unit_Cell>& input_cell_array, string filename)
{
    QString foo = window->getTextEdit()->toPlainText();
    vector<Unit_Cell> cell_array;

    for(unsigned int i = 0; i < input_cell_array.size(); i++)
        cell_array.push_back(input_cell_array[i].copy());


    for(unsigned int j = 0; j < cell_array.size(); j++)
        reduceCell(cell_array[j]);

    ofstream myfile;
    myfile.open (filename.c_str());
    myfile << "FILENAME:  " << filename << endl << endl;
    myfile << foo.toStdString() << endl << endl;
    myfile << "_BEGIN_DATA_FOR  " << filename << endl;
    myfile << "_total_number_of_cells   " << cell_array.size() << endl;
    vector<string> output;
    for(unsigned int i = 0; i < cell_array.size(); i++)
    {
        vector<string> temp = cell_array[i].printCell(i);
        output.insert(output.end(), temp.begin(), temp.end());
    }

    for(unsigned int i = 0; i < output.size(); i++)
        myfile << output[i] << endl;
    myfile << "_END_DATA_FOR  " << filename << endl;
    myfile.close();
}


/**
 * @brief outputShell prints the output of a shell command to the text browser
 * @param window pointer to MainWindow
 * @param put string that calls the shell
 */
void outputShell(MainWindow* window, string put)
{
    window->printText("\n");
    string shove = put.substr(1, string::npos);

    FILE *in;
    char buff[512];

    in = popen(shove.c_str(), "r");
    if(in == NULL)
    {
        window->printText("Failed Command: " + shove);
        return;
    }

    while(fgets(buff, sizeof(buff), in) != NULL)
    { window->getTextBrowser()->insertPlainText(QString::fromUtf8(buff));}
    pclose(in);
}


/**
 * @brief readFile reads the file
 * @param window pointer to MainWindow, necessary to print info to TextEdit
 * @param filename name of the file to be read
 * @return vector collection of Unit_Cell's
 */
bool readFile(MainWindow* window, vector<Unit_Cell> &cell_array, string filename)
{
    vector<string> arrayed_line;
    string line;

    ifstream read(filename.c_str());
    if(!read.is_open())
    {
        window->printText( "This file could not be opened: " + filename);
        cout << "This file could not be opened: " << filename << endl;
        return false;
    }
    int total_number_of_cells;
    while(getline(read, line))
    {
        arrayed_line.push_back(line);
        if(line.find("_total_number_of_cells") != string::npos)
            total_number_of_cells = atoi(tokenizer(line, " ")[1].c_str());
    }
    read.close();
    bool file_corrupt = true;
    for(unsigned int k = 1; k < arrayed_line.size() + 1; k++)
        if(arrayed_line[arrayed_line.size() - k].find("_END_DATA_FOR") != string::npos)
            file_corrupt = false;
    if(file_corrupt == true)
    {
        cout << "The file " << filename << " might be corrupted" << endl;
        cout << "The end of " << filename << " is not as it should be" << endl;
        return false;
    }
    unsigned int count;

    for(count = 0; arrayed_line[count].find("_BEGIN_DATA_FOR") != string::npos; count++)
        window->getTextEdit()->append(arrayed_line[count].c_str());
    for(int i = 0; i < total_number_of_cells; i++)
    {
        Unit_Cell cell;
        vector<string> string_cell;
        for( ; arrayed_line[count].find("_end_Unit_Cell") == string::npos; count++)
            string_cell.push_back(arrayed_line[count]);
        count += 1;
        cell.readCell(string_cell);
        cell_array.push_back(cell);
        string_cell.resize(0);

    }
    return true;

}

/**
 * @brief alignCell the cell is aligned so that the vector connected two selected atoms
 *                  points up
 * @param window the MainWindow where all the widgets exist
 * @param cell_array collection of Unit_Cells
 * @param manage
 * @param input string command
 */
void alignCell(MainWindow *window, vector<Unit_Cell> &cell_array,
              Manager &manage, vector<string> input)
{


    if( manage.getSize() < 2)
    {
         window->printText("Failed Command: " + input[0]);
         window->printText("Need to select more then 1 atom in a unit cell, "
                           "you have " + toString(manage.getSize()) + " selected");
        return;
    }
    Atom first = cell_array[manage.getAtom(0).cell].backup[manage.getAtom(0).clicked];
    Atom second = cell_array[manage.getAtom(1).cell].backup[manage.getAtom(1).clicked];
    Vec3d store = Vec3d(second.x - first.x, second.y - first.y, second.z - first.z);
   //rotate the vector to point to (0, 1, 0) that is what need to be done:
    Vec3d up =  Vec3d(0, 1, 0);
    Vec3d cross = crossProduct(store, up);
    double angle = angleBetween(store, up);

    for(unsigned int i = 0; i < cell_array.size(); i++)
    {
        cell_array[i].rotate_all(angle, cross, Vec3d(second.x, second.y, second.z));
    }

}



/**
 * @brief openFile opens a given file.
 * @param window pointer to MainWindow
 * @param cell_array input vector of Unit_Cells
 * @param manage input Manager object
 * @param input input name of the file, the name is contained in
 */
void openFile(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              string input)
{
   //put the file here.
    vector<string> next = tokenizer(input, '.');
    cell_array.resize(0);

    if(next.size() < 2 || next[1] == "txt")
    {
         next[0] = next[0] + ".txt";
         manage.zeroAll();
         bool opened = readFile(window, cell_array, next[0]);
         if(opened == false)
             return;
         window->setWindowTitle(input.c_str());
         manage.file_names.resize(0);
         manage.file_names.push_back(input);
         for(unsigned int i = 0; i < cell_array.size(); i++)
             if(cell_array[i].active == true)
             {
                manage.Select(i);
                break;
             }

    }
    else if(next[1] == "cif")
    {
         manage.zeroAll();
         cell_array.resize(0);
         Unit_Cell cell = Unit_Cell();
         bool opened = read_CIF_FILE(cell, input);
         if(opened == false)
         {
             window->printText("This file could not be opened: " + input);
             return;
         }
         cell.active = true;
         cell_array.push_back(cell);

         window->setWindowTitle(input.c_str());
         manage.file_names.resize(0);
         manage.file_names.push_back(input);
    }
    else if(next[1] == "xyz")
    {
         manage.zeroAll();
         cell_array.resize(0);
         Unit_Cell cell = Unit_Cell();
         bool opened = molecule_reader_for_xyz(cell, input);
         if(opened == false)
         {
             window->printText("This file could not be opened: " + input);
             return;
         }
         cell.active = true;
         cell.geometry.isSet = false;
         cell_array.push_back(cell);

         window->setWindowTitle(input.c_str());
         manage.file_names.resize(0);
         manage.file_names.push_back(input);
    }
    else
    {
         window->printText("This type of file cannot be opened: " + input);
    }
}


/**
 * @brief importsFile imports a file into an existing open file.
 *                    The cells in the opened file will be combined with
 *                    cells defined in the imported file.
 * @param window pointer to MainWindow
 * @param cell_array input vector of Unit_Cells
 * @param manage input Manager object
 * @param input input name of the file, the name is contained in
 */
void importFile(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              vector<string> input)
{
    if(input.size() < 2)
    {
        window->printText("Failed Command: " + input[0]);
        window->printText("A name of the file must be a second variable ");
        return;
    }
    vector<string> next = tokenizer(input[1], '.');
    if(next.size() < 2)
    {
        next[0] = next[0] + ".txt";
        vector<Unit_Cell> extra;
        bool opened = readFile(window, extra, next[0]);
        if(opened == false)
            return;

        for(unsigned int i = 0; i < extra.size(); i++)
            cell_array.push_back(extra[i]);
        manage.file_names.push_back(input[1]);
    }

    else if(next[1] == "cif")
    {
        Unit_Cell cell = Unit_Cell();
        bool opened = read_CIF_FILE(cell, input[1]);
        if(opened == false)
        {
            window->printText("This file could not be opened: " + input[1]);
            return;
        }

        cell_array.push_back(cell);
        manage.Select(cell_array.size() - 1);
        manage.file_names.push_back(input[1]);
    }
    else if(next[1] == "xyz")
    {
         Unit_Cell cell = Unit_Cell();
         bool opened = molecule_reader_for_xyz(cell, input[1]);
         if(opened == false)
         {
             window->printText("This file could not be opened: " + input[1]);
             return;
         }

         cell.active = true;
         cell_array.push_back(cell);
         manage.file_names.push_back(input[1]);
    }


    else if (next[1] == "txt")
    {
         next[0] = next[0] + ".txt";
         vector<Unit_Cell> extra;
         bool opened = readFile(window, extra, next[0]);
         if (opened == false)
             return;
         for(unsigned int i = 0; i < extra.size(); i++)
             cell_array.push_back(extra[i]);
         manage.file_names.push_back(input[1]);
    }
    else
    {
         window->printText("This type of file cannot be imported: " + input[1]);
    }


    for(unsigned int i = 0; i < cell_array.size(); i++)
        if(cell_array[i].active == true)
        {
            manage.Select(i);
            for(unsigned int j = i + 1; j < cell_array.size(); j++)
                cell_array[j].active = false;
            break;
        }
}


/**
 * @brief cellOperations this method is unfinished.
 * @param window
 * @param cell_array
 * @param manage
 * @param input
 */
void cellOperations(MainWindow* window, vector<Unit_Cell> &cell_array, Manager& manage,
              string put)
{
     string modput = "";
     for(unsigned int i = 0; i < put.size(); i++)
     {
         if(put[i] != ' ' && put[i] != '\t' && put[i] != '-' && put[i] != '>')
            modput = modput + put[i];
         else if(put[i] == '-')
             modput = modput + '.';
     }
}
