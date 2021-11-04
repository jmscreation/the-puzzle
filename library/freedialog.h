#ifndef __FREE_DIALOG_H__
#define __FREE_DIALOG_H__

#include "windows.h"
#include "commctrl.h"
#include <string>
#include <vector>

namespace freedialog {

    class Form {
        const char* CLASS_NAME;
        HINSTANCE hInstance;
        WNDCLASS winClass;

        HWND hWnd;
        COLORREF wincolor, textcolor, fontcolor;
        HBRUSH wincolorbrush;

        std::vector<std::string> results;
        bool allowNoValue, userAbort;

        
        static const HCURSOR DefaultCursor;

        static LRESULT CALLBACK winHandle(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static Form* getThis(HWND hwnd);

        void CreateDlg(const char* title, int width, int height, bool hasbar = true);
        void AddButton(ULONG64 id, const char* text, int x, int y, int width, int height, bool defaultbtn=false);
        void AddTextField(ULONG64 id, const char* value, int x, int y, int width, int height, char pwd=0);
        void AddIPField(ULONG64 id, const char* value, int x, int y, int width, int height);
        void AddTextLabel(ULONG64 id, const char* text, int x, int y, int width, int height);

        void HandleMessages();

        void CloseWindow();
        void Submit();

        void KeyUp(WPARAM key);
        void KeyDown(WPARAM key);
        void ButtonClick(UINT id, HWND button);

    public: // User declarations
        Form();
        virtual ~Form();

        void SetWindowColor(COLORREF col);
        void SetLabelColor(COLORREF col);
        void SetFontColor(COLORREF col);

        void FormCreate(const char* label = "", const char* title = "", const char* defvalue = "", int width = 400, int height = 200,
                        bool titlebar = true, char pwd = 0, bool cancelbtn = false, bool isIP = false);
        
        std::string GetResult(size_t index = 0);
        inline bool GetUserAbort() { return userAbort; }
    };

    void setWindowColor(COLORREF col);
    void setTextColor(COLORREF col);
    void setFontColor(COLORREF col);
    
    bool getInput(std::string& rvalue, const std::string& title, const std::string& caption = "", const std::string& def = "", int width = 400, int height = 200, char passChar = 0, bool cancelbtn = false);
    bool getInputRequired(std::string& rvalue, const std::string& title, const std::string& def = "", int width = 400, int height = 200, char passChar = 0, bool cancelbtn = false);
    bool getIPAddress(std::string& rvalue, const std::string& title, const std::string& caption = "", const std::string& def = "0.0.0.0", int width = 400, int height = 200, bool required = false, bool cancelbtn = false);

    std::string getBasicInput(const std::string& title, const std::string& caption = "", const std::string& def = "");
    std::string getBasicPassword(const std::string& title, const std::string& caption = "", const std::string& def = "", char passChar = '*');




    enum MSGTYPE : UINT {
        // ICON

        Information         = MB_ICONINFORMATION,
        Question            = MB_ICONQUESTION,
        Exclamation         = MB_ICONEXCLAMATION,
        Asterisk            = MB_ICONASTERISK,
        Warning             = MB_ICONWARNING,
        Stop                = MB_ICONSTOP,
        Error               = MB_ICONERROR,
        Mask                = MB_ICONMASK,
        Hand                = MB_ICONHAND,
    
        // INPUT

        YesNoCancel         = MB_YESNOCANCEL,
        YesNo               = MB_YESNO,
        OkCancel            = MB_OKCANCEL,
        OkHelp              = MB_HELP, // does not close dialog box - this sends WM_HELP event to the parent window - see docu https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
        OnlyOk              = MB_OK,
        AbortRetryIgnore    = MB_ABORTRETRYIGNORE,
        RetryCancel         = MB_RETRYCANCEL,
        CancelTryContinue   = MB_CANCELTRYCONTINUE,

        //RESULT

        Ok                  = IDOK,
        Yes                 = IDYES,
        No                  = IDNO,
        Cancel              = IDCANCEL,
        Help                = IDHELP, // see MB_HELP comment above
        Close               = IDCLOSE,
        Abort               = IDABORT,
        Retry               = IDRETRY,
        Ignore              = IDIGNORE,
        Continue            = IDCONTINUE,
        TryAgain            = IDTRYAGAIN
    };

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
        MessageDialog(const std::string &message, const std::string &title="", UINT msgType=OnlyOk);
        virtual ~MessageDialog();
        virtual int result(){ return _result; }
    protected:
        int _result;
    };


    class QuestionDialog: public MessageDialog {
    public:
        QuestionDialog(const std::string &message="",
                        const std::string &title="",
                        UINT msgType=YesNo|Question);
        virtual ~QuestionDialog();
        int result();
    };
}

#endif // __FREE_DIALOG_H__