#pragma once

#include <string>
#include <vector>

#undef UNICODE
#include "windows.h"

namespace freedialog {

    std::string EnvironmentVariable(const std::string &variable);


    class FileDialog {
    public:
        struct Filter {
            const char* label;
            const char* filter;
        };

        FileDialog(std::string &filePath, const std::string &path, const std::vector<Filter> &flt, int defaultFilter, DWORD flags, const std::string &title);
        virtual ~FileDialog();

    protected:
        void store(bool success);
        OPENFILENAME fileCtx;

    private:
        std::string& filePathRef;
        std::string strFilter;
        char szFile[MAX_PATH];
    };

    class SaveDialog: public FileDialog {
    public:
        SaveDialog(std::string &filePath,
                    const std::string &path="",
                    const std::vector<FileDialog::Filter> &flt = {{"All Files (*.*)","*.*"}},
                    int defaultFilter=0,
                    DWORD flags=OFN_OVERWRITEPROMPT,
                    const std::string &title="Save File");
        virtual ~SaveDialog();
    };

    class LoadDialog: public FileDialog {
    public:
        LoadDialog(std::string &filePath,
                    const std::string &path="",
                    const std::vector<FileDialog::Filter> &flt = {{"All Files (*.*)","*.*"}},
                    int defaultFilter=0,
                    DWORD flags=OFN_FILEMUSTEXIST,
                    const std::string &title="Open File");
        virtual ~LoadDialog();
    };

    class MessageDialog {
    public:
        MessageDialog(const std::string &message, const std::string &title="", UINT msgIcon=0);
        virtual ~MessageDialog();
        virtual int result(){ return _result; }
    protected:
        int _result;
    };


    class QuestionDialog: public MessageDialog {
    public:
        QuestionDialog(const std::string &message="",
                        const std::string &title="",
                        UINT msgIcon=0);
        virtual ~QuestionDialog();
        int result();
    };

}
