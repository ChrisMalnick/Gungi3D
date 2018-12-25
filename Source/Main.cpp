/* ============================================================================
 * Project:	Gungi3D
 *
 * File:	Main.cpp
 *
 * Summary:	Initializes, updates, and renders all logical and graphical
 *			components and handles all user input
 *
 * Origin:	N/A
 *
 * Legal:	Unregistered Copyright (C) 2018 Chris Malnick - All Rights Reserved
 *			Unauthorized duplication, reproduction, modification, and/or
 *			distribution is strictly prohibited
 *			All materials, including, but not limited to, code, resources
 *			(models, textures, etc.), documents, etc. are deliberately
 *			unlicensed
 * ============================================================================
 */

#include "Renderer/Backbuffer.h"
#include "Renderer/Depthmap.h"
#include "Renderer/Font.h"
#include "World/Camera.h"
#include "World/Scene.h"

#include <random>
#include <thread>

// For Windows 32-bit & 64-bit
#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "Windows/WindowsUI.h"
#include "Windows/LimitSingleInstance.h"
#include "Windows/RedirectStandardIO.h"

CLimitSingleInstance g_instance(TEXT("Global\\{06F0C117-9399-4705-98A4-02CC3B134895}"));
#endif

// Function prototypes
// -------------------
extern void drawLoadScreen(const std::string &p_path_ref, void *p_window_ptr = nullptr);
extern int rand(int p_min, int p_max);

void initDisplay();

void loadFont(Font &p_font_ref, const std::string &p_path_ref, FT_UInt p_size, FT_Fixed p_thick);

void outputSystemInfo();

void genDepthmapCube();
void genDepthmap2D();

void update(GLFWwindow *p_window_ptr);
void render();

void renderHUD();
void renderDebug();

void renderText(Font &p_font_ref, const std::string &p_text_ref, glm::vec2 p_pos, glm::vec4 p_face_color, glm::vec4 p_stroke_color = glm::vec4(0.0f));

void pollKeys(GLFWwindow *p_window_ptr);

void calcMouseRay(double p_xpos, double p_ypos);

void switchPolygonMode();
void switchCameraMode(GLFWwindow *p_window_ptr);

void toggleFullscreen(GLFWwindow *p_window_ptr);

std::string getHUDMessage();

void newDialog(GLFWwindow *p_window_ptr);
void exitDialog(GLFWwindow *p_window_ptr);

void openDialog(GLFWwindow *p_window_ptr);
void saveDialog(GLFWwindow *p_window_ptr);

// Callbacks
void window_pos_callback(GLFWwindow *p_window_ptr, int p_xpos, int p_ypos);
void window_refresh_callback(GLFWwindow *p_window_ptr);

void window_iconify_callback(GLFWwindow *p_window_ptr, int p_iconified);
void window_close_callback(GLFWwindow *p_window_ptr);

void framebuffer_size_callback(GLFWwindow *p_window_ptr, int p_width, int p_height);

void key_callback(GLFWwindow *p_window_ptr, int p_key, int p_scancode, int p_action, int p_mods);

void mouse_button_callback(GLFWwindow *p_window_ptr, int p_button, int p_action, int p_mods);

void cursor_pos_callback(GLFWwindow *p_window_ptr, double p_xpos, double p_ypos);

void scroll_callback(GLFWwindow *p_window_ptr, double p_xoffset, double p_yoffset);

// Function pointers
int* (*g_new_callback_ptr)(GLFWwindow*);
bool (*g_exit_callback_ptr)(GLFWwindow*);

std::string (*g_open_callback_ptr)(GLFWwindow*);
std::string (*g_save_callback_ptr)(GLFWwindow*);

// Instance properties
// -------------------
unsigned int g_seed;

bool g_debug_mode;
bool g_ready;

// Window properties
// -----------------
int g_window_width = 640;
int g_window_height = 480;

int g_window_refresh_rate = 60;

int g_window_xpos;
int g_window_ypos;

bool g_window_fullscreen;
bool g_window_minimized;

// Display properties
// ------------------
int g_display_width;
int g_display_height;

int g_display_refresh_rate;

// Thread properties
// -----------------
std::thread g_thread;

double g_prev_time;
double g_delta_time;

int g_frame_count;
int g_frame_rate;

// Cursor properties
// -----------------
double g_xprev;
double g_yprev;

bool g_in_window;
bool g_new_window = true; // For updating cursor position relative to new window

// Viewport properties
// -------------------
Camera g_camera;

// Projections
glm::mat4 g_perspective;
glm::mat4 g_orthographic;

glm::mat4 g_view;

// Renderer properties
// -------------------
Scene g_scene;
Backbuffer g_backbuffer;

GLint g_polygon_mode = GL_FILL;

bool g_render_shadows = true;

// Shaders
Shader g_depthmap_cubemap_shader;
Shader g_depthmap_2D_shader;
Shader g_overlay_shader;
Shader g_model_shader;
Shader g_postproc_shader;
Shader g_text_shader;

// Maps
Depthmap g_depthmap;

// Fonts
Font g_debug_font;
Font g_HUD_font;

Font g_logo_outer_font;
Font g_logo_inner_font;

Font g_info_font;

bool g_show_debug;
bool g_show_HUD = true;

// Inline functions
// ----------------
inline int containerWidth() { return (g_window_fullscreen ? g_display_width : g_window_width); }
inline int containerHeight() { return (g_window_fullscreen ? g_display_height : g_window_height); }

inline int containerRefreshRate() { return (g_window_fullscreen ? g_display_refresh_rate : g_window_refresh_rate); }

inline int containerX() { return (g_window_fullscreen ? 0 : g_window_xpos); }
inline int containerY() { return (g_window_fullscreen ? 0 : g_window_ypos); }

int main()
{
	// For Windows 32-bit & 64-bit
#ifdef _WIN32
	if (g_instance.Running())
		return -1;

	g_seed = Seed();
	g_debug_mode = Debug();

	RedirectOutput();
#endif

	// Initialize RNG
	std::srand(g_seed);

	// GLFW: Initialize and configure
	// ------------------------------
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// For Mac OS X
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Disallow use of deprecated OpenGL features
#endif

	// GLFW: Window creation
	// ---------------------
	GLFWwindow *window_ptr = glfwCreateWindow(g_window_width, g_window_height, "Gungi3D", NULL, NULL);

	initDisplay();

	glfwSetWindowMonitor(window_ptr, NULL, g_window_xpos, g_window_ypos, g_window_width, g_window_height, g_window_refresh_rate);

	if (window_ptr == nullptr)
	{
		std::cerr << "ERROR::MAIN::GLFW::WINDOW::CREATE_FAILED" << std::endl;

		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window_ptr);

	// GLFW: Set callbacks and input modes
	// -----------------------------------
	// Window position, refresh, minimize, and close
	glfwSetWindowPosCallback(window_ptr, window_pos_callback);
	glfwSetWindowRefreshCallback(window_ptr, window_refresh_callback);

	glfwSetWindowIconifyCallback(window_ptr, window_iconify_callback);
	glfwSetWindowCloseCallback(window_ptr, window_close_callback);

	// Framebuffer resize
	glfwSetFramebufferSizeCallback(window_ptr, framebuffer_size_callback);

	// Key input and mode
	glfwSetKeyCallback(window_ptr, key_callback);
	glfwSetInputMode(window_ptr, GLFW_STICKY_KEYS, 1);

	// Mouse button input and mode
	glfwSetMouseButtonCallback(window_ptr, mouse_button_callback);
	glfwSetInputMode(window_ptr, GLFW_STICKY_MOUSE_BUTTONS, 1);

	// Cursor position
	glfwSetCursorPosCallback(window_ptr, cursor_pos_callback);

	// Scroll input
	glfwSetScrollCallback(window_ptr, scroll_callback);

	// Platform dependent dialogs
	// --------------------------
	// For Windows 32-bit & 64-bit
#if defined _WIN32
	g_new_callback_ptr = &windows_new_callback;
	g_exit_callback_ptr = &windows_exit_callback;
	
	g_open_callback_ptr = &windows_open_callback;
	g_save_callback_ptr = &windows_save_callback;
#endif

	// GLAD: Load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR::MAIN::GLAD::LOAD_FAILED" << std::endl;

		glfwTerminate();
		return -3;
	}

	outputSystemInfo();

	// Configure global OpenGL state
	// -----------------------------
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize camera
	// -----------------
	g_camera.init(Camera::FIXED);

	// Build multisampled and singlesampled color buffers
	// --------------------------------------------------
	g_backbuffer.build(g_window_width, g_window_height);

	// Build and compile shader programs
	// ---------------------------------
	g_depthmap_cubemap_shader.build("./Resources/Shaders/Depthmap/Cubemap/Depthmap_Cubemap");
	g_depthmap_2D_shader.build("./Resources/Shaders/Depthmap/2D/Depthmap_2D");
	g_overlay_shader.build("./Resources/Shaders/Overlay/Overlay");
	g_model_shader.build("./Resources/Shaders/Model/Model");
	g_postproc_shader.build("./Resources/Shaders/Postproc/Postproc");
	g_text_shader.build("./Resources/Shaders/Text/Text");

	// Build depth buffers for shadow mapping
	// --------------------------------------
	g_depthmap.build();

	// FreeType: Initialize library and build fonts
	// --------------------------------------------
	// For Windows 32-bit & 64-bit
#ifdef _WIN32
	loadFont(g_debug_font, KnownFolderPath(FOLDERID_Fonts) + "/courbd.ttf", 16, 2);
	loadFont(g_HUD_font, KnownFolderPath(FOLDERID_Fonts) + "/ariblk.ttf", 24, 2);

	loadFont(g_logo_outer_font, KnownFolderPath(FOLDERID_Fonts) + "/ariblk.ttf", 128, 4);
	loadFont(g_logo_inner_font, KnownFolderPath(FOLDERID_Fonts) + "/ariblk.ttf", 128, 2);

	loadFont(g_info_font, KnownFolderPath(FOLDERID_Fonts) + "/ariblk.ttf", 12, 2);
#endif

	// Build scene models and lights
	// -----------------------------
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	drawLoadScreen("", window_ptr);

	g_scene.build();
	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

	// Render static models into cubemapped depth buffer
	// -------------------------------------------------
	genDepthmapCube();

	g_ready = true;

	// Game loop
	// ---------
	while (!glfwWindowShouldClose(window_ptr))
	{
		// Thread management
		// -----------------
		double curr_time = glfwGetTime();

		g_delta_time = curr_time - g_prev_time;
		g_prev_time = curr_time;

		if (g_frame_count % 20 == 0)
			g_frame_rate = static_cast<int>(1.0 / g_delta_time);

		// Execution
		// ---------
		if (!g_window_minimized)
		{
			update(window_ptr);
			render();
		}

		// GLFW: Swap framebuffers and process events
		// ------------------------------------------
		glfwSwapBuffers(window_ptr);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

extern void drawLoadScreen(const std::string &p_path_ref, void *p_window_ptr)
{
	static GLFWwindow *window_ptr = static_cast<GLFWwindow*>(p_window_ptr);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	g_text_shader.bind();
	g_text_shader.setMat4("u_projection", g_orthographic);
	
	renderText(g_logo_outer_font, "Gungi3D", glm::vec2(containerWidth() / 2 - 295, containerHeight() / 2 - 33), glm::vec4(0.0f), glm::vec4(1.0f));
	renderText(g_logo_inner_font, "Gungi3D", glm::vec2(containerWidth() / 2 - 295, containerHeight() / 2 - 33), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.5f, 1.0f));

	renderText(g_info_font, "v1.0", glm::vec2(containerWidth() / 2 + 262, containerHeight() / 2 - 50), glm::vec4(1.0f));
	renderText(g_info_font, "Loading: " + p_path_ref.substr(p_path_ref.find_last_of("/\\") + 1), glm::vec2(5.0f), glm::vec4(1.0f));
	renderText(g_info_font, "2018 Chris Malnick", glm::vec2(containerWidth() - 130, 5.0f), glm::vec4(1.0f));

	glfwSwapBuffers(window_ptr);

	if (!p_path_ref.empty())
		glfwPollEvents();
}

extern int rand(int p_min, int p_max)
{
	static unsigned int offset;
	static thread_local std::default_random_engine RNG(g_seed + offset++);
	std::uniform_int_distribution<int> dist(p_min, p_max);

	return dist(RNG);
}

void initDisplay()
{
	const GLFWvidmode *mode_ptr = glfwGetVideoMode(glfwGetPrimaryMonitor());

	g_display_width = mode_ptr->width;
	g_display_height = mode_ptr->height;

	g_display_refresh_rate = mode_ptr->refreshRate;

	g_window_xpos = g_display_width / 2 - g_window_width / 2;
	g_window_ypos = g_display_height / 2 - g_window_height / 2;
}

void loadFont(Font &p_font_ref, const std::string &p_path_ref, FT_UInt p_size, FT_Fixed p_thick)
{
	p_font_ref.init(p_path_ref, 0);
	p_font_ref.build();

	p_font_ref.setSize(0, p_size);
	p_font_ref.setStroker(p_thick, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

	p_font_ref.loadGlyphs(128);
	p_font_ref.release();
}

void outputSystemInfo()
{
	if (!g_debug_mode)
		return;

	std::cout << "===============================================================================" << std::endl;
	std::cout << "SYSTEM INFO" << std::endl;
	std::cout << "===============================================================================" << std::endl;
	std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	GLint target;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &target);
	std::cout << "Maximum number of vertex attributes supported: " << target << std::endl;

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &target);
	std::cout << "Maximum number of texture units supported: " << target << std::endl;
	
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &target);
	std::cout << "Maximum number of color samples supported: " << target << std::endl;

	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &target);
	std::cout << "Maximum number of depth samples supported: " << target << std::endl;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &target);
	std::cout << "Maximum texture size supported: " << target << std::endl;

	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &target);
	std::cout << "Maximum cubemap size supported: " << target << std::endl;
	std::cout << "===============================================================================" << std::endl;
}

void genDepthmapCube()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	g_depthmap.bindCubemap();

	glClear(GL_DEPTH_BUFFER_BIT);

	g_depthmap_cubemap_shader.bind();

	g_depthmap.setUniformsCubemap(g_depthmap_cubemap_shader);

	g_scene.renderModels(g_depthmap_cubemap_shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void genDepthmap2D()
{
	g_depthmap.bind2D();

	glClear(GL_DEPTH_BUFFER_BIT);

	g_depthmap_2D_shader.bind();

	g_depthmap.setUniforms2D(g_depthmap_2D_shader);

	g_scene.renderPieces(g_depthmap_2D_shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update(GLFWwindow *p_window_ptr)
{
	// Screen fade-in
	// --------------
	g_backbuffer.fade(g_delta_time);
	
	// For smooth continuous hardware independent key input
	// ----------------------------------------------------
	pollKeys(p_window_ptr);
	
	// For camera updates independent of user input
	// --------------------------------------------
	if (g_camera.getRotating())
	{
		g_camera.autoRotate(g_delta_time);
		calcMouseRay(g_xprev, g_yprev);
	}

	// Transformations
	// ---------------
	g_perspective = glm::perspective(glm::radians(g_camera.getZoom()), static_cast<float>(containerWidth()) / static_cast<float>(containerHeight()), 0.1f, 100.0f);
	g_view = g_camera.getView();

	// Create thread and evaluate AI move
	// ----------------------------------
	State *state_ptr = g_scene.getStatePtr();
	Player *player_ptr = state_ptr->getActivePlayerPtr();

	if (!state_ptr->gameOver() && !player_ptr->controllable() && !player_ptr->evaluating())
		g_thread = std::thread(&Player::eval, player_ptr);

	// Update scene and handle AI move
	// -------------------------------
	if (g_scene.update(g_delta_time) && state_ptr->handleAI())
	{
		if (g_thread.joinable())
			g_thread.join();

		calcMouseRay(g_xprev, g_yprev);
	}
}

void render()
{
	// Render dynamic models into 2D depth buffer
	// ------------------------------------------
	if (g_render_shadows)
		genDepthmap2D();

	// Render scene into multisampled floating point color buffer
	// ----------------------------------------------------------
	g_backbuffer.bind(containerWidth(), containerHeight());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, g_polygon_mode);

	glDisable(GL_CULL_FACE);

	g_overlay_shader.bind();

	g_overlay_shader.setMat4("u_projection", g_perspective);
	g_overlay_shader.setMat4("u_view", g_view);

	g_scene.renderSquares(g_overlay_shader);

	glEnable(GL_CULL_FACE);

	g_model_shader.bind();

	g_model_shader.setMat4("u_projection", g_perspective);
	g_model_shader.setMat4("u_view", g_view);

	g_model_shader.setVec3("u_view_pos", g_camera.getPosition());
	g_model_shader.setBool("u_occlude", g_render_shadows);

	g_depthmap.bind(g_model_shader);

	g_scene.getLightPtr()->setUniforms(g_model_shader);

	g_scene.renderModels(g_model_shader);
	g_scene.renderPieces(g_model_shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Perform post-processing
	// -----------------------
	g_backbuffer.blit(containerWidth(), containerHeight());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, containerWidth(), containerHeight());

	g_postproc_shader.bind();

	g_backbuffer.render(g_postproc_shader);

	// Render text independently of scene
	// ----------------------------------
	g_text_shader.bind();

	g_text_shader.setMat4("u_projection", g_orthographic);

	renderHUD();
	renderDebug();

	glEnable(GL_DEPTH_TEST);

	// Log frame
	// ---------
	++g_frame_count;
}

void renderHUD()
{
	if (!g_show_HUD)
		return;

	renderText(g_HUD_font, "Turn " + std::to_string(g_scene.getStatePtr()->getTurn()) + getHUDMessage(), glm::vec2(5.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void renderDebug()
{
	if (!g_show_debug)
		return;

	renderText(g_debug_font, std::to_string(g_frame_rate) + " fps", glm::vec2(5.0f, containerHeight() - 15.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, std::to_string(g_frame_count), glm::vec2(5.0f, containerHeight() - 30.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	renderText(g_debug_font, std::to_string(containerWidth()) + "x" + std::to_string(containerHeight()) + "(" + std::to_string(containerRefreshRate()) + ")", glm::vec2(5.0f, containerHeight() - 60.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, std::to_string(containerX()) + ", " + std::to_string(containerY()), glm::vec2(5.0f, containerHeight() - 75.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	renderText(g_debug_font, std::to_string(g_camera.getPosition().x) + ", " + std::to_string(g_camera.getPosition().y) + ", " + std::to_string(g_camera.getPosition().z), glm::vec2(5.0f, containerHeight() - 105.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, std::to_string(g_camera.getRotation().x) + ", " + std::to_string(g_camera.getRotation().y), glm::vec2(5.0f, containerHeight() - 120.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, std::to_string(g_camera.getZoom()), glm::vec2(5.0f, containerHeight() - 135.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	renderText(g_debug_font, std::to_string(g_scene.getLightPtr()->getBrightness()), glm::vec2(5.0f, containerHeight() - 165.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	renderText(g_debug_font, std::to_string(g_seed).insert(0, 10 - std::to_string(g_seed).size(), '0'), glm::vec2(containerWidth() - 105.0f, containerHeight() - 15.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	switch (g_polygon_mode)
	{
	case GL_POINT:
		renderText(g_debug_font, "Polygon Mode: POINT", glm::vec2(containerWidth() - 195, containerHeight() - 45.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		break;

	case GL_LINE:
		renderText(g_debug_font, "Polygon Mode: LINE", glm::vec2(containerWidth() - 185, containerHeight() - 45.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		break;

	case GL_FILL:
		renderText(g_debug_font, "Polygon Mode: FILL", glm::vec2(containerWidth() - 185, containerHeight() - 45.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	}

	switch (g_camera.getMode())
	{
	case Camera::FREE:
		renderText(g_debug_font, "Camera Mode: FREE", glm::vec2(containerWidth() - 175, containerHeight() - 60.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		break;

	case Camera::FIXED:
		renderText(g_debug_font, "Camera Mode: FIXED", glm::vec2(containerWidth() - 185, containerHeight() - 60.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		break;
	}

	bool animate = g_scene.getStatePtr()->getBoardPtr()->getAnimateRef();

	renderText(g_debug_font, (animate ? "Animations: ON" : "Animations: OFF"), glm::vec2(containerWidth() - (animate ? 145.0f : 155.0f), containerHeight() - 90.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, (g_render_shadows ? "Shadows: ON" : "Shadows: OFF"), glm::vec2(containerWidth() - (g_render_shadows ? 115.0f : 125.0f), containerHeight() - 105.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	int black_level = g_scene.getStatePtr()->getBlackPlayerPtr()->getLevel();
	int white_level = g_scene.getStatePtr()->getWhitePlayerPtr()->getLevel();

	renderText(g_debug_font, (black_level == HUMAN ? "Black: HMN" : "Black: CPU" + std::to_string(black_level)), glm::vec2(containerWidth() - (black_level == HUMAN ? 105.0f : 115.0f), containerHeight() - 135.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	renderText(g_debug_font, (white_level == HUMAN ? "White: HMN" : "White: CPU" + std::to_string(white_level)), glm::vec2(containerWidth() - (white_level == HUMAN ? 105.0f : 115.0f), containerHeight() - 150.0f), glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

void renderText(Font &p_font_ref, const std::string &p_text_ref, glm::vec2 p_pos, glm::vec4 p_face_color, glm::vec4 p_stroke_color)
{
	if (p_stroke_color.a != 0.0f)
	{
		g_text_shader.setVec4("u_text.color", p_stroke_color);
		p_font_ref.render(p_text_ref, p_pos, 1.0f, true);
	}

	if (p_face_color.a != 0.0f)
	{
		g_text_shader.setVec4("u_text.color", p_face_color);
		p_font_ref.render(p_text_ref, p_pos, 1.0f, false);
	}
}

void pollKeys(GLFWwindow *p_window_ptr)
{
	if (glfwGetKey(p_window_ptr, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		g_scene.getLightPtr()->dim(-g_delta_time);

	if (glfwGetKey(p_window_ptr, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		g_scene.getLightPtr()->dim(g_delta_time);

	if (g_camera.getMode() == Camera::FREE)
	{
		if (glfwGetKey(p_window_ptr, GLFW_KEY_W) == GLFW_PRESS)
			g_camera.move(Camera::FORWARD, g_delta_time);

		if (glfwGetKey(p_window_ptr, GLFW_KEY_S) == GLFW_PRESS)
			g_camera.move(Camera::BACKWARD, g_delta_time);

		if (glfwGetKey(p_window_ptr, GLFW_KEY_A) == GLFW_PRESS)
			g_camera.move(Camera::LEFT, g_delta_time);

		if (glfwGetKey(p_window_ptr, GLFW_KEY_D) == GLFW_PRESS)
			g_camera.move(Camera::RIGHT, g_delta_time);
	}
}

// Mouse coordinates are updated in 2D viewport space
// Perform inverse transformations to determine where the mouse is pointing in 3D world space
// Viewport space -> normalized device space -> homogenous clip space -> eye space -> world space
void calcMouseRay(double p_xpos, double p_ypos)
{
	if (!g_in_window || g_camera.getRotatable())
		return;

	float x = static_cast<float>(p_xpos) * 2.0f / containerWidth() - 1.0f;
	float y = 1.0f - static_cast<float>(p_ypos) * 2.0f / containerHeight();

	glm::vec4 eye_coords = glm::inverse(g_perspective) * glm::vec4(x, y, -1.0f, 1.0f);
	glm::vec4 world_coords = glm::inverse(g_view) * glm::vec4(eye_coords.x, eye_coords.y, -1.0f, 0.0f);

	glm::vec3 direction = glm::normalize(glm::vec3(world_coords.x, world_coords.y, world_coords.z));

	g_scene.mousePick(g_camera.getPosition(), direction);
}

void switchPolygonMode()
{
	if (!g_debug_mode)
		return;

	switch (g_polygon_mode)
	{
	case GL_POINT:
		g_polygon_mode = GL_LINE;
		break;

	case GL_LINE:
		g_polygon_mode = GL_FILL;
		break;

	case GL_FILL:
		g_polygon_mode = GL_POINT;
		break;
	}
}

void switchCameraMode(GLFWwindow *p_window_ptr)
{
	if (!g_debug_mode)
		return;

	switch (g_camera.getMode())
	{
	case Camera::FIXED:
		g_camera.init(Camera::FREE);
		glfwSetInputMode(p_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;

	case Camera::FREE:
		g_camera.init(Camera::FIXED);
		glfwSetInputMode(p_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	}
}

void toggleFullscreen(GLFWwindow *p_window_ptr)
{
	if (g_window_fullscreen ^= true)
	{
		glfwSetWindowMonitor(p_window_ptr, glfwGetPrimaryMonitor(), 0, 0, g_display_width, g_display_height, g_display_refresh_rate);
		g_backbuffer.resize(g_display_width, g_display_height);

		glViewport(0, 0, g_display_width, g_display_height);
	}

	else
	{
		glfwSetWindowMonitor(p_window_ptr, NULL, g_window_xpos, g_window_ypos, g_window_width, g_window_height, g_window_refresh_rate);
		g_backbuffer.resize(g_window_width, g_window_height);

		glViewport(0, 0, g_window_width, g_window_height);
	}

	g_new_window = true;
}

std::string getHUDMessage()
{
	if (g_scene.getStatePtr()->stalemate())
		return " - Stalemate";
	
	if (g_scene.getStatePtr()->getBoardPtr()->getWhiteCheckmateRef())
		return " - White Checkmated";

	if (g_scene.getStatePtr()->getBoardPtr()->getBlackCheckmateRef())
		return " - Black Checkmated";

	if (g_scene.getStatePtr()->getBoardPtr()->getWhiteCheckRef())
		return " - White in Check";

	if (g_scene.getStatePtr()->getBoardPtr()->getBlackCheckRef())
		return " - Black in Check";

	return "";
}

void newDialog(GLFWwindow *p_window_ptr)
{
	int *result_ptr = g_new_callback_ptr(p_window_ptr);
	
	if (*result_ptr != ABORT)
	{
		if (g_thread.joinable())
			g_thread.join();
		
		g_backbuffer.init();

		g_scene.init();
		g_scene.getStatePtr()->init(result_ptr);

		g_camera.init(Camera::FIXED);
		glfwSetInputMode(p_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		calcMouseRay(g_xprev, g_yprev);
	}

	g_prev_time = glfwGetTime();
}

void exitDialog(GLFWwindow *p_window_ptr)
{
	if (!g_exit_callback_ptr(p_window_ptr))
		glfwSetWindowShouldClose(p_window_ptr, GLFW_FALSE);

	else if (g_thread.joinable())
		g_thread.join();

	g_prev_time = glfwGetTime();
}

void openDialog(GLFWwindow *p_window_ptr)
{
	std::string path = g_open_callback_ptr(p_window_ptr);

	g_prev_time = glfwGetTime();

	if (path.empty())
		return;

	if (g_thread.joinable())
		g_thread.join();

	g_backbuffer.init();

	g_scene.init();
	g_scene.getStatePtr()->load(path);

	g_camera.init(Camera::FIXED);
	glfwSetInputMode(p_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	calcMouseRay(g_xprev, g_yprev);
}

void saveDialog(GLFWwindow *p_window_ptr)
{
	std::string path = g_save_callback_ptr(p_window_ptr);

	g_prev_time = glfwGetTime();

	if (path.empty())
		return;

	g_scene.getStatePtr()->save(path);
}

// GLFW: Handle window move
// ------------------------
void window_pos_callback(GLFWwindow *p_window_ptr, int p_xpos, int p_ypos)
{
	if (!g_window_fullscreen)
	{
		g_window_xpos = p_xpos;
		g_window_ypos = p_ypos;
	}

	glfwSetWindowPos(p_window_ptr, p_xpos, p_ypos);
}

// GLFW: Handle window refresh
// ---------------------------
void window_refresh_callback(GLFWwindow *p_window_ptr)
{
	g_orthographic = glm::ortho(0.0f, static_cast<float>(containerWidth()), 0.0f, static_cast<float>(containerHeight()));

	if (!g_ready)
	{
		drawLoadScreen("");
		return;
	}

	g_perspective = glm::perspective(glm::radians(g_camera.getZoom()), static_cast<float>(containerWidth()) / static_cast<float>(containerHeight()), 0.1f, 100.0f);

	render();

	--g_frame_count;

	glfwSwapBuffers(p_window_ptr);
}

// GLFW: Handle window minimize
// ----------------------------
void window_iconify_callback(GLFWwindow *p_window_ptr, int p_iconified)
{
	g_window_minimized = p_iconified;
}

// GLFW: Handle window close
// -------------------------
void window_close_callback(GLFWwindow *p_window_ptr)
{
	if (!g_ready)
	{
		glfwTerminate();
		std::exit(EXIT_SUCCESS);
	}
	
	exitDialog(p_window_ptr);
}

// GLFW: Handle framebuffer resize
// -------------------------------
void framebuffer_size_callback(GLFWwindow *p_window_ptr, int p_width, int p_height)
{
	if (!g_window_fullscreen)
	{
		g_window_width = p_width;
		g_window_height = p_height;
	}

	g_backbuffer.resize(p_width, p_height);

	glViewport(0, 0, p_width, p_height);
}

// GLFW: Handle key input
// ----------------------
void key_callback(GLFWwindow *p_window_ptr, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (!g_ready)
		return;
	
	if (p_key == GLFW_KEY_F1 && p_action == GLFW_PRESS)
		newDialog(p_window_ptr);

	if (p_key == GLFW_KEY_F3 && p_action == GLFW_PRESS)
		g_show_debug ^= true;

	if (p_key == GLFW_KEY_F5 && p_action == GLFW_PRESS)
		saveDialog(p_window_ptr);

	if (p_key == GLFW_KEY_F7 && p_action == GLFW_PRESS)
		openDialog(p_window_ptr);

	if (p_key == GLFW_KEY_F9 && p_action == GLFW_PRESS)
		g_show_HUD ^= true;

	if (p_key == GLFW_KEY_F11 && p_action == GLFW_PRESS)
		toggleFullscreen(p_window_ptr);

	if (p_key == GLFW_KEY_1 && p_action == GLFW_PRESS)
		switchPolygonMode();

	if (p_key == GLFW_KEY_2 && p_action == GLFW_PRESS)
		switchCameraMode(p_window_ptr);

	if (p_key == GLFW_KEY_3 && p_action == GLFW_PRESS)
	{
		if (!(g_scene.getStatePtr()->getBoardPtr()->getAnimateRef() ^= g_debug_mode))
			g_scene.getStatePtr()->getBoardPtr()->getAnimationsPtr()->clear();
	}

	if (p_key == GLFW_KEY_4 && p_action == GLFW_PRESS)
		g_render_shadows ^= g_debug_mode;

	if (p_key == GLFW_KEY_SPACE && p_action == GLFW_PRESS)
		setSpeedMod(SPEED_FAST);

	if (p_key == GLFW_KEY_SPACE && p_action == GLFW_RELEASE)
		setSpeedMod(SPEED_NORMAL);

	if (g_camera.getMode() == Camera::FIXED)
	{
		if (p_key == GLFW_KEY_UP && p_action == GLFW_PRESS)
			g_camera.adjustPitch(false);

		if (p_key == GLFW_KEY_DOWN && p_action == GLFW_PRESS)
			g_camera.adjustPitch(true);

		if (p_key == GLFW_KEY_LEFT && p_action == GLFW_PRESS)
			g_camera.adjustYaw(false);

		if (p_key == GLFW_KEY_RIGHT && p_action == GLFW_PRESS)
			g_camera.adjustYaw(true);

		if (p_key == GLFW_KEY_KP_2 && p_action == GLFW_PRESS)
			g_camera.setRotation(89.9f, 270.0f);

		if (p_key == GLFW_KEY_KP_4 && p_action == GLFW_PRESS)
			g_camera.setRotation(89.9f, 0.0f);

		if (p_key == GLFW_KEY_KP_6 && p_action == GLFW_PRESS)
			g_camera.setRotation(89.9f, 180.0f);

		if (p_key == GLFW_KEY_KP_8 && p_action == GLFW_PRESS)
			g_camera.setRotation(89.9f, 90.0f);
	}
}

// GLFW: Handle mouse click
// ------------------------
void mouse_button_callback(GLFWwindow *p_window_ptr, int p_button, int p_action, int p_mods)
{
	if (!g_ready)
		return;
	
	if (g_camera.getMode() == Camera::FIXED)
	{
		if (g_scene.getStatePtr()->getCurrSquarePtr() == nullptr)
		{
			if (p_button == GLFW_MOUSE_BUTTON_1 && p_action == GLFW_PRESS)
				g_camera.setRotatable(true);

			if (p_button == GLFW_MOUSE_BUTTON_1 && p_action == GLFW_RELEASE)
			{
				g_camera.setRotatable(false);
				calcMouseRay(g_xprev, g_yprev);
			}
		}

		else if (p_action == GLFW_PRESS)
			g_scene.getStatePtr()->mouseClick(p_button);
	}
}

// GLFW: Handle mouse move
// -----------------------
void cursor_pos_callback(GLFWwindow *p_window_ptr, double p_xpos, double p_ypos)
{
	if (!g_ready)
		return;
	
	if (g_new_window)
	{
		g_xprev = p_xpos;
		g_yprev = p_ypos;

		g_new_window = false;
	}

	switch (g_camera.getMode())
	{
	case Camera::FIXED:
		g_camera.rotate(p_xpos - g_xprev, p_ypos - g_yprev);
		calcMouseRay(p_xpos, p_ypos);
		break;

	case Camera::FREE:
		g_camera.turn(p_xpos - g_xprev, g_yprev - p_ypos);
		break;
	}

	g_xprev = p_xpos;
	g_yprev = p_ypos;

	g_in_window = (p_xpos >= 0 && p_xpos <= containerWidth() && p_ypos >= 0 && p_ypos <= containerHeight());
}

// GLFW: Handle mouse scroll
// -------------------------
void scroll_callback(GLFWwindow *p_window_ptr, double p_xoffset, double p_yoffset)
{
	if (!g_ready)
		return;
	
	switch (g_camera.getMode())
	{
	case Camera::FIXED:
		g_camera.advance(p_yoffset);
		calcMouseRay(g_xprev, g_yprev);
		break;

	case Camera::FREE:
		g_camera.zoom(p_yoffset);
		break;
	}
}