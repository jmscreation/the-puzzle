/**                 Simple ZLIB Wrapper
    Author: jmscreator          2020 - 01 - 27

    This uses C++ standard library strings as input / output for extreme simplicity

**/

#ifndef __SIMPLE_ZLIB
#define __SIMPLE_ZLIB


#include "zlib.h"
#include <string>
#include <sstream>


namespace simplezlib {

    namespace __internal { //  internal function scope
        // convert std::string to Bytef buffer in heap
        inline Bytef* Allocate(const std::string& str) {
            Bytef* buf = new Bytef[str.size()];
            memcpy(buf, reinterpret_cast<const Bytef*>(str.data()), str.size());
            return buf;
        }
        // allocate fixed size Bytef buffer in heap
        inline Bytef* Allocate(const uLongf sz) {
            Bytef* buf = new Bytef[sz];
            memset(buf, 0, sz);
            return buf;
        }
        // convert Bytef buffer into an std::string
        inline void Stringify(const Bytef* buf, const uLongf len, std::string& output) {
            std::stringstream outStream;
            outStream.write(reinterpret_cast<const char*>(buf), len);
            output += outStream.str();
        }
        // free the Bytef buffer from heap
        inline void Free(Bytef* buf) {
            delete[] buf;
        }
    }   //  end internal function scope

    inline int Compress(const std::string& input, std::string& output) {
        using namespace __internal; // access internal members

        uLongf dataSize = input.size(),  // uncompressed data size
               compressedSize = compressBound(dataSize);  // compressed data size

        Bytef *in = Allocate(input), *out = Allocate(compressedSize); // allocate data

        int result = compress(out, &compressedSize, in, dataSize);  // compress data

        Free(in);   // free input buffer - we don't need it anymore

        if(result != Z_OK){
            Free(out);  // free output buffer before return
            return result;   // compression failed - return error
        }

        for(unsigned int i=0; i<sizeof(dataSize); i++) output.push_back(*((unsigned char*)&dataSize + i));    // take the raw data from dataSize and dump into the output

        Stringify(out, compressedSize, output); // stringify the compressed data onto the output string
        Free(out);  // free the remaining buffer

        return Z_OK;
    }

    inline int Decompress(const std::string& input, std::string& output) {
        using namespace __internal; // access internal members

        if(input.size() < sizeof(uLongf)) return -7;    // error because there's not enough data for the dataSize

        std::string data = input.substr(sizeof(uLongf), std::string::npos);   // chop off the uLongf data from the beginning

        uLongf dataSize,    //uncompressed data size
               compressedSize = data.size();  // compressed data size
        memcpy(&dataSize, (unsigned char*)(input.data()), sizeof(uLongf));   // get the uncompressed data size

        Bytef *in = Allocate(data), *out = Allocate(dataSize);    // allocate data

        int result = uncompress(out, &dataSize, in, compressedSize);

        if(result != Z_OK) {
            Free(in);   // free input buffer
            Free(out);  // free output buffer before return
            return result;   // compression failed - return error
        }


        Stringify(out, dataSize, output);
        Free(in);   // free input buffer
        Free(out);  // free the remaining buffer

        return Z_OK;
    }
}



#endif // __SIMPLE_ZLIB
