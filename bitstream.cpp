//--------------------------------------------------------------------------------------------------------------------------------
//  Adarsh Kumar Jha
//--------------------------------------------------------------------------------------------------------------------------------
//================================================================================================================================
// 1: This file is basically the implementation of header file "bitstream.h"
//================================================================================================================================



#include "bitstream.h"
// ========================================================
// Round up to the next power-of-two number, e.g. 37 => 64
static int nextPowerOfTwo(int num)
{
//    --num;
//    for (size_t i = 1; i < sizeof(num) * 8; i <<= 1)
//    {
//        num = num | num >> i;
//    }
//    return ++num;
     if (num <= 0) return 1; // Edge case: for non-positive numbers, return 1.

    // Check if num is already a power of two
    if ((num & (num - 1)) == 0) 
        return num;

    // Find the next power of two using leading zero count
    int leadingZeros = __builtin_clz(num); // Count leading zeros
    int totalBits = sizeof(num) * 8;       // Total bits in int (usually 32)
    int msbPosition = totalBits - leadingZeros; // Position of the most significant bit (1-based)
    
    return 1 << msbPosition; // Calculate the next power of two
}

// ========================================================


// ========================================================
// class BitStreamWriter:
// ========================================================

void BitStreamWriter::internalInit()
{
    stream         = nullptr;
    bytesAllocated = 0;
    granularity    = 2;
    currBytePos    = 0;
    nextBitPos     = 0;
    numBitsWritten = 0;
}

void BitStreamWriter::allocate(int bitsWanted)
{
    // Require at least a byte.
    if (bitsWanted <= 0)
    {
        bitsWanted = 8;
    }

    // Round upwards if needed:
    if ((bitsWanted % 8) != 0)
    {
        bitsWanted = nextPowerOfTwo(bitsWanted);
    }

    // We might already have the required count.
    const int sizeInBytes = bitsWanted / 8;
    //yaha pe change kiya hu
    if (sizeInBytes <= BitStreamWriter::bytesAllocated)
    {
        return;
    }

    stream = allocBytes(sizeInBytes, stream, BitStreamWriter::bytesAllocated); //is function ka defintion header me nhi hai
                                                                    //naya buffer bana dega ye with size of sizeInBytes variable
    bytesAllocated = sizeInBytes;
}

BitStreamWriter::BitStreamWriter()				  // 8192 bits for a start (1024 bytes). It will resize if needed. Default granularity is 2.
{

    internalInit();
    allocate(8192); //implementaion is written on the top
}

BitStreamWriter::BitStreamWriter(const int initialSizeInBits, const int growthGranularity)
{
    internalInit();
    setGranularity(growthGranularity);
    allocate(initialSizeInBits);
}

BitStreamWriter::~BitStreamWriter()
{
    if (stream != nullptr)
    {
        free(stream);
    }
}

void BitStreamWriter::appendBit(const int bit)
{
    const uint32_t mask = uint32_t(1) << nextBitPos;
    stream[currBytePos] = (stream[currBytePos] & ~mask) | (-bit & mask);
    ++numBitsWritten;

    if (++nextBitPos == 8)
    {
        // If nextBitPos reaches 8 (all bits in the byte are used), reset it to 0 and move to the next byte
        nextBitPos = 0;
        // currBytePos is incremented to move to the next byte in the buffer.
        // If currBytePos reaches bytesAllocated (all allocated bytes are used), the buffer needs to grow.
        if (++currBytePos == bytesAllocated)
        {
            allocate(bytesAllocated * granularity * 8);
        }
    }
}

void BitStreamWriter::appendBitsU64(const uint64_t num, const int bitCount)
{
    assert(bitCount <= 64);
    for (int b = 0; b < bitCount; ++b)
    {
        const uint64_t mask = uint64_t(1) << b;
        const int bit = !!(num & mask); //Double negation (!!) converts the result into a boolean 0 (Stack Overflow)
                                        // (if the bit is 0) or 1 (if the bit is 1). Iske jagah ternary op bhi use kar sakte ho
                                        // In C++, !x is a bool type, So !!x is a "collapse to false or true operator" 
                                        // in the sense that any non-zero number is mapped to true, and zero is mapped to false.
        appendBit(bit);
    }
}


void BitStreamWriter::appendBitString(const string & bitStr)
{
    for (size_t i = 0; i < bitStr.length(); ++i)
    {
        appendBit(bitStr[i] == '0' ? 0 : 1);
    }
}

string BitStreamWriter::toBitString() const
{
    string bitString;

    int usedBytes = numBitsWritten / 8;
    int leftovers = numBitsWritten % 8;
    if (leftovers != 0)
    {
        ++usedBytes;
    }
    assert(usedBytes <= bytesAllocated); //Ensures the number of used bytes does not exceed the allocated size of the stream buffer. 
                                        // This is a safeguard against memory corruption or buffer overflows.


    for (int i = 0; i < usedBytes; ++i)
    {
        const int nBits = (leftovers == 0) ? 8 : (i == usedBytes - 1) ? leftovers : 8;
        for (int j = 0; j < nBits; ++j)
        {
            bitString += (stream[i] & (1 << j) ? '1' : '0');
        }
    }

    return bitString;
}


uint8_t * BitStreamWriter::release()
{
	uint8_t * oldPtr = stream;
    internalInit();
    return oldPtr;
}

void BitStreamWriter::setGranularity(const int growthGranularity)
{
    granularity = (growthGranularity >= 2) ? growthGranularity : 2;
}

int BitStreamWriter::getByteCount() const
{
    int usedBytes = numBitsWritten / 8;
    int leftovers = numBitsWritten % 8;
    if (leftovers != 0)
    {
        ++usedBytes;
    }
    assert(usedBytes <= bytesAllocated);
    return usedBytes;
}

int BitStreamWriter::getBitCount() const
{
    return numBitsWritten;
}

const uint8_t * BitStreamWriter::getBitStream() const
{
    return stream;
}

uint8_t * BitStreamWriter::allocBytes(const int bytesWanted, uint8_t * oldPtr, const int oldSize)
{
    uint8_t * newMemory = static_cast<uint8_t *>(malloc(bytesWanted)); //Static cast means this conversion will happen 
                            // at complie time aur mujhe pata bhi chal jaayega kaha pe change bhi hua hai
                            //isme conversion operator/ conversion constructor bhi padh lena
    memset(newMemory, 0, bytesWanted);

    if (oldPtr != nullptr)
    {
        memcpy(newMemory, oldPtr, oldSize);
        free(oldPtr); //Memory leak se bachne ke liye
    }

    return newMemory;
}

// ========================================================
// class BitStreamReader:
// ========================================================

BitStreamReader::BitStreamReader(const BitStreamWriter & bitStreamWriter) //What kind of syntax is this
    //Isme jo bisStreamWriter ne pahale buffer banaya tha usse hi reader ko bata rahe hai
    : stream(bitStreamWriter.getBitStream())
    , sizeInBytes(bitStreamWriter.getByteCount())
    , sizeInBits(bitStreamWriter.getBitCount())
{
    reset(); //iss reset ka bhi kaam nhi pata ab to
}

BitStreamReader::BitStreamReader(const uint8_t * bitStream, const int byteCount, const int bitCount)
    : stream(bitStream)         // 
    , sizeInBytes(byteCount)    // ------> Shayad ko constructor ko initialize karne ka tareeka hai
    , sizeInBits(bitCount)      //
{
    reset();
}

bool BitStreamReader::readNextBit(int & bitOut)
{
    if (numBitsRead >= sizeInBits)
    {
        return false; // We are done.
    }

    const uint32_t mask = uint32_t(1) << nextBitPos;
    bitOut = !!(stream[currBytePos] & mask);
    ++numBitsRead;

    if (++nextBitPos == 8)
    {
        nextBitPos = 0;
        ++currBytePos;
    }
    return true;
}

uint64_t BitStreamReader::readBitsU64(const int bitCount)
{
    assert(bitCount <= 64); //ye production lvl me code check karne ke liye hota hai 
                            //jab check ho jaaye to assert ko hata dena by using NDEBUG

    uint64_t num = 0;
    for (int b = 0; b < bitCount; ++b)
    {
        int bit;
        if (!readNextBit(bit))
        {
        	cout<<"Failed to read bits from stream! Unexpected end."<<endl;
            break;
        }

        // Based on a "Stanford bit-hack": //ye bhi nhi padha
        const uint64_t mask = uint64_t(1) << b;
        num = (num & ~mask) | (-bit & mask);
    }

    return num;
}

void BitStreamReader::reset()
{
    currBytePos = 0;
    nextBitPos  = 0;
    numBitsRead = 0;
}

bool BitStreamReader::isEndOfStream() const
{
    return numBitsRead >= sizeInBits;
}

