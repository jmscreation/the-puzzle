#ifndef __ARCHIVE_FILE__
#define __ARCHIVE_FILE__

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

class ArchiveFile {
protected:
    const size_t fileVersion;
    size_t fileDataRegion;

    virtual std::string ReadString();

    virtual uint64_t ReadUint64();
    virtual uint32_t ReadUint32();
    virtual uint16_t ReadUint16();
    virtual uint8_t ReadUint8();

    virtual int64_t ReadInt64();
    virtual int32_t ReadInt32();
    virtual int16_t ReadInt16();
    virtual int8_t ReadInt8();

    virtual float ReadFloat();
    virtual double ReadDouble();

    virtual size_t GetTimestamp();
    virtual size_t GetDataRegion();
    
    virtual int Error();
public:
    ArchiveFile();
    virtual ~ArchiveFile();

    struct ArchiveHeader {
        size_t archiveVersion;
        size_t archiveTimestamp;
        std::string archiveName;
        std::string archiveSignature;
        size_t fileCount;
    } header;

    struct FileHeader {
        std::string name;
        std::string type;
        std::string filename;
        size_t size;
        size_t offset;

        const char* buffer; // not used in file - internal buffer use
    } empty;
    std::vector<FileHeader> table;

    struct Stream {
        const char* buffer;
        size_t length, pos;
    } archive;

    bool archiveError;

    virtual void ClearArchive();
    virtual bool LoadArchive(const char* data, size_t length);
    virtual FileHeader FindFile(const std::string& name) const;
    virtual std::vector<FileHeader> ListFiles() const;

};










#endif // __ARCHIVE_FILE__