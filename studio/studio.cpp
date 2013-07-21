/*
 * Copyright (C) 2013 Marcin Zdun
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include <memory>
#include <iostream>
#include <algorithm>
#include <map>

#include <scene.hpp>
#include <camera.hpp>
#include <block.hpp>
#include <platform_api.hpp>
#include <canvas_types.hpp>

#include <future>
#include <iomanip>

#define STEREO_CANVAS

#if 1
typedef studio::CyanMagentaCanvas<> StereoCanvasType;
#else
typedef studio::SideBySideCanvas<> StereoCanvasType;
#endif

struct Command {
	const char* name;
	int (*command)(int, char* []);
	Command(const char* name, int (*command)(int, char* []))
		: name(name)
		, command(command)
	{}
	int run(int argc, char* argv[])
	{
		return command(argc, argv);
	}
};

template <size_t N>
Command* get_cmmd(Command (&commands)[N], int argc, char* argv[])
{
	if (argc > 1)
		for (size_t i = 0; i < N; ++i)
		{
			if (strcmp(argv[1], commands[i].name) == 0)
				return commands + i;
		}

		if (argc > 1)
			fprintf(stderr, "%s: unknown command: %s\n", argv[0], argv[1]);
		else
			fprintf(stderr, "%s: command missing\n", argv[0]);

		fprintf(stderr, "\nKnown commands are:\n");
		for (size_t i = 0; i < N; ++i)
			fprintf(stderr, "\t%s\n", commands[i].name);

		return nullptr;
}

int test(int, char* []);

Command commands[] = {
	Command("test", test)
};

int main(int argc, char* argv[])
{
	char prog[] = "studio";
	argv[0] = prog;
	Command* command = get_cmmd(commands, argc, argv);

	if (command == nullptr)
		return 1;

	return command->run(argc - 1, argv + 1);
}

using namespace studio;

void setUp(std::shared_ptr<Scene>& scene)
{
	scene->add<Block>(2015, 1235, 1);
	scene->add<Block>(2015, 1, 1060)->translate(0,    0,  -1060);
	scene->add<Block>(2015, 1, 1060)->translate(0, 1235,  -1060);

	scene->add<Block>(425, 300, 210)->translate(1365, 0,   -210);
	scene->add<Block>(425, 300, 210)->translate(940,  0,   -210);
	scene->add<Block>(425, 300, 210)->translate(515,  0,   -210);

	scene->add<Block>(225, 590, 210)->translate(1790, 0,   -210);
	scene->add<Block>(225, 590, 210)->translate(1790, 590, -210);

	scene->add<Block>(225, 780, 160)->translate(1565, 400, -160);
	scene->add<Block>(225, 590, 160)->translate(1340, 590, -160);

	scene->add<Block>(225, 590, 160)->translate(410,  590, -160);
	scene->add<Block>(225, 780, 160)->translate(185,  400, -160);

	scene->add<Block>(577, 352, 48)->translate(700, 545, -96);
}

std::shared_ptr<StereoCanvasType> g_canvas;

int test(int argc, char* argv [])
{
	PlatformAPI init;

	auto scene = std::make_shared<Scene>();
	setUp(scene);

	math::Vertex camPos { 1007.5, 617.5, -1000 };

#ifdef STEREO_CANVAS
	auto canvas = std::make_shared<StereoCanvasType>(1400, 800);
	g_canvas = canvas;
	auto cam = scene->add<StereoCamera>(1000, 50, camPos, camPos + math::Vertex(0, 0, 100));
	cam->setCanvas(canvas.get());
#else
	auto cam = scene->add<Camera>(1000, camPos, camPos + math::Vertex(0, 0, 100));
	auto canvas = cam->create_canvas<SimpleCanvas>(1400, 800);
#endif

	scene->renderAllCameras();
	canvas->save("test.png");

#if 0
	std::vector<std::future<void>> tasks;

	for (size_t id = 0; id < 25; ++id)
	{
		tasks.push_back(std::move(std::async([=](const math::Vertex& camPos){
			std::cout << "<" << std::flush;
			auto cam = scene->add<Camera>(1000, camPos + math::Vertex(50 * id, 0, 0), camPos + math::Vertex(50 * id, 0, 100));
			auto canvas = cam->create_canvas<BmpCanvas>(1400, 800);
			scene->renderTo(cam.get());

			wchar_t fname[100];
			swprintf_s(fname, L"test_%04d.png", id);
			canvas->save(fname, L"image/png");
			std::cout << ">" << std::flush;
		}, math::Vertex{ 507.5, 800, -1000 })));
	}

	for (auto && f : tasks)
		f.get();

	tasks.clear();
#endif

	g_canvas.reset();
	scene.reset();

	return 0;
}