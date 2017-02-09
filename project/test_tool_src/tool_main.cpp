/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
Description : PandoraGS test utility -- entry point and test functions
*******************************************************************************/
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <Windows.h>
#include <tchar.h>
#include <string>
using namespace std;
#include "../src/psemu_main.h" // plugin PSEmu interface
#include "window_manager.h"
#include "primitive_builder.h"
#include "plugin_loader.h"
#include "tool_main.h"

int32_t getUserInput(const char* title, const int32_t min, const int32_t max); ///< Get user input (integer value)


/// @brief Test utility - window creation
/// @param[in] hInstance      Application instance handle
/// @param[in] hPrevInstance  Previous instance handle
/// @param[in] lpCmdLine      Command line arguments
/// @param[in] nCmdShow       Startup window display flag
/// @returns Exit code
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	WindowManager menu(hInstance, hPrevInstance, lpCmdLine);
	return menu.showDialog(nCmdShow);
}

/// @brief Plugin dialog testing
/// @param[in] type  Dialog type
void openDialog(plugin_dialog_t type)
{
    if (type == plugin_dialog_t::config)
        GPUconfigure();
    else
        GPUabout();
}

/// @brief Launch plugin and start demonstration sequence
/// @param[in] hWindow  Main window handle
void startDemoSequence(HWND hWindow)
{
    try
    {
        PluginLoader loader(hWindow);
        loader.playDemoSequence();
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}

/// @brief Load plugin unit testing system
/// @param[in] hWindow  Main window handle
void startUnitTesting(HWND hWindow)
{
    try
    {
        PluginLoader loader(hWindow);
        if (loader.checkUnits())
            printf("\nSUCCESS: all the tests were successful.\n");
        else
            printf("\nWARNING: some tests have failed...\n");

        fflush(stdout);
        system("pause");
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}

/// @brief Custom primitive testing
/// @param[in] hWindow  Main window handle
void startPrimitiveTesting(HWND hWindow)
{
    try
    {
		int32_t maxVal, inputVal;
		primitive_category_t primitiveType;
		shape_rendering_t renderingType;
		shape_type_t primitiveSubtype = shape_type_t::flat_base;
		rectangle_type_t rectangleSubtype = rectangle_type_t::dot;
		bool isFlipped = false;
		bool isSemiTransparent;

        PluginLoader loader(hWindow); // load plugin
        do
        {
			// get primitive type
			maxVal = PrimitiveBuilder::listPrimitiveCategories();
			inputVal = getUserInput("Primitive type", 0, maxVal);
			primitiveType = PrimitiveBuilder::getPrimitiveCategory(inputVal);
			if (inputVal == 0)
				break;
			
			// get primitive sub-type
			maxVal = PrimitiveBuilder::listShapeTypes(primitiveType);
			if (primitiveType == primitive_category_t::rectangle)
			{
				inputVal = getUserInput("Shape", -maxVal, maxVal);
				rectangleSubtype = PrimitiveBuilder::getRectangleType(inputVal);
				isFlipped = (inputVal < 0);
			}
			else
			{
				inputVal = getUserInput("Shape", 0, maxVal);
				primitiveSubtype = PrimitiveBuilder::getShapeType(inputVal);
			}
			if (inputVal == 0)
				break;

			// get rendering type
			maxVal = PrimitiveBuilder::listRenderingTypes();
			printf(" Negative value : enable semi-transparency");
			inputVal = getUserInput("Rendering", -maxVal, maxVal);
			isSemiTransparent = (inputVal < 0);
			renderingType = PrimitiveBuilder::getRenderingType((inputVal < 0) ? -inputVal : inputVal);

            // draw primitive
			if (inputVal != 0)
			{
				switch (primitiveType)
				{
					case primitive_category_t::line: loader.drawLine(primitiveSubtype, isSemiTransparent); break;
					case primitive_category_t::rectangle: loader.drawRectangle(rectangleSubtype, renderingType, isSemiTransparent, isFlipped); break;
					default: loader.drawPolygon(primitiveSubtype, renderingType, isSemiTransparent); break;
				}
			}
        }
        while (inputVal != 0);
    }
    catch (const std::exception& exc)
    {
        printf("%s", exc.what());
    }
}

/// @brief Get user input (integer value)
/// @param[in] title  Name of value
/// @param[in] min    Min value
/// @param[in] max    Max value
int32_t getUserInput(const char* title, const int32_t min, const int32_t max)
{
	char buffer;
	int32_t inputVal;
	if (max < min)
		return 0;
	printf("Enter 0 to exit\n");

	do
	{
		printf("%s (%d to %d): ", title, min, max);
		fflush(stdin);
		inputVal = 0;
		while ((buffer = getchar()) != EOF && buffer != '\n')
		{
			if (buffer >= '0' && buffer <= '9')
				inputVal = (inputVal * 10) + (buffer - '0');
			else if (buffer == '-')
				inputVal *= -1;
		}
	} 
	while (inputVal < min || inputVal > max);
	return inputVal;
}
