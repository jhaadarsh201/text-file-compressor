//--------------------------------------------------------------------------------------------------------------------------------
// Adarsh Kumar Jha
//--------------------------------------------------------------------------------------------------------------------------------
//================================================================================================================================
// 1: This file is used in encoding and decoding. First 256 dictionary entries are reserved to the byte/ASCII range. Additional entries 	
//	follow for the character sequences found in the input. Up to 4096 - 256 (MaxDictEntries - FirstCode).
//================================================================================================================================


#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>
using namespace std;

// ========================================================
// LZW Dictionary helper:
// ========================================================


// Ab use kyu karna constexpr ko -> Ye compile time me output produce kar skata hai ...but hamesha karega uski gaurente nhi
// Jese PI ko constexpr bana diya to jaha bhi PI variable use hua hai vaha pe complie time me hi PI ki value replace ho jayegi
// Fhir macros me kya dikkat ho gayi -> Macros ek code hai jo khud code likhta hai aur complie hone se pahale hi 
                                      // #define wale line ko replace kar deta hai

//isme ham constexpr ko hata bhi sakte hai
const int Nil = -1;
const int MaxDictBits = 12;
const int StartBits = 9;
const int FirstCode = (1 << (StartBits - 1)); // 256
const int MaxDictEntries = (1 << MaxDictBits); // 4096


// The final keyword makes the class Dictionary non-inheritable.
// If a class is not designed for inheritance (
// e.g., it has no virtual destructor), inheritance can lead to undefined behavior or bugs.
// If a class is not designed for inheritance (e.g., it has no virtual destructor), 
// inheritance can lead to undefined behavior or bugs.
class Dictionary final
{
public:
    //struct kyu banaya do alag se entry bana lete?
    //ese to pair<int, int> kyu likha do alag se integer likh lo nonsense

    struct Entry //The Entry structure represents a single entry in the dictionary.
    {
        int code;
        int value;
    };

    int size;
    Entry entries[MaxDictEntries]; //This is an array of Entry objects. Max number of entries a dict. can hold

    Dictionary();
    int findIndex(int code, int value) const; //const at the end indicates that this function does not modify any member variables of the class.

    //Checks if there is enough space in the dictionary (i.e., size < MaxDictEntries).
    // If space is available, it inserts the entry into the entries array and increments size.
    // Returns true on success, false if the dictionary is full or the entry cannot be added.
    bool add(int code, int value);


    bool flush(int & codeBitsWidth); //Clear all entry and reset size to zero but codeBitsWidth ka kaam pata nhi

};
