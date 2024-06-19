#include <iostream>

int main(int argc, char** argv)
{
	auto app = Shadow::CreateApplication();
	app->Run();
	delete app;
}