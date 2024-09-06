#include "time.h"
#include "framework.h"
#include "puzzle.h"
#include <set>

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WCHAR szSubTitle[MAX_LOADSTRING];               // текст над пазлом
WCHAR szYouWin[MAX_LOADSTRING];                  //текст при победе


//текст правил
WCHAR szRul0[MAX_LOADSTRING];
WCHAR szRul1[MAX_LOADSTRING];
WCHAR szRul2[MAX_LOADSTRING];
WCHAR szRul3[MAX_LOADSTRING];
WCHAR szRul4[MAX_LOADSTRING];
WCHAR szRul5[MAX_LOADSTRING];
WCHAR szRul6[MAX_LOADSTRING];
WCHAR szRul7[MAX_LOADSTRING];
WCHAR szRul8[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Answer(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Rules(HWND, UINT, WPARAM, LPARAM);


WNDCLASS PW;
//Основная картинка
HBITMAP BMShip, BMFon, BMHelpFon;
HBITMAP BMPole;
int w_bm, h_bm;
const int bord_left = 45, bord_top = 130;

//выбор уровня
int lvl, row, col;
int w_puzl, h_puzl;
int h_space, w_space;
int m, n, sdv;
HBITMAP done_puzl[50];
HBITMAP shuf_puzl[50];
HBITMAP cur_puzl[50];
POINTS  start_pos_puzl[50];
POINTS cur_pos_puzl[50];
bool choice_puzl[50];

//пазлы уровень 1
const int lvl1 = 6, row1 = 2, col1 = 3;
const int w_puzl1 = 223, h_puzl1 = 275;
const int h_space1 = 50, w_space1 = 30;
const int m1 = 4, n1 = 3;
const int sdv1 = 0;

//пазлы уровень 2
const int lvl2 = 30, row2 = 5, col2 = 6;
const int w_puzl2 = 112, h_puzl2 = 110;
const int h_space2 = 8, w_space2 = 8;
const int m2 = 7, n2 = 6, sdv2 = -3;

//вспомогательные
int numbers[50];
int field_position[50];
int x, y;
int x_pos, y_pos;
RECT main_pic, rpnt_puzl, rpnt;
bool ischoiced = false;
int step = 0;
COLORREF colorBord;
std::set<int> number_choice_puz;
std::set<int> number_in_field;
int this_puzl_ch = -1;
bool ruls = false;
HWND hpopup;
HMENU hPopupMenu;

//Инструменты
HBRUSH hOldBrush, hBrush;
HPEN hOldPen, hPen;
HFONT hFont, hOldFont;
RECT textRect;
RECT timeRect;
RECT timeRectmin;
int min = 0, sec = 0;
RECT minrect;
RECT secrect;

void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap);
void DrawPuzl(HDC hDC, int x, int y, int h, int w, HBITMAP hBitmap);
void CopyPart(HBITMAP BitmapDest, HBITMAP BitmapSrs, int srcX, int srcY, int width, int height);
void Shuffle(int level);
bool IsFinally();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    lvl = lvl1;
    row = row1;
    col = col1;
    h_puzl = h_puzl1;
    w_puzl = w_puzl1;
    h_space = h_space1, w_space = w_space1;
    int part = IDB_Part1;
    m = m1; n = n1; sdv = sdv1;

    hPopupMenu = CreatePopupMenu();
    AppendMenu(hPopupMenu, MF_STRING, TIMER_STOP, L"Пауза");      
    AppendMenu(hPopupMenu, MF_STRING, TIMER_CONTINUE, L"Продолжить");     
    AppendMenu(hPopupMenu, MF_STRING, TIMER_START, L"Начать отсчет сначала");  


    //картинки
    BMShip = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_SHIP), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    BMFon = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_CLEARFON), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    BMHelpFon = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_HELPCLEARFON), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    BITMAP bmpInfo = {};
    if (::GetObject(BMShip, sizeof(BITMAP), &bmpInfo) == sizeof(BITMAP)){
        w_bm = bmpInfo.bmWidth;
        h_bm = bmpInfo.bmHeight; 
    }
    
    main_pic.left = bord_left - 10;
    main_pic.right = bord_left + w_bm + 10;
    main_pic.bottom = bord_top - 10;
    main_pic.top = bord_top + h_bm + 10;

    timeRectmin.left = 450;
    timeRectmin.right = 500;
    timeRectmin.top = 50;
    timeRectmin.bottom = 90;

    timeRect.left = 595;
    timeRect.right = 645;
    timeRect.top = 50;
    timeRect.bottom = 90;

    SetRect(&secrect, 652, 50, 710, 90);
    SetRect(&minrect, 505, 50, 565, 90);

    HBITMAP tmp;
    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++) {
            tmp = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(part), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
            CopyPart(tmp, BMShip, w_puzl *j, h_puzl *i, w_puzl, h_puzl);
            done_puzl[i * col + j] = tmp;
        }

    for (int i = 0; i < lvl; i++)
        choice_puzl[i] = false;
    
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PUZZLE, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_SUBTITLE, szSubTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_YOUWIN, szYouWin, MAX_LOADSTRING);

    LoadString(hInst, IDS_RUL0, szRul0, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL1, szRul1, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL2, szRul2, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL3, szRul3, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL4, szRul4, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL5, szRul5, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL6, szRul6, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL7, szRul7, MAX_LOADSTRING);
    LoadString(hInst, IDS_RUL8, szRul8, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PUZZLE));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
    wcex.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(240, 240, 240));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PUZZLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MYICON));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW);
   ShowWindow(hWnd, SW_SHOWMAXIMIZED);
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND ButReplay;
    static HWND ButHelpFon;
    static HWND LVLComboBox;
    static HWND hListBox;
    static HWND ButOk;
    switch (message)
    {
    case WM_CREATE:
    {
        BMPole = BMFon;
        HDC hDC = GetDC(hWnd);
        hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(WHITE_BRUSH));
        hOldPen = (HPEN)SelectObject(hDC, GetStockObject(DC_PEN));
        hOldFont = (HFONT)SelectObject(hDC, GetStockObject(SYSTEM_FONT));
        ReleaseDC(hWnd, hDC);
        Shuffle(1);
        ButReplay = CreateWindowW(
            L"BUTTON",
            L"Начать заново",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            40,
            50,
            150,
            60,
            hWnd,
            reinterpret_cast<HMENU>(ID_BUTREPLAY),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
        ButHelpFon = CreateWindowW(
            L"BUTTON",
            L"Подсказка",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            1380,
            20,
            120,
            80,
            hWnd,
            reinterpret_cast<HMENU>(ID_HELPBUT),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL
        );
        LVLComboBox = CreateWindowW(L"COMBOBOX", TEXT(""),
            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
            230, 70, 200, 80, hWnd, reinterpret_cast<HMENU>(ID_LVLCOMBOBOX), hInst,
            NULL);
        TCHAR levels[2][10] = { TEXT("Простой"), TEXT("Сложный") };
        TCHAR A[16];
        memset(&A, 0, sizeof(A));
        for (int k = 0; k < 2; k ++)
        {
            wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)levels[k]);
            SendMessage(LVLComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
        }
        SetWindowTextA(LVLComboBox, "Уровень сложности");
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_OK:
            {
                ruls = false;
                InvalidateRect(hWnd, &rpnt, true);
                DestroyWindow(hListBox);
                DestroyWindow(ButOk);
            }
            break;
            case TIMER_STOP:
            {
                KillTimer(hWnd, IDT_TIMER1);
            }
            break;
            case TIMER_CONTINUE:
            {
                SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
            }
            break;
            case TIMER_START:
            {
                sec = 0;
                min = 0;
                KillTimer(hWnd, IDT_TIMER1);
                SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
            }
            break;
            case ID_BUTREPLAY:
            {
                Shuffle(1);
                step = 0;
                number_choice_puz.clear();
                number_in_field.clear();
                this_puzl_ch = -1;
                for (int i = 0; i < lvl; i++) {
                    choice_puzl[i] = false;
                    cur_puzl[i] = NULL;
                    cur_pos_puzl[i].x = 0;
                    cur_pos_puzl[i].y = 0;
                }
                InvalidateRect(hWnd, NULL, true);
                sec = 0;
                min = 0;
                SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
            }
            break;
            case IDM_ANSWER:
            {
                CreateDialogW(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Answer);
            }
            break;
            
            case ID_HELPBUT:
            {
                if (SendDlgItemMessage(hWnd, ID_HELPBUT, BM_GETCHECK, 0, 0))
                    BMPole = BMHelpFon;
                else 
                    BMPole = BMFon;
                InvalidateRect(hWnd, &main_pic, false);
            }
            break;
            case ID_LVLCOMBOBOX:
            {
                    int check_int = SendMessage(LVLComboBox, CB_GETCURSEL, 0, 0L);
                    if (check_int == 0 && lvl != lvl1)
                    {
                        lvl = lvl1;
                        row = row1;
                        col = col1;
                        h_puzl = h_puzl1;
                        w_puzl = w_puzl1;
                        h_space = h_space1, w_space = w_space1;
                        int part = IDB_Part1;
                        m = m1; n = n1; sdv = sdv1;
                        HBITMAP tmp;
                        for (int i = 0; i < row; i++)
                            for (int j = 0; j < col; j++) {
                                tmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(part), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                                CopyPart(tmp, BMShip, w_puzl * j, h_puzl * i, w_puzl, h_puzl);
                                done_puzl[i * col + j] = tmp;
                            }
                        PostMessage(ButReplay, WM_LBUTTONDOWN, NULL, NULL);
                        PostMessage(ButReplay, WM_LBUTTONUP, NULL, NULL);
                    }

                    if (check_int == 1 && lvl != lvl2)
                    {
                        lvl = lvl2;
                        row = row2;
                        col = col2;
                        h_puzl = h_puzl2;
                        w_puzl = w_puzl2;
                        h_space = h_space2, w_space = w_space2;
                        int part = IDB_Part2;
                        m = m2; n = n2; sdv = sdv2;
                        HBITMAP tmp;
                        for (int i = 0; i < row; i++)
                            for (int j = 0; j < col; j++) {
                                tmp = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(part), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
                                CopyPart(tmp, BMShip, w_puzl * j, h_puzl * i, w_puzl, h_puzl);
                                done_puzl[i * col + j] = tmp;
                            }
                        PostMessage(ButReplay, WM_LBUTTONDOWN, NULL, NULL);
                        PostMessage(ButReplay, WM_LBUTTONUP, NULL, NULL);
                    }
            }
            break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_RULES:
            {                
                hListBox = CreateWindowW(L"listbox", NULL,
                    WS_CHILD | WS_VISIBLE | LBS_NOTIFY| WS_BORDER|WS_VSCROLL| 
                    LBS_WANTKEYBOARDINPUT| LBS_DISABLENOSCROLL,
                    400, 230, 750, 200,
                    hWnd, (HMENU)140, hInst, NULL);
                WCHAR Space[20]{};
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul0);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul1);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul2);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul3);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul4);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul5);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul6);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul7);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szRul8);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)Space);
                ButOk = CreateWindowW(L"Button", L"Ок", BS_DEFPUSHBUTTON |WS_VISIBLE | WS_CHILD, 740, 430, 40, 40, hWnd, reinterpret_cast<HMENU>(IDM_OK), hInst, NULL);
                SetRect(&rpnt, 300, 200, 1200, 600);
                ruls = true;
                InvalidateRect(hWnd, &rpnt, false);
                
            }
            break;
            case IDM_EXIT:
                KillTimer(hWnd, IDT_TIMER1);
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CONTEXTMENU:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (pt.x == -1 && pt.y == -1)
        {
            // Вызов с клавиатуры
            RECT rect;
            GetWindowRect(hWnd, &rect);
            // Выводим меню радом с левым верхним углом окна
            pt.x = rect.left + 5;
            pt.y = rect.top + 5;
        }

        TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
    }
    break;
    case WM_TIMER:
    {
        sec++;
        if (sec % 60 == 0)
        {
            sec = 0;
            min++;
            InvalidateRect(hWnd, &timeRectmin, true);
        }
        InvalidateRect(hWnd, &timeRect, true);
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);           
            static LOGFONT lf;
            lf.lfCharSet = DEFAULT_CHARSET;
            lf.lfPitchAndFamily = DEFAULT_PITCH;
            lf.lfHeight = 50;
            lf.lfWidth = 20;
            lf.lfWeight = 50;
            lf.lfEscapement = 0;
            hFont = CreateFontIndirect(&lf);
            hOldFont = (HFONT)SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(0, 0, 255));
            SetBkColor(hdc, RGB(240, 240, 240));
            SetRect(&textRect, 1200, 20, 700, 100);
            //текст
            if (step == lvl && IsFinally())
                DrawText(hdc, szYouWin, -1, &textRect, DT_CENTER | DT_NOCLIP);
            else
                DrawText(hdc, szSubTitle, -1, &textRect, DT_CENTER | DT_NOCLIP);
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);

            //рамка и обновление
            if (step == lvl && IsFinally())
            {
                colorBord = RGB(0, 255, 0);
                KillTimer(hWnd, IDT_TIMER1);
            } 
            else {
                colorBord = RGB(255, 0, 0);
                if (step == lvl) {
                    Shuffle(1);
                    step = 0;
                    number_choice_puz.clear();
                    number_in_field.clear();
                    this_puzl_ch = -1;
                    for (int i = 0; i < lvl; i++) {
                        choice_puzl[i] = false;
                        cur_puzl[i] = NULL;
                        cur_pos_puzl[i].x = 0;
                        cur_pos_puzl[i].y = 0;
                    }
                }
            }
            hPen = CreatePen(PS_SOLID, 5, colorBord);
            hOldPen = (HPEN)SelectObject(hdc, hPen);
            Rectangle(hdc, bord_left - 5, bord_top - 5, bord_left + 5 + w_bm, bord_top + 5 + h_bm);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
            
            //картинка фона
            DrawBitmap(hdc, bord_left, bord_top, BMPole);

            //детали пазлов
            for (int i = 0; i < row; i++)
                for (int j = 0; j < col; j++) { 
                    if (number_choice_puz.find(i * col + j) == number_choice_puz.end()) {
                        //если клик не на поле
                        x = 750 + w_puzl * j + w_space * j;
                        y = 100 + h_puzl * i + h_space * i;
                        start_pos_puzl[i * col + j].x = x;
                        start_pos_puzl[i * col + j].y = y;
                        DrawBitmap(hdc, x, y, shuf_puzl[i * col + j]);
                    }
                    else //перемещение на поле 
                    {
                        x = cur_pos_puzl[i * col + j].x;
                        y = cur_pos_puzl[i * col + j].y;
                        DrawBitmap(hdc, x, y, shuf_puzl[i * col + j]);
                    }
                }

            //контуры деталей
            for (int i = 0; i < lvl; i++) {
                if (this_puzl_ch > -1 && choice_puzl[i]) {

                    hPen = CreatePen(PS_SOLID, 5, RGB(0, 255, 0));
                    hOldPen = (HPEN)SelectObject(hdc, hPen);

                    for (int j = 0; j < n; j++) {
                        MoveToEx(hdc, bord_left + sdv + w_puzl * 0, bord_top + h_puzl * j, NULL);
                        LineTo(hdc, bord_left + sdv + w_puzl * col, bord_top + h_puzl * j);
                    }

                    for (int j = 0; j < m; j++) {
                        MoveToEx(hdc, bord_left + sdv + w_puzl * j, bord_top, NULL);
                        LineTo(hdc, bord_left + sdv + w_puzl * j, bord_top + h_puzl * row);
                    }

                    MoveToEx(hdc, start_pos_puzl[i].x, start_pos_puzl[i].y, NULL);
                    LineTo(hdc, start_pos_puzl[i].x, start_pos_puzl[i].y + h_puzl);

                    MoveToEx(hdc, start_pos_puzl[i].x, start_pos_puzl[i].y, NULL);
                    LineTo(hdc, start_pos_puzl[i].x + w_puzl, start_pos_puzl[i].y);

                    MoveToEx(hdc, start_pos_puzl[i].x + w_puzl, start_pos_puzl[i].y, NULL);
                    LineTo(hdc, start_pos_puzl[i].x + w_puzl, start_pos_puzl[i].y + h_puzl);

                    MoveToEx(hdc, start_pos_puzl[i].x, start_pos_puzl[i].y + h_puzl, NULL);
                    LineTo(hdc, start_pos_puzl[i].x + w_puzl, start_pos_puzl[i].y + h_puzl);

                    SelectObject(hdc, hOldPen);
                    DeleteObject(hPen);
                }
            }
            //правила
            if (ruls)
            {
                hBrush = CreateSolidBrush(RGB(207, 159, 255));
                hPen = CreatePen(PS_SOLID, 4, RGB(128, 0, 255));
                hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                hOldPen = (HPEN)SelectObject(hdc, hPen);
                Rectangle(hdc, 350, 200, 1200, 480);
                SelectObject(hdc, hOldBrush);
                SelectObject(hdc, hOldPen);
                DeleteObject(hPen);
                DeleteObject(hBrush);
            }
       
            //вывод времени
            hFont = CreateFontIndirect(&lf);
            hOldFont = (HFONT)SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(255, 0, 0));
            SetBkColor(hdc, RGB(240, 240, 240));            
            DrawTextA(hdc, "мин", -1, &minrect, DT_CENTER | DT_NOCLIP);           
            DrawTextA(hdc, "сек", -1, &secrect, DT_CENTER | DT_NOCLIP);
            char buffer1[20];
            _itoa_s(sec, buffer1, 20, 10);
            LPCSTR p = buffer1;
            DrawTextA(hdc, p, -1, &timeRect, DT_CENTER | DT_NOCLIP);
            char buffer2[20];
            _itoa_s(min, buffer2, 20, 10);
            LPCSTR q = buffer2;
            DrawTextA(hdc, q, -1, &timeRectmin, DT_CENTER | DT_NOCLIP);
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            EndPaint(hWnd, &ps);
        }
        break;
        
    case WM_LBUTTONDOWN:
    {
        x_pos = LOWORD(lParam);
        y_pos = HIWORD(lParam);
        bool is_puz = false;
        //если ничего не выбрано
        if (!(this_puzl_ch > -1)) {
            for (int i = 0; i < row; i++)
                for (int j = 0; j < col; j++)
                    if ((x_pos > 750 + w_puzl * j + w_space * j) && (x_pos < w_puzl + 750 + w_puzl * j + w_space * j) && (y_pos > 100 + h_puzl * i + h_space * i) && (y_pos < w_puzl + 100 + w_puzl * i + h_space * i)) {
                        if (step == 0)
                        {
                            sec = 0;
                            min = 0;
                            SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
                        }
                            
                        this_puzl_ch = i * col + j;    
                        choice_puzl[this_puzl_ch] = true;
                        rpnt_puzl.left = start_pos_puzl[this_puzl_ch].x - 5;
                        rpnt_puzl.right = start_pos_puzl[this_puzl_ch].x + w_puzl + 5;
                        rpnt_puzl.top = start_pos_puzl[this_puzl_ch].y - 5;
                        rpnt_puzl.bottom = start_pos_puzl[this_puzl_ch].y + h_puzl + 5;
                        InvalidateRect(hWnd, &rpnt_puzl, true);
                        InvalidateRect(hWnd, &main_pic, true);   
                    }
        }
        //если выбрана деталь
        else {
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) { //затем опять выбрали деталь
                    if ((x_pos > 750 + w_puzl * j + w_space * j) && (x_pos < w_puzl + 750 + w_puzl * j + w_space * j) && (y_pos > 100 + h_puzl * i + h_space * i) && (y_pos < h_puzl + 100 + h_puzl * i + h_space * i)) {
                        //перерисовка старой делати
                        choice_puzl[this_puzl_ch] = false;
                        rpnt_puzl.left = start_pos_puzl[this_puzl_ch].x - 5;
                        rpnt_puzl.right = start_pos_puzl[this_puzl_ch].x + w_puzl + 5;
                        rpnt_puzl.top = start_pos_puzl[this_puzl_ch].y - 5;
                        rpnt_puzl.bottom = start_pos_puzl[this_puzl_ch].y + h_puzl + 5;
                        InvalidateRect(hWnd, &rpnt_puzl, true);

                        //перерисовка новой детали
                        if (this_puzl_ch != i * col + j) {
                            this_puzl_ch = i * col + j;
                            choice_puzl[this_puzl_ch] = true;
                            rpnt_puzl.left = start_pos_puzl[this_puzl_ch].x - 5;
                            rpnt_puzl.right = start_pos_puzl[this_puzl_ch].x + w_puzl + 5;
                            rpnt_puzl.top = start_pos_puzl[this_puzl_ch].y - 5;
                            rpnt_puzl.bottom = start_pos_puzl[this_puzl_ch].y + h_puzl + 5;
                            InvalidateRect(hWnd, &rpnt_puzl, true);
                        }
                        else { //если выбрали ту же самую деталь
                            this_puzl_ch = -1;
                            InvalidateRect(hWnd, &main_pic, true);
                        }
                        is_puz = true;
                    } //нажатие на поле с картинкой
                    if ((x_pos > bord_left + w_puzl * j) && (x_pos < bord_left + w_puzl * (j + 1)) && (y_pos > bord_top + h_puzl * i) && (y_pos < bord_top + h_puzl * (i + 1))) {
                            //перерисовка части поля с картинкой
                            if (number_in_field.find(i*col+j) == number_in_field.end())
                            { 
                                //перерисовка старой детали
                                if (this_puzl_ch > -1) {
                                    number_choice_puz.insert(this_puzl_ch);
                                    choice_puzl[this_puzl_ch] = false;
                                    rpnt_puzl.left = start_pos_puzl[this_puzl_ch].x - 5;
                                    rpnt_puzl.right = start_pos_puzl[this_puzl_ch].x + w_puzl + 5;
                                    rpnt_puzl.top = start_pos_puzl[this_puzl_ch].y - 5;
                                    rpnt_puzl.bottom = start_pos_puzl[this_puzl_ch].y + h_puzl + 5;
                                    InvalidateRect(hWnd, &rpnt_puzl, true);
                                    cur_pos_puzl[this_puzl_ch].x = bord_left + w_puzl * j;
                                    cur_pos_puzl[this_puzl_ch].y = bord_top + h_puzl * i;
                                    is_puz = true;
                                }
                                InvalidateRect(hWnd, &main_pic, true);
                                number_in_field.insert(i*col+j);
                                if (this_puzl_ch > -1 && this_puzl_ch < lvl)
                                    cur_puzl[i * col + j] = shuf_puzl[this_puzl_ch];
                                field_position[i * col + j] = this_puzl_ch;
                                step++;
                                if (step == lvl)
                                    InvalidateRect(hWnd, NULL, true);
                                this_puzl_ch = -1;
                            }
                            else { //если пытаемся поставить деталь на поле, где уже стоит деталь (обмен)
                                number_choice_puz.erase(field_position[i * col + j]);
                                number_choice_puz.insert(this_puzl_ch);
                                cur_puzl[i * col + j] = shuf_puzl[this_puzl_ch];
                                field_position[i * col + j] = this_puzl_ch;
                                choice_puzl[field_position[i * col + j]] = false;
                                choice_puzl[this_puzl_ch] = false;
                                cur_pos_puzl[this_puzl_ch].x = bord_left + w_puzl * j;
                                cur_pos_puzl[this_puzl_ch].y = bord_top + h_puzl * i;
                                this_puzl_ch = -1;
                                is_puz = true;
                                InvalidateRect(hWnd, NULL, true);      
                            }
                    }
                }
            }
            if (!is_puz){ //нажатие на пустое поле
                choice_puzl[this_puzl_ch] = false;
                rpnt_puzl.left = start_pos_puzl[this_puzl_ch].x - 5;
                rpnt_puzl.right = start_pos_puzl[this_puzl_ch].x + w_puzl + 5;
                rpnt_puzl.top = start_pos_puzl[this_puzl_ch].y - 5;
                rpnt_puzl.bottom = start_pos_puzl[this_puzl_ch].y + h_puzl + 5;
                InvalidateRect(hWnd, &rpnt_puzl, true);
                InvalidateRect(hWnd, &main_pic, true);
                this_puzl_ch = -1;
            }
        }
    }
    break;
    case WM_LBUTTONDBLCLK:
    {
        x_pos = LOWORD(lParam);
        y_pos = HIWORD(lParam);
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                if ((x_pos > bord_left + w_puzl * j) && (x_pos < bord_left + w_puzl * (j + 1)) && (y_pos > bord_top + h_puzl * i) && (y_pos < bord_top + h_puzl * (i + 1)))
                    if (number_in_field.find(i * col + j) != number_in_field.end())
                    {
                        number_in_field.erase(i* col + j);
                        number_choice_puz.erase(field_position[i * col + j]);
                        cur_puzl[i * col + j] = NULL;
                        field_position[i * col + j] = NULL;
                        choice_puzl[field_position[i * col + j]] = false;
                        rpnt_puzl.left = start_pos_puzl[field_position[i * col + j]].x - 5;
                        rpnt_puzl.right = start_pos_puzl[field_position[i * col + j]].x + w_puzl + 5;
                        rpnt_puzl.top = start_pos_puzl[field_position[i * col + j]].y - 5;
                        rpnt_puzl.bottom = start_pos_puzl[field_position[i * col + j]].y + h_puzl + 5;
                        this_puzl_ch = -1;
                        step--;
                        InvalidateRect(hWnd, NULL, false); 
                    }
    }
    break;
    case WM_CLOSE:
    {
        if (MessageBox(hWnd, L"Действительно хотите выйти из игры?", L"Подтверждение выхода", MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hWnd);
        }
    }
    break;
    case WM_DESTROY: 
    {
        HDC hDC = GetDC(hWnd);
        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);
        DeleteObject(hOldBrush);
        DeleteObject(hOldPen);
        ReleaseDC(hWnd, hDC);
        PostQuitMessage(0);
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND Text1;
    static HWND Text2;
    static HWND Name;
    static HWND Group;
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        Text1 = CreateWindowW(
            L"Static",
            L"Лабораторная работа №0",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
            50,
            30,
            350,
            40,
            hDlg,
            reinterpret_cast<HMENU>(1),
            (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
            NULL);
        Text2 = CreateWindowW(
            L"Static",
            L"Игра ""Пазлы""",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
            10,
            60,
            450,
            40,
            hDlg,
            reinterpret_cast<HMENU>(1),
            (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
            NULL);
        Name = CreateWindowW(
            L"Static",
            L"Зайцева Ирина",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
            260,
            130,
            200,
            50,
            hDlg,
            reinterpret_cast<HMENU>(1),
            (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
            NULL);
        Group = CreateWindowW(
            L"Static",
            L"Группа А-13а-20",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_CENTER,
            260,
            170,
            200,
            50,
            hDlg,
            reinterpret_cast<HMENU>(1),
            (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE),
            NULL);
        return (INT_PTR)TRUE;
    }
    case WM_PAINT:
    {
        HBITMAP img = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MYPUZL));
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);
        DrawBitmap(hdc, 40, 110, img);
        EndPaint(hDlg, &ps);
    }
    break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;      
    }
    return (INT_PTR)FALSE;
}

// Обработчик сообщений для окна отзывы
INT_PTR CALLBACK Answer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
    HBITMAP hOldbm;
    HDC hMemDC;
    BITMAP bm;
    POINT  ptSize;

    // Создаем устройство для работы с картой
    hMemDC = CreateCompatibleDC(hDC);

    // Выбираем изображение bitmap в контекст памяти
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);

    if (hOldbm)
    {
        SetMapMode(hMemDC, GetMapMode(hDC));
        // Определяем размеры изображения
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);

        ptSize.x = bm.bmWidth;
        ptSize.y = bm.bmHeight;

        // Рисуем изображение bitmap
        BitBlt(hDC, x, y, ptSize.x, ptSize.y, hMemDC, 0, 0, SRCCOPY);

        // Восстанавливаем контекст памяти
        SelectObject(hMemDC, hOldbm);
    }
    // Удаляем контекст памяти
    DeleteDC(hMemDC);
}

void DrawPuzl(HDC hDC, int x, int y, int w, int h, HBITMAP hBitmap)
{
    HBITMAP hOldbm;
    HDC hMemDC;

    // Создаем устройство для работы с картой
    hMemDC = CreateCompatibleDC(hDC);

    // Выбираем изображение bitmap в контекст памяти
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);

    if (hOldbm)
    {
        SetMapMode(hMemDC, GetMapMode(hDC));

        // Рисуем изображение bitmap
        BitBlt(hDC, x, y, w, h, hMemDC, 0, 0, SRCCOPY);

        // Восстанавливаем контекст памяти
        SelectObject(hMemDC, hOldbm);
    }
    // Удаляем контекст памяти
    DeleteDC(hMemDC);
}

void CopyPart(HBITMAP BitmapDest, HBITMAP BitmapSrs, int srcX, int srcY, int width, int height)
{
    // Получаем контекст устройства (Device Context) для Bitmap1
    HDC hdcSrc = CreateCompatibleDC(NULL);
    SelectObject(hdcSrc, BitmapSrs);

    // Получаем контекст устройства (Device Context) для Bitmap2
    HDC hdcDest = CreateCompatibleDC(NULL);
    SelectObject(hdcDest, BitmapDest);

    // Выполняем копирование из Bitmap1 в Bitmap2
    BitBlt(hdcDest, 0, 0, width, height, hdcSrc, srcX, srcY, SRCCOPY);

    // Освобождаем контексты устройств
    DeleteDC(hdcSrc);
    DeleteDC(hdcDest);
};

void Shuffle(int level = 1)
{
    srand((unsigned)time(0));
    std::set<int> sdigit;
    if (level == 1) {
        int k;
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++) {
                k = rand() % lvl;
                while (sdigit.find(k) != sdigit.end()) //есть в массиве
                    k = rand() % lvl;
                sdigit.insert(k);
                shuf_puzl[i * col + j] = done_puzl[k];
                numbers[i * col + j] = k;
            }
    }
}

bool IsFinally()
{
    for (int i = 0; i < lvl; i++)
        if (done_puzl[i] != cur_puzl[i])
            return false;
    return true;
}