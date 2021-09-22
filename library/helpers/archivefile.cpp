#include "archivefile.h"

ArchiveFile::ArchiveFile(): fileVersion(100) {
    empty.size = empty.offset = 0;
    ClearArchive();
}

ArchiveFile::~ArchiveFile() {}

int ArchiveFile::Error() {
    std::cout << "archive data reached end of stream\n";
    archiveError = true;
    return 0;
}

void ArchiveFile::ClearArchive() {
    header.archiveVersion = fileVersion;
    header.archiveName = "";
    header.archiveSignature = "";
    header.archiveTimestamp = 0;
    header.fileCount = 0;

    fileDataRegion = 0;

    archive.buffer = nullptr;
    archive.length = 0;
    archive.pos = 0;

    archiveError = false;

    table.clear();
}

bool ArchiveFile::LoadArchive(const char* data, size_t length) {
    if(length == 0){
        std::cout << "no archive data to load\n";
        return false;
    }

    ClearArchive();
    archive.buffer = data;
    archive.length = length;

    header.archiveVersion = ReadUint64();
    if(header.archiveVersion > fileVersion){
        std::cout << "archive too new to load\n";
        ClearArchive();
        return false; // cannot load newer versions
    }

    header.archiveName = ReadString();
    header.archiveSignature = ReadString();
    header.archiveTimestamp = ReadUint64();
    header.fileCount = ReadUint64();

    table.clear();
    table.reserve(header.fileCount);

    for(size_t i=0; i < header.fileCount; ++i){
        FileHeader file;
        file.name = ReadString();
        file.type = ReadString();
        file.filename = ReadString();
        file.size = ReadUint64();
        file.offset = ReadUint64();

        file.buffer = nullptr;

        table.push_back(file);
    }

    fileDataRegion = archive.pos;

    for(FileHeader& f : table){
        f.buffer = archive.buffer + GetDataRegion() + f.offset; // set file buffer position to file offset in memory
    }
    return true;
}

ArchiveFile::FileHeader ArchiveFile::FindFile(const std::string& name) const {
    auto f = std::find_if(table.begin(), table.end(), [&](const FileHeader& f){ return f.name == name; });
    if(f == table.end()) return empty;

    return *f;
}

std::vector<ArchiveFile::FileHeader> ArchiveFile::ListFiles() const {
    return table;
}

size_t ArchiveFile::GetTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count();
}

size_t ArchiveFile::GetDataRegion() { // calculate position offset where the data region is based upon the file format headers
    return fileDataRegion;
}

std::string ArchiveFile::ReadString() {
    std::string rval;
    size_t len = 0, dataleft = archive.length - archive.pos;

    while(1){
        if(dataleft - len <= 0) {
            Error();
            return "";
        }
        if(!*(archive.buffer + archive.pos + len++)) break; // get length of string (null terminated)
    }

    rval.assign(archive.buffer + archive.pos, len - 1); // assign string
    archive.pos += len; // increment read pointer

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
uint64_t ArchiveFile::ReadUint64() {
    size_t dataleft = archive.length - archive.pos;
    uint64_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
uint32_t ArchiveFile::ReadUint32() {
    size_t dataleft = archive.length - archive.pos;
    uint32_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
uint16_t ArchiveFile::ReadUint16() {
    size_t dataleft = archive.length - archive.pos;
    uint16_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
uint8_t ArchiveFile::ReadUint8() {
    size_t dataleft = archive.length - archive.pos;
    uint8_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
int64_t ArchiveFile::ReadInt64() {
    size_t dataleft = archive.length - archive.pos;
    int64_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
int32_t ArchiveFile::ReadInt32() {
    size_t dataleft = archive.length - archive.pos;
    int32_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
int16_t ArchiveFile::ReadInt16() {
    size_t dataleft = archive.length - archive.pos;
    int16_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
int8_t ArchiveFile::ReadInt8() {
    size_t dataleft = archive.length - archive.pos;
    int8_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
float ArchiveFile::ReadFloat() {
    float dataleft = archive.length - archive.pos;
    uint64_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}
double ArchiveFile::ReadDouble() {
    double dataleft = archive.length - archive.pos;
    uint64_t rval;

    if(dataleft < sizeof(rval)) return Error();

    memcpy((char*)&rval, archive.buffer + archive.pos, sizeof(rval));
    archive.pos += sizeof(rval);

    if(archive.pos > archive.length) archive.pos = archive.length;

    return rval;
}