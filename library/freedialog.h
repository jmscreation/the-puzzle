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

}

#endif // __FREE_DIALOG_H__