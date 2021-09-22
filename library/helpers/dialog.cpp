#include "dialog.h"

namespace freedialog {

    std::string EnvironmentVariable(const std::string &variable){
        char data[5096];
        GetEnvironmentVariable(variable.data(), data, 5096); //returns length
        return std::string(data);
    }

    FileDialog::FileDialog(std::string &filePath, const std::string &path,const std::vector<FileDialog::Filter> &flt, int defaultFilter, DWORD flags, const std::string &title):
            filePathRef(filePath) {
        ZeroMemory(&fileCtx, sizeof(fileCtx));
        fileCtx.lStructSize = sizeof(fileCtx);
        fileCtx.nMaxFile = MAX_PATH;
        fileCtx.lpstrFile = szFile;
        fileCtx.lpstrFile[0] = '\0';

        for(int i=0;i<flt.size();i++)
            strFilter += std::string(flt[i].label) + '\0' + flt[i].filter + '\0';

        fileCtx.lpstrFilter = strFilter.data();
        fileCtx.nFilterIndex = defaultFilter+1;
        fileCtx.lpstrInitialDir = path.data();
        fileCtx.Flags = flags;
        fileCtx.lpstrTitle = title.data();
    }

    FileDialog::~FileDialog() {}

    void FileDialog::store(bool success){
        if(success) filePathRef = fileCtx.lpstrFile;
    }

    SaveDialog::SaveDialog(std::string &filePath,
                            const std::string &path,
                            const std::vector<FileDialog::Filter> &flt,
                            int defaultFilter,
                            DWORD flags,
                            const std::string &title):
            FileDialog(filePath, path, flt, defaultFilter, flags, title) {
        store(GetSaveFileName(&fileCtx));
    }

    SaveDialog::~SaveDialog() {}

    LoadDialog::LoadDialog(std::string &filePath,
                            const std::string &path,
                            const std::vector<FileDialog::Filter> &flt,
                            int defaultFilter,
                            DWORD flags,
                            const std::string &title):
            FileDialog(filePath, path, flt, defaultFilter, flags, title) {
        store(GetOpenFileName(&fileCtx));
    }

    LoadDialog::~LoadDialog() {}


    MessageDialog::MessageDialog(const std::string &message,const std::string &title,UINT msgIcon){
        _result = MessageBox(NULL, message.data(), title.data(), msgIcon);
    }

    MessageDialog::~MessageDialog(){}


    QuestionDialog::QuestionDialog(const std::string &message,
                                    const std::string &title,
                                    UINT msgIcon):
            MessageDialog(message, title, MB_YESNO | msgIcon){}

    QuestionDialog::~QuestionDialog(){}

    int QuestionDialog::result(){
        switch(_result){
            case IDYES: return 1;
            case IDNO:  return 0;
            default: return 0;
        }
    }

}
