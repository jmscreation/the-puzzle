#ifndef __ARCHIVE_FILE_EXPORTER__
#define __ARCHIVE_FILE_EXPORTER__

#include "archivefile.h"

#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class ArchiveFileExporter : public ArchiveFile {
    virtual bool WriteString(const std::string& val);

    virtual bool WriteUint64(uint64_t val);
    virtual bool WriteUint32(uint32_t val);
    virtual bool WriteUint16(uint16_t val);
    virtual bool WriteUint8(uint8_t val);
    virtual bool WriteInt64(int64_t val);
    virtual bool WriteInt32(int32_t val);
    virtual bool WriteInt16(int16_t val);
    virtual bool WriteInt8(int8_t val);

    virtual bool WriteFloat(float val);
    virtual bool WriteDouble(double val);
    virtual bool WriteData(const char* data, size_t length);


    virtual bool WriteHeader();
    virtual bool WriteFile(FileHeader fheader);

public:
    ArchiveFileExporter();
    virtual ~ArchiveFileExporter();

    std::ostream* buffer;
    size_t dataPosition; // next file offset location

    virtual inline bool LoadArchive(Stream data) { return false; } // remove reading
    virtual inline FileHeader FindFile(const std::string& name) { return empty; } // remove reading

    virtual bool SignArchive(const std::string& signature);
    virtual void ClearArchive() override;
    virtual bool AddFile(const std::string& path, const std::string& name, const std::string& type);
    virtual bool SaveArchive(const std::string& path, const std::string& name);

};










#endif // __ARCHIVE_FILE_EXPORTER__