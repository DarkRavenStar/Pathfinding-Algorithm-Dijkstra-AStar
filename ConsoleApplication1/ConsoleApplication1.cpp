
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "AllHeader.h"

#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl2.h"
#include "windows.h"
#include "Pathfinding.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

static GLFWwindow* window;
struct nk_context *ctx;

int fps = 0;
int fpsLimit = 60;
bool play = false;
bool reseted = true;
bool reset = false;
int weight = 1;
bool allowDiagonal = false;

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h, c);
}

void UpdateText()
{
	gotoxy(0, 0);
	std::cout << "Pathfinding 0118856 Teena" << std::endl;
	gotoxy(0, 1);
	std::cout << "Use the GUI or Keyboard" << std::endl;
	gotoxy(0, 2);
	std::cout << "Use P to play or pause" << std::endl;
	gotoxy(0, 3);
	std::cout << "Use R to reset" << std::endl;
	gotoxy(0, 4);
	std::cout << "Use D to toggle diagonal on / off" << std::endl;
	gotoxy(0, 5);
	std::cout << "Use W/S or UP/DOWN to increase/decrease Weight" << std::endl;
	gotoxy(0, 6);
	std::string diagonal = (allowDiagonal == true) ? "TRUE  " : "FALSE  ";
	std::cout << "Allow Diagonal: " << diagonal << std::endl;
	gotoxy(0, 7);
	std::cout << "Current Weight: " << weight << "       " << std::endl;
}

void UpdateDiagonal()
{
	gotoxy(0, 6);
	std::string diagonal = (allowDiagonal == true) ? "TRUE  " : "FALSE  ";
	std::cout << "Allow Diagonal: " << diagonal << std::endl;
}

void UpdateWeight()
{
	gotoxy(0, 7);
	std::cout << "Current Weight: " << weight << "       " << std::endl;
}


void DrawGUI()
{
	nk_glfw3_new_frame();
	static size_t prevWeight = 1;
	static int diagonalCheck = 1;
	static int revertToDij = 1;

	/* GUI */
	if (nk_begin(ctx, "Dijkstra and A* Star", nk_rect(0, 0, WINDOW_WIDTH * 0.25, WINDOW_HEIGHT),
		NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
	{
		nk_layout_row_dynamic(ctx, 30, 1);
		if (nk_button_label(ctx, "PLAY/PAUSE"))
		{
			play = !play;
		}

		nk_layout_row_dynamic(ctx, 30, 1);
		if (nk_button_label(ctx, "RESET"))
		{
			reset = true;
		}

		nk_checkbox_label(ctx, "Allow Diagonal", &diagonalCheck);
		if (diagonalCheck == 1)
		{
			allowDiagonal = false;
			UpdateDiagonal();
		}
		else
		{
			allowDiagonal = true;
			UpdateDiagonal();
		}

		nk_checkbox_label(ctx, "Revert to Dijkstra", &revertToDij);
		if (revertToDij == 0)
		{
			if (play == false && reseted == true && reset == false)
			{
				prevWeight = 0;
				UpdateWeight();
			}
			else
			{
				revertToDij = 1;
			}
		}

		nk_labelf(ctx, NK_TEXT_LEFT, "Weight: %zu", prevWeight);
		nk_progress(ctx, &prevWeight, 100, NK_MODIFIABLE);

		if (prevWeight != weight)
		{
			if (play == false && reseted == true && reset == false)
			{
				weight = prevWeight;
				UpdateWeight();
			}
			else
			{
				prevWeight = weight;
			}
		}
	}
	nk_end(ctx);

	nk_glfw3_render(NK_ANTI_ALIASING_ON);
}

void onWindowResized(GLFWwindow* window, int width, int height)
{
	if (height == 0) height = 1;						// Prevent A Divide By Zero By making Height Equal One

	WINDOW_HEIGHT = height;
	WINDOW_WIDTH = width;

	glViewport(0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT);	// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0, -1, 1);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void PathFindingInput(GLFWwindow* window, int width, int height)
{
	static double xpos, ypos;
	static bool foundPath = false;
	glfwGetCursorPos(window, &xpos, &ypos);
	int leftMousebutton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);

	int xMin = width - height;
	int xMax = width;

	int yMin = 0;
	int yMax = height;
	int xOffset = (xMax - xMin) / cols;
	int yOffset = (yMax - yMin) / rows;

	int intX = (xpos - xMin) / xOffset;
	int intY = (ypos - yMin) / yOffset;

	if (play == false && reseted == true)
	{
		if (intX >= 0 && intX < cols && intY >= 0 && intY < rows)
		{
			SelectPathGrid(intX, intY, leftMousebutton);
		}
	}
}

void PathfindingExecution()
{
	static bool foundPath = false;
	static bool completed = false;

	if (play == true)
	{
		if (reseted == true)
		{
			reseted = false;
			foundPath = false;
			completed = false;
		}
		if (completed == false)
		{
			if (fps == 60 / fpsLimit)
			{
				if (foundPath == false)
				{
					foundPath = CalculatePathGrid(allowDiagonal, weight, HeuristicsMode::Octile);
				}

				if (foundPath == true)
				{
					if (endingNode->parentNode != NULL)
					{
						PathRender(endingNode->parentNode);
					}
					foundPath = false;
					completed = true;
				}
				fps = 0;
			}
			fps++;
		}
	}

	if (reset == true)
	{
		ResetGrid();
		play = false;
		reseted = true;
		reset = false;
	}
}

int main()
{
	system("mode con: cols=80 lines=30");
	/* Initialize the GLFW library */
	if (!glfwInit())
		return -1;

	// Open an OpenGL window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Hook window resize.
	glfwSetWindowSizeCallback(window, onWindowResized);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	onWindowResized(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSwapInterval(1);

	glClearColor(0.2f, 0.2f, 0.2f, 0.5f);
	UpdateText();
	SetUpPathGrid();
	ResetGrid();

	/* GUI */
	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	{struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&atlas);
	nk_glfw3_font_stash_end(); }

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// OpenGL rendering goes here...
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			break;

		PathFindingInput(window, WINDOW_WIDTH, WINDOW_HEIGHT);
		DrawPathGrid(WINDOW_WIDTH, WINDOW_HEIGHT);
		PathfindingExecution();
		DrawGUI();

		// Swap front and back rendering buffers
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	// Close window and terminate GLFW
	nk_glfw3_shutdown();
	glfwTerminate();
}