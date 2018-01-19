#include "Vec3d.h"


struct Selected_atom {
	int clicked;
	int cell;
};

class Manager
{
	private:
        vector<Selected_atom> clicked_atoms;   //holds the information for all selected atoms
        int selected_cell;   //holds the number of the selected cell.
	public:
		Manager ()
		{
            toggle_question = true;
            selected_cell = 0;
		}
        bool toggle_question;
        vector<string> file_names;

        Manager copy()
        {
            Manager manage;
            for(unsigned int i = 0; i < clicked_atoms.size(); i++)
            {
                manage.putAtom(clicked_atoms[i].cell, clicked_atoms[i].clicked);
            }
            manage.toggle_question = toggle_question;
            manage.selected_cell = selected_cell;
            manage.file_names = file_names;
            return manage;
        }

        int getSize() { return clicked_atoms.size(); }
		
		void Select(int input) { selected_cell = input; }
		
        void zeroAll()
        {
            clicked_atoms.resize(0);
            selected_cell = 0;
        }


		void removeSelected(int clicked)
		{
			vector<Selected_atom> temp;
            for(unsigned int i = 0; i < clicked_atoms.size(); i++)
			{
				
                if(selected_cell == clicked_atoms[i].cell &&\
                        clicked == clicked_atoms[i].clicked) continue;
					
                temp.push_back(clicked_atoms[i]);
				
			}
            clicked_atoms = temp;
		}
		
		void removeAll(vector<int> input)
		{
			for(unsigned int i = 0; i < input.size(); i++)
			{
				removeSelected(input[i]);
			}
		}
		
		void removeAll()
		{
            clicked_atoms.resize(0);
		}	
		int getSelected() { return selected_cell; }
		

/**
         * @brief putAtom
         * @param place
         * @return
         */

        bool putAtom(int place)
		{
			Selected_atom tmp;
			tmp.clicked = place;
			tmp.cell = selected_cell;
            clicked_atoms.push_back(tmp);
			return true;
		}

        bool putAtom(int cell, int index)
        {
            Selected_atom tpm;
            tpm.clicked = index;
            tpm.cell = cell;
            clicked_atoms.push_back(tpm);
            return true;
        }

		void print()
		{
            for(unsigned int i = 0; i < clicked_atoms.size(); i++)
			{
                cout << i << ". atom:" << clicked_atoms[i].clicked \
                     << " cell:" << clicked_atoms[i].cell << endl;
			}
		}
		
        /*
         * Gets a selected atom for a given position in the array.
         */
		Selected_atom getAtom(int index)
		{
            return clicked_atoms[index];
		}

        vector<Selected_atom> getAll()
        {
            return clicked_atoms;
        }

/**
         * @brief getAtomsSelectedCell gets all the atoms in the selected cell
         * @return return an int array of positions of atoms that are selected.
         */
        vector<int> getAtomsSelectedCell()
        {
            vector<int> atoms_in_selected_cell;
            for(unsigned int i = 0; i < clicked_atoms.size(); i++)
            {
                if(clicked_atoms[i].cell == selected_cell)
                {
                    atoms_in_selected_cell.push_back(clicked_atoms[i].clicked);
                }
            }
            return atoms_in_selected_cell;
        }
};
