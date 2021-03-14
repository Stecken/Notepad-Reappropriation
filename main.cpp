#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
//debug libs
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <windows.h>

#define MARTELO 13

#define IDM_MYMENURESOURCE   3

#define IDM_FILE_NEW 0
#define IDM_FILE_OPEN 1
#define IDM_FILE_SAVE 2
#define IDM_FILE_QUIT 3
#define IDM_FILE_FILE 4

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
HMENU hMenu;
HWND hWndEdit;
HINSTANCE hinst;
HFONT hfont;
LOGFONT lf;
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
void addMenus(HWND hWnd);
void openFile(HWND hWnd);
void saveFile(HWND hWnd);
void addControls(HWND hWnd);
void displayFile(char *pathFile);
void cleanExit();
void addFont();
void resizeWindow(HWND hWnd);


HICON icon;

char *data;
FILE *file;

int WINAPI WinMain (HINSTANCE hinstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    icon = LoadIcon(hinstance, MAKEINTRESOURCE(11));
    MSG msg = { };  // message
    WNDCLASS wc;    // windowclass data
    HWND hwnd;      // handle to the main window

    // Create the window class for the main window. Specify
    // the identifier of the menu-template resource as the
    // lpszMenuName member of the WNDCLASS structure to create
    // the class menu.

    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(11));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) 0;
    wc.lpszMenuName =  MAKEINTRESOURCE(IDM_MYMENURESOURCE);
    wc.lpszClassName = "MainWClass";

    if (!RegisterClass(&wc))
        return FALSE;

    hinst = hinstance;

    // Create the main window. Set the hmenu parameter to NULL so
    // that the system uses the class menu for the window.

    hwnd = CreateWindow("MainWClass", "SteckenNotepad",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hinstance,
        NULL);

    if (hwnd == NULL)
        return FALSE;

    // Make the window visible and send a WM_PAINT message to the
    // window procedure.

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&msg, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&msg);
        /* Send message to WindowProcedure */
        DispatchMessage(&msg);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
        UNREFERENCED_PARAMETER(hPrevInstance);
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_COMMAND:

            // Test for the identifier of a command item.

            switch(LOWORD(wParam))
            {
                case IDM_FILE_NEW:
                    break;

                case IDM_FILE_OPEN:
                    openFile(hwnd);
                    break;
                case IDM_FILE_SAVE:
                    saveFile(hwnd);
                    break;
                case IDM_FILE_QUIT:
                    cleanExit();
                    PostQuitMessage (0);
                    break;
                default:
                    break;

            }
            return 0;
        case WM_SIZE:
            resizeWindow(hwnd);
            break;
        case WM_SIZING:
            resizeWindow(hwnd);
            break;
        case WM_CREATE:
            addMenus(hwnd);
            addControls(hwnd);
            break;
        case WM_DESTROY:
            cleanExit();
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


void addMenus(HWND hWnd) {
    hMenu = CreateMenu();

    HMENU hFileMenu = CreateMenu();
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_NEW, "Novo");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_OPEN, "Abrir...");
    AppendMenu(hFileMenu, MF_STRING, IDM_FILE_SAVE, "Salvar");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR) hFileMenu, "Arquivo");
    AppendMenu(hMenu, MF_STRING, IDM_FILE_QUIT, "Sair");
    SetMenu(hWnd, hMenu);
}


void openFile(HWND hWnd) {
    OPENFILENAME ofile;

    char filename[100];

    ZeroMemory(&ofile, sizeof(OPENFILENAME));

    ofile.lStructSize = sizeof(OPENFILENAME);
    ofile.hwndOwner = hWnd;
    ofile.lpstrFile = filename;
    ofile.lpstrFile[0] = '\0';
    ofile.nMaxFile = 100;
    ofile.lpstrFilter = "All files\0*.*\0Text Files\0*.TXT*\0";
    ofile.nFilterIndex = 1;

    GetOpenFileName(&ofile);
    displayFile(ofile.lpstrFile);
}

void saveFile(HWND hWnd){
    int _size = GetWindowTextLength(hWndEdit);

    char *contentSave = (char *)malloc(sizeof(char) * (_size + 1));

    SendMessage(hWndEdit, WM_GETTEXT, (WPARAM)_size + 1, (LPARAM)contentSave);
    if(file != NULL){
        printf("ainda existe\n");
    }
    if (ftruncate(fileno(file), 1) != 0){
        printf("\ne isso\n");
    }
    rewind(file);
    int resulf = fwrite(contentSave, sizeof(char), _size + 1, file);
    fclose(file);
    if(file != NULL){
        printf("\nnao existe dps\n");
    }
    printf("%d, %s\n result fwrite: %d", _size, contentSave, resulf);
    free(contentSave);
}

void addControls(HWND hWnd){
    RECT rect;
    int width, height;
    if(GetWindowRect(hWnd, &rect)){
      width = rect.right - rect.left;
      height = rect.bottom - rect.top;
    }

    hfont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);
    hWndEdit = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE| WS_VSCROLL | ES_AUTOVSCROLL, 0, 0, width - 10, height, hWnd, NULL, NULL, NULL);
    addFont();
}

void displayFile(char *pathFile){
    file = fopen(pathFile, "r+b");
    fseek(file, 0, SEEK_END);
    int _size = ftell(file);
    rewind(file);
    data = (char *)malloc(_size + 1);
    fread(data, _size, 1, file);

    SetWindowText(hWndEdit, data);
}

void addFont(){
    GetObject(GetStockObject(LTGRAY_BRUSH | BLACK_BRUSH | BLACK_PEN), sizeof(LOGFONT), &lf);
	hfont = CreateFont(lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, VARIABLE_PITCH,TEXT("Arial"));
	SendMessage(hWndEdit, WM_SETFONT, WPARAM (hfont), TRUE);
}

void resizeWindow(HWND hWnd) {
    RECT rect;
    int width, height;
    if(GetWindowRect(hWnd, &rect)){
      width = rect.right - rect.left;
      height = rect.bottom - rect.top;
    }
    SetWindowPos(hWndEdit, NULL, 0, 0, width - 15, height - 50, SWP_NOMOVE|SWP_NOZORDER);
}

void cleanExit(){
    fclose(file);
    free(data);
}
