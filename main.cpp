//--------------------------------------------------------------------------------------------------------------------------------
//  Adarsh Kumar Jha
//--------------------------------------------------------------------------------------------------------------------------------
//================================================================================================================================
// 1: This file is used to call all the functions and this class is basic interaction between the client and the system. For part 3
//    we have implemented an algorithm technique that compresses the file along with encoding it. This algorithm is called "LZW".
// 2: LZW = Lempel-Ziv-Welch. It is a universal lossless data compression algorithm. No data is loss during this compression. It is 
//    usually used in GIFs and PDFs. For this programming, we created a header library instead of a header file as we wanted to post
//    this code on our github later on.
//================================================================================================================================


#include "lzw.cpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace lzw;

void LZW(const uint8_t * sampleData, const int sampleSize)			//Implementation of LZW ALgorithm
{
    int compressedSizeBytes = 0;
    int compressedSizeBits  = 0;
    uint8_t * compressedFile = nullptr;  //Here, uint8_t ** compressed means that compressed 
	// is a pointer to a pointer (uint8_t*) that will hold the address of the dynamically allocated memory 
	// for compressed data.


//----------------------------------------------------------------------------------------------------------------------------------
    
	easyEncode(sampleData, sampleSize, &compressedFile, &compressedSizeBytes, &compressedSizeBits);			//Encoding
  
  	cout << "LZW uncompressed size bytes = " << sampleSize << "\n";
	cout << "LZW compressed size bytes   = " << compressedSizeBytes << "\n\n";
	
	ofstream out("encoded.txt");
	for (int i = 0; i < compressedSizeBytes; i++) 
	{
    	out << compressedFile[i];
    }
	out.close(); 
    
//----------------------------------------------------------------------------------------------------------------------------------	

    vector<uint8_t> uncompressedBuffer(sampleSize, 0); // isme uncompressed data aa jaayega compresseddata ko decode karke

	//what is vector::data() -> It returns a pointer to the first element of the internal array used by the vector.
    int uncompressedSize = easyDecode(compressedFile, compressedSizeBytes, compressedSizeBits, uncompressedBuffer.data(), uncompressedBuffer.size());				//Decoding
                                                 
    ofstream out2("decoded.txt");
    for (int i = 0; i < uncompressedBuffer.size(); i++) 
    {
    	out2 << uncompressedBuffer[i];
    }
    
    out2.close();
    
	cout << "LZW decompressed size bytes   = " << uncompressedBuffer.size() << "\n\n";


//----------------------------------------------------------------------------------------------------------------------------------


    bool successful = true;
    if (uncompressedSize != sampleSize)					// comparing sizes
    {
        cout << "LZW COMPRESSION ERROR! Size mismatch!\n";
        successful = false;
    }
    
//----------------------------------------------------------------------------------------------------------------------------------

    //memcmp is a buitl in function declared in the <cstring> which 
	// compare the contents of two memory blocks (buffers) byte by byte
	
    if (memcmp(uncompressedBuffer.data(), sampleData, sampleSize) != 0)				//Comparing data
    {
        cout << "The files are not same. Data has been corrupted!\n";
        successful = false;
    }

    if (successful)
    {
        cout << "LZW compression successful!\n";
    }

    free(compressedFile);

}

//----------------------------------------------------------------------------------------------------------------------------------
//															Main starts here
//----------------------------------------------------------------------------------------------------------------------------------



int main() 
{

//----------------------------------------------------------------------------------------------------------------------------------

	cout << endl << "----------------------------------------------------------------------------------------------------" << endl;
	cout << endl << "----------------------------------------------------------------------------------------------------" << endl;
	cout << endl << "----------------------------------------------------------------------------------------------------" << endl;
	cout << "\t \t \t Task 3 (LZW Data Compression & Decompression Algorithm)" << endl<<endl;
	cout << "\t \t \t\t\t Adarsh Kumar Jha " << endl;
	cout << "\t \t \t\t Welcome to Our File Compressor" << endl;
	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;
	cout << endl << "----------------------------------------------------------------------------------------------------" << endl;
	cout << endl << "----------------------------------------------------------------------------------------------------" << endl;
//----------------------------------------------------------------------------------------------------------------------------------
	
	string str,filename;
	
	cout<<"Enter the name of the file without the extension = ";
	cin>>filename;	
	ifstream in(filename + ".txt");

	cout<<"Waiting..."<<endl;
	if (in)
	{
		char c = in.get();
		while (!in.eof()) 
		{
			str += c;
			c = in.get();
		}
		in.close();
	}
	
	
	cout << "Bytes read from file = " << str.size() << endl;
	
	
	vector<uint8_t> myVector(str.begin(), str.end());
	static const uint8_t* str0 = &myVector[0]; 
	// is line ka matlab hai pointer to the first element of vector banana 
	//jiska type hai unit8_t The const keyword ensures that the data pointed to by str0 cannot be modified through this pointer.
	// This adds safety, as it signals that the pointer should only be used to read data, not modify it.
	//It is also stactic bec it will retain it's value in all functions and it is initialised only once

	// for(int i=0; i<myVector.size(); i++){
	// 	cout<<myVector[i]<<" ";
	// }
	// cout<<endl;
	
	LZW(str0, str.size());
	
}
