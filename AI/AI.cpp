// AI.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "AI.h"
#include "coordinate.h"
#include <queue>
using namespace std;

#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
POINT **p = NULL, s, e;
static int*c, delta = 120, scale = 1;
int n = 0;
vector<pointOwner> pArray;
vector<POINT> path;
bool existFlag = false;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				deAlloc();
void				draw(HWND);
void				readFile(HWND);
void				createPointOwner();
bool				doIntersect(POINT, POINT, POINT, POINT);
bool				existPath();
void				drawPath(HWND);
int					getindex(POINT x);
void				convertIndex(pointOwner a, vector <int> &b);
void BFSfindRoute();
void returnPath(int x,int y,vector <int> p);
float calPath();
bool flagshortest = false;
void countWeight(pointOwner a, vector <MyWeight> &w);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		readFile(hWnd);
		existFlag = existPath();
		if (!existFlag) {
			MessageBox(hWnd, L"Cannot find the path from Start to End", L"Notification!", MB_OK);
		}
		else {
			createPointOwner();
			if (flagshortest == false)
			{
				BFSfindRoute();
			}		
			drawPath(hWnd);
		}
		break;
    case WM_COMMAND:
        {
			wchar_t info[256] = {};
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_RESET:
				readFile(hWnd);
				scale = 1;
				InvalidateRect(hWnd, NULL, TRUE);
				SendMessage(hWnd, WM_PAINT, 0, 0);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_DISTANCE:
				swprintf(info, L"Path length: %.2f", calPath());
				MessageBox(hWnd, info, L"Distance!", MB_OK | MB_ICONINFORMATION);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_MOUSEWHEEL:
	{
		int tmp = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
		scale += tmp;
		if (scale == 201)
			scale = 200;
		else if (scale == 0)
			scale = 1;
		else {
			InvalidateRect(hWnd, NULL, TRUE);

			SendMessage(hWnd, WM_PAINT, 0, 0);
		}
		break;
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			draw(hWnd); // only draw the polygons
			if (existFlag)
				drawPath(hWnd); // draw the path from Start to End 
            EndPaint(hWnd, &ps);
			
        }
        break;
    case WM_DESTROY:
		deAlloc();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

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

void readFile(HWND hWnd) {
	ifstream f;
	int d, x, y;
	f.open("input.txt");
	if (!f) {
		MessageBox(hWnd, L"Could not open the file, please check the name and re-run the program!", L"Error!!!", MB_OK);
		return;
	}
	f >> n; // số poly
	p = new POINT*[n];
	c = new int[n];

	

	//readFromFile
	for (int i = 0; i < n; ++i) {
		f >> c[i];
		p[i] = new POINT[c[i]];
		for (int j = 0; j < c[i]; ++j) {
			f >> x >> y;
			p[i][j].x = x ;
			p[i][j].y = y;
		}
	}
	f >> x >> y;
	s.x = x ;
	s.y = y ;
	f >> x >> y;
	e.x = x ;
	e.y = y ;
}

void draw(HWND hWnd) {
	//draw by vector poly
	
	HDC dc = GetDC(hWnd);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < c[i]; ++j) {
			p[i][j].x /= scale;
			p[i][j].y /= scale;
		}
		Polygon(dc, p[i], c[i]);
		for (int j = 0; j < c[i]; ++j) {
			p[i][j].x *= scale;
			p[i][j].y *= scale;
		}
	}
	Ellipse(dc, (s.x - 10) / scale, (s.y + 10) / scale, (s.x + 10) / scale, (s.y - 10) / scale);
	Ellipse(dc, (e.x - 10) / scale, (e.y + 10) / scale, (e.x + 10) / scale, (e.y - 10) / scale);
	ReleaseDC(hWnd, dc);
}

void deAlloc() {
	for (int i = 0; i < n; ++i) {
		delete[]p[i];
	}
	delete[]c;
	delete[]p;
}

bool isConnect(POINT a, POINT b) {
	for (int i = 0; i < n; ++i) {
		for (int j = 1; j < c[i]; ++j) {
			if (doIntersect(a, b, p[i][j], p[i][j - 1])) // Just check the sides of each polygon
				return false;
		}
		if (doIntersect(a, b, p[i][0], p[i][c[i] - 1]))
			return false;
	}
	return true;
}
void createPointOwner() {
	pointOwner pO;
	pO._init(s);
	if (isConnect(s, e)) {	// if exist a line from Start to End, break anh return the result
		flagshortest = true;
		pO.arr[pO.n++] = e;
		pArray.push_back(pO);
		path.push_back(s);
		path.push_back(e);
		return;
	}
	// Create point for StartPoint
	for (int i = 0; i < n; i++) { 
		for (int j = 0; j < c[i]; ++j) {
			if (isConnect(s, p[i][j])) {
				pO.arr[pO.n++] = p[i][j];
			}
		}
	}
	pArray.push_back(pO);
	// Create for vertices point of polygon
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < c[i]; ++j) {
			pO._init(p[i][j]);
			if (isConnect(p[i][j], s))
				pO.arr[pO.n++] = s;
			if (isConnect(p[i][j], e))
				pO.arr[pO.n++] = e;
			for (int h = 0; h < n; ++h) {
				for (int k = 0; k < c[h]; ++k) { // chay chi toi 1
					if (i != h || j != k){
						if (i == h) {
							if (abs(j - k) == 1 || abs(j - k) == c[i]-1)
								pO.arr[pO.n++] = p[h][k];
						}
						else {
							if (isConnect(p[i][j], p[h][k])) 
								pO.arr[pO.n++] = p[h][k];
						}
					}
				}
			}
			
			pArray.push_back(pO);
		}
	}
	// create for EndPoint
	pO._init(e);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < c[i]; ++j) {
			if (isConnect(s, p[i][j])) {
				pO.arr[pO.n++] = p[i][j];
			}
		}
	}
		pArray.push_back(pO);
}

bool operator==(POINT a, POINT b) {
	return (a.x == b.x && a.y == b.y);
}

bool Is_In(double x, double a, double b)
{
	return a < b ? (x > a) && (x < b) : (x < a) && (x > b);
}

bool doIntersect(POINT p1, POINT q1, POINT p2, POINT q2)
{
	if (p1 == p2 || p1 == q1 || p1 == q2 || q1 == p2 || q1 == q2 || p2 == q2)
		return false;
	double x1 = p1.x, x2 = q1.x, x3 = p2.x, x4 = q2.x, y1 = p1.y, y2 = q1.y, y3 = p2.y, y4 = q2.y;
	if (x1>x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}
	if (x3>x4)
	{
		swap(x3, x4);
		swap(y3, y4);
	}

	double a1 = (y1 - y2) * (x3 - x4), //slope of the line contains segment 1
		a2 = (y3 - y4) * (x1 - x2),
		t = (x3 - x4)*(x1 - x2);
	//slope of the line contains segment 2
	/*!
	the line function
	y = a*x - a*xo + yo
	solve for x where two lines intersect
	!*/

	if (a1 == a2) //special cases where 2 segments are parallel or lie on each other
	{
		if (x1 == x2 && y3 == y4) {
			int x = x1, y = y3;
			if (Is_In(x, x3, x4) && Is_In(y, y1, y2))
				return true;
			return false;
		}
		else if (x3 == x4 && y1 == y2) {
			int x = x3, y = y1;
			if (Is_In(x, x1, x2) && Is_In(y, y3, y4))
				return true;
			return false;
		}
		return false;
	}
	else
	{
		double x = (a1*x1 - a2*x3 - y1*t + y3*t) / (a1 - a2);
		double y;
		if (x == x1 && x == x2) {
			y = (y4 - y3)*(x3 - x) / (x3 - x4) + y3;
			if (Is_In(y, y3, y4) && Is_In(y, y1, y2) || Is_In(x, x3, x4))
				return true;
			return false;
		}
		else if (x == x3 && x == x4)
		{
			y = (y2 - y1)*(x1 - x) / (x1 - x2) + y1;
			if (Is_In(y, y1, y2) && Is_In(y , y3, y4) || Is_In(x, x1, x2))
				return true;
			return false;
		}
		if (Is_In(x, x1, x2) && Is_In(x, x3, x4))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
pointOwner findPointOwner(POINT p) { // loop in pArray and find Point p. Not finished yet
	for (auto i : pArray) {
		if (i.a == p)
			return i;
	}
}
void BFSfindRoute() 
{
	queue<int> q;
	q.push(0);
	vector <int> visited(pArray.size(),0);
	vector <int> pathindex(pArray.size(),-1);
	visited[0] = 1;
	while (!q.empty())
	{
		int tmp = q.front(); // phan tu dang xet
		q.pop(); // xoa no khoi queue
		vector <int> convex;
		vector<MyWeight>weight;
		////convertIndex(pArray[tmp], convex); // convert sang chi so
		countWeight(pArray[tmp], weight);
		sort(weight.begin(), weight.end());
		for (int i = 0; i < pArray[tmp].n; i++) // tat ca cac canh connect
		{
			if (0 == visited[weight[i].index]) // neu dinh nay chua dc to mau 
			{
				pathindex[weight[i].index] = tmp; // luu lai dinh cha
				visited[weight[i].index] = 1; // dang to mau
				q.push(weight[i].index); // day vao queue
			}
		}
		visited[tmp] = 2; // da to xong dinh
		if (visited[tmp] == 2 && tmp==pArray.size()-1 )
		{
			returnPath(0, pArray.size() - 1, pathindex);
		}
	}
	return;
}
// DFS recur. Not finished yet
vector<POINT> findRoute(pointOwner cur, int index, vector<POINT> path, vector<POINT> visited) {
	bool flag = false;
	int _x = cur.a.x, _y = cur.a.y;
	if (_x == e.x && _y == e.y) { // end point
		path.push_back(e);
		return path;
	}
	if (cur.n != 0) { // not last point

		for (auto i : visited) { 
			if (i.x == _x && i.y == _y) {
				flag = true;
				break;
			}
		}
		if (!flag) { // point not visited
			path.push_back(cur.a);
			visited.push_back(cur.a);
			findRoute(findPointOwner(cur.arr[index]), index + 1, path, visited);
		}
	}

}
float calDis(POINT a, POINT b) {
	return sqrt(pow(b.y - a.y, 2) + pow(b.x - a.x, 2));
}
float calPath() {
	float r = 0;
	for (int i = 1; i < path.size(); ++i) {
			r += calDis(path[i], path[i - 1]);
	}
	return r;
}

void drawPath(HWND hWnd) {
	HPEN hPen = CreatePen(PS_DASHDOT, 2, RGB(54,117, 23));
	HDC hdc = GetDC(hWnd);
	SelectObject(hdc, hPen);
	for (int i = 0; i < path.size() - 1; ++i) {
		MoveToEx(hdc, path[i].x/scale, path[i].y/scale, NULL);
		LineTo(hdc, path[i + 1].x/scale, path[i + 1].y/scale);
	}
	ReleaseDC(hWnd, hdc);
}

bool existPath() {
	for (int i = 0; i < n; ++i) {
		HRGN tmp = CreatePolygonRgn(p[i], c[i], ALTERNATE);
		if (PtInRegion(tmp, s.x, s.y) || PtInRegion(tmp, e.x, e.y))
			return false;
	}
	return true;
}
int	getindex(POINT x)
{
	for (int i = 0; i < pArray.size(); i++)
	{
		if (pArray[i].a == x) return i;
	}
	return -1;
}
void convertIndex(pointOwner a,vector <int> &b)
{
	for (int i = a.n-1; i >=0; i--)
	{
		int tmp;
		tmp = getindex(a.arr[i]);
		b.push_back(tmp);
	}
}
void returnPath(int x,int y,vector<int> p)
{
	if (p[y] == x)
	{
		path.push_back(pArray[x].a);
		path.push_back(pArray[y].a);
	}
	else
	{
		returnPath(x, p[y], p);
		path.push_back(pArray[y].a);
	}
}
void countWeight(pointOwner a,vector <MyWeight> &w)
{
	for (int i = a.n - 1; i >= 0; i--)
	{
		float tmp;
		MyWeight tmpw;
		tmp = calDis(a.a, a.arr[i]);
		tmpw.index = getindex(a.arr[i]);
		tmpw.weight = tmp;
		w.push_back(tmpw);
	}
}