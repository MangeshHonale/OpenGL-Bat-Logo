#include <windows.h>
#include <gl/GL.h>
#include<math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")


//Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
int giX, giY;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;

	//code
	//initializing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	//Registering Class
	RegisterClassEx(&wndclass);

	giX = GetSystemMetrics(SM_CXFULLSCREEN) / 2 - WIN_WIDTH / 2;
	giY = GetSystemMetrics(SM_CYFULLSCREEN) / 2 - WIN_HEIGHT / 2;

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("WHY SO SERIOUS!"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		giX,
		giY,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);


	ghwnd = hwnd;

	//initialize
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Message Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				display();
			}

		}
	}

	uninitialize();
	return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype

	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;


	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	void initialize(void);
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	//function prototypes
	void resize(int, int);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{

	void drawBatLogo();

	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawBatLogo();

	SwapBuffers(ghdc);
}

void drawBatLogo() {
	const float PI = 3.1415926535898f;


	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslated(-1.0f, -1.0f, 0.0f);

	//bottom left curve
	for (float angle = 0.0f; angle <= PI / 2; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle), sin(angle), 0.0f);
	}
	glEnd();

	//bottom right curve
	glTranslated(2.0f, 0.0f, 0.0f);
	for (float angle = PI / 2; angle <= PI; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle), sin(angle), 0.0f);
	}
	glEnd();

	//left side curve
	glTranslated(-2.0f, 0.78f, 0.0f);
	for (float angle = 0.0f; angle <= PI / 2; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 0.2f, sin(angle), 0.0f);
	}
	glEnd();

	//top rectangle
	glTranslated(0.0f, 1.22f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, -0.22f, 0.0f);
	glVertex3f(0.0f, -0.22f, 0.0f);
	glEnd();

	//top left curve
	glTranslated(0.82f, -0.22f, 0.0f);
	for (float angle = PI; angle <= 3 * PI / 2; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 0.4f, sin(angle) * 0.4f, 0.0f);
	}
	glEnd();

	//bat horns
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.35f, 0.0f, 0.0f);
	glVertex3f(0.35f, -0.07f, 0.0f);
	glVertex3f(0.0f, -0.07f, 0.0f);
	glVertex3f(0.05f, -0.3f, 0.0f);
	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.35f, -0.07f, 0.0f);
	glEnd();

	//top right curve
	glTranslated(0.35f, 0.0f, 0.0f);
	for (float angle = 3 * PI / 2; angle <= 2 * PI; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 0.4f, sin(angle) * 0.4f, 0.0f);
	}
	glEnd();

	//right side curve
	glTranslated(0.83f, -1.0f, 0.0f);
	for (float angle = PI / 2; angle <= PI; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 0.2f, sin(angle), 0.0f);
	}
	glEnd();

	//bat head curve
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslated(-1.0f, 0.0f, 0.0f);
	for (float angle = 0.43f * PI; angle <= 0.57f * PI; angle = angle + 0.0001f) {
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 0.72f, sin(angle) * 0.72f, 0.0f);
	}
	glEnd();

	//word box
	//glColor3f(0.0f, 0.0f, 0.0f);
	glTranslated(0.0f, 0.22f, 0.0f);



	//Letter B
	glColor3f(0.411f, 0.733f, 0.0039f);
	glBegin(GL_QUADS);
	glVertex3f(-0.5f, 0.125f, 0.0f);
	glVertex3f(-0.5f, -0.125f, 0.0f);
	glVertex3f(-0.33333333f, -0.125f, 0.0f);
	glVertex3f(-0.33333333f, 0.125f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(-0.44444f, 0.0625f, 0.0f);
	glVertex3f(-0.44444f, -0.0625f, 0.0f);
	glVertex3f(-0.38888f, -0.06256f, 0.0f);
	glVertex3f(-0.38888f, 0.0625f, 0.0f);
	glEnd();

	glColor3f(0.411f, 0.733f, 0.0039f);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glVertex3f(-0.44444f, 0.0f, 0.0f);
	glVertex3f(-0.38888f, 0.0f, 0.0f);
	glEnd();

	//letter A
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.25f, 0.125f, 0.0f);
	glVertex3f(-0.333f, -0.125f, 0.0f);
	glVertex3f(-0.166667f, -0.125f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glVertex3f(-0.2777444f, -0.125f, 0.0f);
	glVertex3f(-0.2777444f, -0.055f, 0.0f);
	glVertex3f(-0.2218888f, -0.055f, 0.0f);
	glVertex3f(-0.2218888f, -0.125f, 0.0f);
	glEnd();

	//Letter T
	glColor3f(0.411f, 0.733f, 0.0039f);
	glBegin(GL_QUADS);
	glVertex3f(-0.16666f, 0.125f, 0.0f);
	glVertex3f(-0.16666f, 0.06255f, 0.0f);
	glVertex3f(0.0f, 0.0625, 0.0f);
	glVertex3f(0.0f, 0.125f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(-0.11111f, 0.125f, 0.0f);
	glVertex3f(-0.11111f, -0.125f, 0.0f);
	glVertex3f(-0.0555f, 0.125f, 0.0f);
	glVertex3f(-0.0555f, -0.125f, 0.0f);
	glEnd();

	//letter M
	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 0.125f, 0.0f);
	glVertex3f(0.0f, -0.125f, 0.0f);
	glVertex3f(0.1666f, -0.125f, 0.0f);
	glVertex3f(0.1666f, 0.125f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.08333f, -0.125f, 0.0f);
	glVertex3f(0.125f, 0.0625f, 0.0f);
	glVertex3f(0.125f, -0.125f, 0.0f);
	glEnd();


	glBegin(GL_TRIANGLES);
	glVertex3f(0.04166f, -0.125f, 0.0f);
	glVertex3f(0.04166f, 0.0625f, 0.0f);
	glVertex3f(0.08333f, -0.125f, 0.0f);
	glEnd();

	//letter A
	glColor3f(0.411f, 0.733f, 0.0039f);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.1666f, -0.125f, 0.0f);
	glVertex3f(0.25f, 0.125f, 0.0f);
	glVertex3f(0.333f, -0.125f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(0.22222f, -0.055f, 0.0f);
	glVertex3f(0.2222f, -0.125f, 0.0f);
	glVertex3f(0.2777f, -0.125f, 0.0f);
	glVertex3f(0.2777f, -0.055f, 0.0f);
	glEnd();

	//letter N
	glColor3f(0.411f, 0.733f, 0.0039f);
	glBegin(GL_QUADS);
	glVertex3f(0.5f, 0.125f, 0.0f);
	glVertex3f(0.5f, -0.125f, 0.0f);
	glVertex3f(0.33333333f, -0.125f, 0.0f);
	glVertex3f(0.33333333f, 0.125f, 0.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.3889f, 0.0625f, 0.0f);
	glVertex3f(0.3889f, -0.125f, 0.0f);
	glVertex3f(0.4444f, -0.125f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(0.3889f, 0.125f, 0.0f);
	glVertex3f(0.4444f, 0.125f, 0.0f);
	glVertex3f(0.4444f, -0.0625f, 0.0f);
	glEnd();

	//line to seprate T and M
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.125f, 0.0f);
	glVertex3f(0.0f, -0.125f, 0.0f);
	glEnd();
}


void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
}
