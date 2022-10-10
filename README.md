# MIRABO 
A program to display and manipulate crystalline compounds. Accepts all *.cif and *.xyz file formats, allows for manipulation of 
crystal Unit Cells with a keyboard or a mouse. When first launched, loads a sample LCO.cif file, which displays a single 
Unit Cell of Lanthanum Cobalt Oxide. Later the program would load the last file the user worked on - saved in file mirabo_autosave.txt. 
To understand the structure of the program, go to "man_explained.txt" file. 

Image of the structure of salt NaCl created by the program: <br>
![Alt text](pics/salt.png?raw=true "NaCl") <br>
Image of the interface between Graphite and SiO<sub>2</sub> <br>
![Alt text](pics/GraphiteSiO2comp.png?raw=true "Graphite/SiO") <br>
Image of the single subsection of LaCoO<sub>3</sub> of 8 octahedra <br>
![Alt text](pics/octahedra11.png?raw=true "octahedra") <br>


## Keyboard Commands
- To see all the possible keypress commands in openGL widget, got to "man_commands.txt" file or "opengl_keypress.cpp" source code
file.
- To see all the possible line commands, got to "man_commands.txt" file or "line_keypress.cpp" source code file. 

If the openGL widget (in my program known as MyGLWidget) is active, each key on the keyboard is a command. To know how each command operates, go to line input widget (LineEdit widget) and type: man *key* 
For example:
man k
Would print out the results of such a command, alternatively looking at the file "man_commands.txt" has all the possible OpenGL widget commands that my program accepts.
To execute this command, click on the MyGlWidget (openGL widget) and press 'k' key. 

In the LineEdit widget to know how each command operates type: man *line_command* , alternatively looking at the file "man_commands.txt" does the trick too. 
For example typing into LineEdit widget:
man invert
Would explain how align command works. 
Typing in:
invert
Would execute 'invert' command.

A collection of sample cif files is saved in the "Saved_files" folder. 
When the program is first started, a file located in "Saved_files/mirabo_autosave.txt" is loaded. 

To load a new file, type into LineEdit widget:
open <relative_path_started from Mirabo.exe/file_name>
This would open a new file. A collection of sample file is stored in the Saved_files folder, so a command
open Saved_files/LCO.cif
Would open the LCO file. To see what fun things can be done next, look into 'man_commands.txt' file. 

To save a file, type into LineEdit widget: 
save <relative_path_started from Mirabo.exe/file_name.txt>
without the <,> of course. 

Tab key autocompletes a command when selected in the lineWidget. 




 
