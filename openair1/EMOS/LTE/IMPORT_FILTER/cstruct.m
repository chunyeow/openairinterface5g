%CSTRUCT   Analyze a Matlab array to be packed into a C structure
%   ATTRIB = CSTRUCT(FORMAT) analyzes a Matlab array, to determine how many
%   bytes required to pack the data into a C language structure, and its
%   byte alignment. FORMAT is any numerical or aggregate (CELL or STRUCT)
%   data type. ATTRIB is a structure containing fields SIZE, which
%   determines how many bytes are required to store data organized lie
%   FORMAT, and ALIGN which is the data's byte alignment.
%
%   DATA = CSTRUCT(FORMAT,BYTES) - packs the INT8 or UINT8 type values in
%   BYTES, organizes them according to FORMAT, and returns the bytes as
%   DATA.
%
%   [ATTRIB,BYTES] = CSTRUCT(DATA) allows conversion of a Matlab structure
%   to a UINT8 vector BYTES as would be stored by a C structure.
%
%   ATTRIB = CSTRUCT(FORMAT,[],...) and
%   [ATTRIB,BYTES] = CSTRUCT(DATA,[],...) and
%   DATA = CSTRUCT(FORMAT,BYTES,...) allows options to be specified starting
%   at the third argument.
%
%   [...] = CSTRUCT(...,ALIGNEMNT) allows the default byte
%   alignment of 8 to be changed to the specified value, typically 1, 2, 4,
%   8, depending on how the C (or other) compiler organizes the data within
%   BYTES.
%
%   [...] = CSTRUCT(...,BYTEORDER) allows the default byte order (big Endian)
%   of the binary data to be changed using one of the following characters:
%     'n' - Native byte ordering (byte are not swapped, regarless of platform)
%     'l' - Little Endian byte ordering
%     'b' - Big Endian byte ordering
%     'r' - Reverse byte ordering (bytes always swapped)
%
%   CSTRUCT is particularly useful when reading or writing data to/from a
%   file or socket interface which contains data that was created in
%   another language, such as C. As an example, lets say the following C
%   structure as written to a file:
%      struct { long A; double B; char C[5]; };
%   Because double values are (typically) aligned on 8-byte boundaries,
%   padding gets added to the structure. The memory-byte representation
%   or BYTES would be (24 bytes):
%      A A A A x x x x B B B B B B B B C C C C C x x x
%   To convert this vector of bytes to a matlab structure, the following
%   would work:
%      DATA = CSTRUCT(struct('A',int32(0),'B',double(0),'C','xxxxx'},BYTES)
%   DATA would be STRUCT fields A, B and C with data types INT32, DOUBLE
%   and CHAR, respectively.
%
%   See also BINREAD, BINWRITE, COMPUTER
