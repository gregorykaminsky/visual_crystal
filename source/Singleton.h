#ifndef SINGLETON_H
#define SINGLETON_H
#include <string>
#include "myarray.h"

class Singleton
{
    public:
            static Singleton& getInstance()
            {
                    static Singleton instance;
                    return instance;
            }

        vector<Unit_Cell> cell;
        Stack stack;
        Vec3d motion;
        int four_vector_draw;
        vector<string> all_keys;
        MyArray lineEdit_input;
        MyArray tab_array;
        Manager manage;

    private:
            Singleton() {}
            Singleton(Singleton const&);   // Don't Implement.
            void operator=(Singleton const&);   // Don't implement
};




#endif   // SINGLETON_H

