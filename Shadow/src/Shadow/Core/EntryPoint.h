#include "Shadow/Core/Tool/Log.h"
#include "Shadow/Core/Application.h"
#include "Shadow/Core/Base.h"
#include <iostream>


int main(int argc, char** argv)
{
    Shadow::Log::Init();
    // 不知道为什么中文乱码
    SD_CORE_TRACE("Welcome to Shadow Engine World!");

	auto app = Shadow::CreateApplication();
	app->Run();
	delete app;
}