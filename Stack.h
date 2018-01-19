#ifndef STACK_H
#define STACK_H

#include <deque>

#include "Unit_Cell.h"
#include "header.h"


   //maximum allowed stack size, beyond that the deque would be cut from the tail,
   //the last saved entries would be deleted.
const int STACK_SIZE = 200;

class Container
{
    public:
        Container(vector<Unit_Cell> input_cell, Manager input_manage)
        {
            manage = input_manage.copy();
            cell.resize(input_cell.size());
            for(unsigned int i = 0; i < input_cell.size(); i++)
            {
                cell[i] = input_cell[i].copy();
            }
        }

        vector<Unit_Cell> cell;
        Manager manage;
};


/**
 * @brief The Stack class is used to save the information contained in the running program,
 *        this is useful if the undo command is employed.
 */
class Stack
{
	public:
        Stack(){}

		void push(Container contents)
		{
			backpack.push_back(contents);
            if(backpack.size() > STACK_SIZE)
                backpack.pop_front();
		}
	
		Container pop()
		{
			Container temp = backpack.back();
			backpack.pop_back();
			return temp;
		}		
	
		int size()
		{
			return backpack.size();
		}

	private:
		deque<Container> backpack;
};


#endif   //STACK_H
