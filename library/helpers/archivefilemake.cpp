#include "archivefilemake.h"

namespace fs = std::filesystem;

ArchiveFileExporter::ArchiveFileExporter() : buffer(nullptr), dataPosition(0), ArchiveFile() {}

ArchiveFileExporter::~ArchiveFileExporter() {}

void ArchiveFileExporter::ClearArchive() {
    ArchiveFile::ClearArchive(); // inherit
    dataPosition = 0; // reset data position for file offset
}

bool ArchiveFileExporter::AddFile(const std::string& path, const std::string& name, const std::string& type) {
    size_t flen = 0;
    {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if(!file.is_open()) return false;

        file.seekg(0, file.end);
        flen = file.tellg(); // get file length
        file.seekg(0, file.beg);

        file.close();
    }

    FileHeader fhead;
    fhead.name = name;
    fhead.type = type;
    fhead.filename = path;
    fhead.size = flen;
    fhead.offset = dataPosition;
    dataPosition += flen; // increment next file position
    
    fhead.buffer = nullptr; // read use only - if pointing to a buffer, use buffer instead of filename

    table.push_back(fhead);
    return true;
}

bool ArchiveFileExporter::SaveArchive(const std::string& path, const std::string& name) {

    header.archiveName = name;
    header.archiveTimestamp = GetTimestamp();
    header.archiveVersion = fileVersion;
    header.fileCount = table.size();

    std::ofstream output(path, std::ios::binary | std::ios::out);
    if(!output.is_open()) return false;
    buffer = &output;

    // export header
    if(!WriteHeader()){
        std::cout << "failed to write header to stream\n";
        goto on_failure;
    }
    
    // export file headers
    for(FileHeader& fhead : table) {
        if(!WriteFile(fhead)){
            std::cout << "failed to write file header [" << fhead.filename << "] to stream\n";
            goto on_failure;
        }
    }

    fileDataRegion = buffer->tellp();
    
    // export file data
    for(FileHeader& fhead : table) {
        if(fhead.buffer == nullptr){ // export from file stream
            std::ifstream file(fhead.filename, std::ios::binary | std::ios::in);
            if(!file.is_open()){
                std::cout << "failed to open file [" << fhead.filename << "] for output stream\n";
                goto on_failure;
            }
            size_t len;
            while(!file.eof()){
                char buf[4096];
                len = file.readsome(buf, 4096);
                if(!len) break;

                if(!WriteData(buf, len)){
                    std::cout << "failed to flush file [" << fhead.filename << "] to stream\n";
                    file.close();
                    goto on_failure;
                }
            }

            file.close();
        } else { // export from memory stream
            if(!WriteData(fhead.buffer, fhead.size)){
                std::cout << "failed to write file data [" << fhead.filename << "] to stream\n";
                goto on_failure;
            }
        }
    }

    output.close();
    return true;
    
    on_failure: // if archive exporter fails

    output.close();
    buffer = nullptr;
    return false;

}

bool ArchiveFileExporter::SignArchive(const std::string& signature) {
    header.archiveSignature = signature;
    return true;
}


bool ArchiveFileExporter::WriteHeader() {
    return (
        WriteUint64(header.archiveVersion) &&
        WriteString(header.archiveName) &&
        WriteString(header.archiveSignature) &&
        WriteUint64(header.archiveTimestamp) &&
        WriteUint64(header.fileCount)
    );
}
bool ArchiveFileExporter::WriteFile(FileHeader fheader) {
    return (WriteString(fheader.name) &&
            WriteString(fheader.type) &&
            WriteString(fs::path(fheader.filename).filename().string()) &&
            WriteUint64(fheader.size) &&
            WriteUint64(fheader.offset)
    );
}

bool ArchiveFileExporter::WriteString(const std::string& val) {
    if(buffer == nullptr) return false;
    *buffer << val << char(0); // null terminate strings
    return true;
}
bool ArchiveFileExporter::WriteUint64(uint64_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteUint32(uint32_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteUint16(uint16_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteUint8(uint8_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteInt64(int64_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteInt32(int32_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteInt16(int16_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteInt8(int8_t val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteFloat(float val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteDouble(double val) {
    if(buffer == nullptr) return false;
    buffer->write((char*)&val, sizeof(val));
    return true;
}
bool ArchiveFileExporter::WriteData(const char* data, size_t length) {
    if(buffer == nullptr) return false;
    buffer->write(data, length);
    return true;
}