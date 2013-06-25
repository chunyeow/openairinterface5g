//--------------------------------------------------------------
// file: MatlabCStruct.h - Include file for MatlabCStruct.c
//--------------------------------------------------------------

//--------------------------------------------------------------
// ELEMENT_ATTRIBUTES
//--------------------------------------------------------------
typedef struct
{
  int ElementSize; // Number of bytes required for storage of a single element in a Matlab array
  int ElementCount; // Number of elements in array
  int StorageSize; // Number of bytes required for storage of a single element in a C Structure
  int Alignment; // 1, 2, 4, or 8 denoting required byte alignment for this structure.
} ELEMENT_ATTRIBUTES;

extern int MatlabCStruct_DebugFlag;

// Function prototypes
ELEMENT_ATTRIBUTES MatlabCStruct(const mxArray *pArray, int MemoryAlignment, int NestLevel,
                                     unsigned char *pDataDest, unsigned char *pDataSource,
                                     int SwapFlag);
char *ClassIDText(const mxArray *pArray);
void SwapBytes(char *p0, int size, int count);
#define SWAP(x) SwapBytes(&(x), sizeof(x))




