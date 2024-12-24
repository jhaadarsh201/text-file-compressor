//--------------------------------------------------------------------------------------------------------------------------------
//  Adarsh Kumar Jha
//--------------------------------------------------------------------------------------------------------------------------------
//================================================================================================================================
// 1: This file is basically an implementation of header file "dictionary.h"
//================================================================================================================================



#include "dictionary.h"
// ========================================================
// class Dictionary:
// ========================================================

Dictionary::Dictionary()
{
    size = FirstCode; //FirstCode value is 256
    for (int i = 0; i < size; ++i)
    {
        entries[i].code  = Nil;
        entries[i].value = i;
    }
}

int Dictionary::findIndex(const int code, const int value) const
{
    if (code == Nil)
    {
        return value; //Ye tab block tab hit hoga jab ASCII code ko search karna hoga
    }

    // Linear search
    for (int i = 0; i < size; ++i)
    {
        if (entries[i].code == code && entries[i].value == value)
        {
            return i;
        }
    }

    return Nil;
}

bool Dictionary::add(const int code, const int value)
{
    if (size == MaxDictEntries)
    {
    	cout<<"Dictionary overflowed!"<<endl;
        return false;
    }

    entries[size].code  = code;
    entries[size].value = value;
    ++size;
    return true; //The key value pair has been sucessfully added
}

bool Dictionary::flush(int & codeBitsWidth)
{
    if (size == (1 << codeBitsWidth))
    {
        ++codeBitsWidth;
        if (codeBitsWidth > MaxDictBits)
        {
            // Clear the dictionary (except the first 256 byte entries).
            codeBitsWidth = StartBits;
            size = FirstCode;
            //Returns true when the dict. is flushed 
            return true;
        }
    }
    return false;
}
