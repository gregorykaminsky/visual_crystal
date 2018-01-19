#ifndef MYARRAY
#define MYARRAY

#include <string>
class MyArray
{
    private:
        int pos;
        bool accessed;
        string start;
        vector<string> info;

    public:
        MyArray()
        {
            pos = 0;
            accessed = false;
        }
        void push_back(string input)
        {
            info.push_back(input);
        }

        inline string operator[](int input)
        {
            return info[(unsigned int)input];
        }

        void access(bool input)
        {
            accessed = input;
        }

        bool getAccess()
        {
            return accessed;
        }


        int index()
        {
            return pos;
        }

        void setIndex(int ipos)
        {
            pos = ipos;
        }

        void increment()
        {
           pos = (pos + 1)%info.size();
        }

        void decrement()
        {
            pos = pos - 1;
            if(pos < 0)
                pos = info.size() + pos;
        }

        string get()
        {
            return info[pos];
        }

        void put(string input)
        {
            start = input;
        }

        string getInitial()
        {
            return start;
        }

        unsigned int size()
        {
            return info.size();
        }

};


#endif   // MYARRAY

