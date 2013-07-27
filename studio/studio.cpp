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

#include <sstream>

#define DEPTH_BUFFER
//#define STEREO_CAMERA
#define CYAN_MAGENTA

namespace studio
{
#ifdef DEPTH_BUFFER
#  ifdef STEREO_CAMERA
	typedef GrayscaleDepthBitmap BasicBitmap;
#  else
	typedef ColorDepthBitmap BasicBitmap;
#  endif
#else
	typedef GrayscaleBitmap BasicBitmap;
#endif

#ifdef STEREO_CAMERA
#  ifdef CYAN_MAGENTA
	typedef CyanMagentaCanvas<BasicBitmap> CanvasType;
#  else
	typedef SideBySideCanvas<BasicBitmap> CanvasType;
#  endif
#else
	typedef SimpleCanvas<BasicBitmap> CanvasType;
#endif

	template <typename Canvas>
	struct CanvasTraits;

	template <typename Bitmap>
	struct CanvasTraits<CyanMagentaCanvas<Bitmap>>
	{
		typedef StereoCamera CameraType;
	};

	template <typename Bitmap>
	struct CanvasTraits<SideBySideCanvas<Bitmap>>
	{
		typedef StereoCamera CameraType;
	};

	template <typename Bitmap>
	struct CanvasTraits<SimpleCanvas<Bitmap>>
	{
		typedef Camera CameraType;
	};
}

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

template <typename It>
class range_t
{
	It _begin;
	It _end;
public:
	typedef It iterator;
	range_t(It b, It e) : _begin(b), _end(e) {}
	iterator begin() { return _begin; }
	iterator end() { return _end; }
};

template <typename It>
range_t<It> make_range(It from, It to) { return { from, to }; }

void setUp(std::shared_ptr<Scene>& scene)
{
	scene->add<Block>(2015, 1235, 1);
	scene->add<Block>(2015, 1, 1060)->translate(0,    0,  -1060);
	scene->add<Block>(2015, 1, 1060)->translate(0, 1235,  -1060);
	/*scene->add<Block>(1, 1235, 1060)->translate(-1, 0, -1060);
	scene->add<Block>(1, 1235, 1060)->translate(2016, 0, -1060);*/

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

	auto wall = ((studio::Block*)scene->begin()->get());
	auto wall_color = std::make_shared<studio::SimpleMaterial>(0xff, 0xfd, 0xe6);
	for (int i = 0; i < 6; ++i)
		wall->setMaterialForSide(i, wall_color);

	auto floor = ((studio::Block*)(scene->begin() + 1)->get()); //d5a862
	auto floor_color = std::make_shared<studio::SimpleMaterial>(0xd5, 0xa8, 0x62);
	for (int i = 0; i < 6; ++i)
		floor->setMaterialForSide(i, floor_color);

	auto tv = ((studio::Block*)(scene->end() - 1)->get());
	auto tv_color = std::make_shared<studio::SimpleMaterial>(0x11, 0x11, 0x11);
	for (int i = 0; i < 6; ++i)
		tv->setMaterialForSide(i, tv_color);

	auto side_color = std::make_shared<studio::SimpleMaterial>(0xff, 0xff, 0xa0);
	auto front_color = std::make_shared<studio::SimpleMaterial>(0x26, 0x13, 0x05);

	for (auto && solid : make_range(scene->begin() + 3, scene->end() - 1))
	{
		auto block = ((studio::Block*)solid.get());
		for (int i = 1; i < 6; ++i)
			block->setMaterialForSide(i, side_color);
		block->setMaterialForSide(0, front_color);
	}
}

void lights(const std::shared_ptr<studio::Scene>& scene)
{
	math::Vertex lightPos { fixed(1007.5), 1030, -1000 }; //same as camera's

	scene->add<studio::SimpleLight>(lightPos + math::Vertex(100, 0, 0), 100);
	scene->add<studio::SimpleLight>(lightPos + math::Vertex(-100, 0, 0), 100);
	scene->add<studio::SimpleLight>(math::Vertex{100, 100, -500}, 100);
}

template <typename CanvasT>
std::shared_ptr<CanvasT> create_canvas(const std::shared_ptr<studio::Scene>& scene)
{
	math::Vertex camPos { fixed(1007.5), fixed(617.5), -1000 };
	math::Vertex camTarget { camPos + math::Vertex(0, 0, 100) };

	auto cam = scene->add<studio::CanvasTraits<CanvasT>::CameraType>(1000, camPos, camTarget);
	return cam->create_canvas<CanvasT>(1400, 800);
}

std::shared_ptr<PlatformBitmap<BitmapType::G8>> calcShadow(Camera* camera, CanvasType* canvas, Light* light, int i)
{
	std::ostringstream o;
	o << "shadow_" << i << ".png";

	auto _pos = light->position();
	camera->transform(_pos, math::Matrix::identity());
	auto pos = camera->project(_pos);
	auto shadow = canvas->calcShadow(pos, _pos.z());
	shadow->save(o.str().c_str());
	return shadow;
}

int test(int argc, char* argv [])
{
	PlatformAPI init;

	auto scene = std::make_shared<Scene>();
	setUp(scene);
	lights(scene);
	auto canvas = create_canvas<CanvasType>(scene);
#ifdef DEPTH_BUFFER
	canvas->setRenderType(Render::Solid);
#endif

	scene->renderAllCameras();
#if defined(DEPTH_BUFFER) && !defined(STEREO_CAMERA)
	canvas->saveDepths("depths.png");

	auto && lights = scene->lights();
	auto i = 0;
	auto cams = scene->cameras();
	auto icam = cams.front();

	// casting to this type, 'cos I don't want the program to crash but rather have
	// it not compiled on the matching lambda to the async
	auto camera = std::static_pointer_cast<CanvasTraits<CanvasType>::CameraType>(icam);

	std::vector< std::future< std::shared_ptr<PlatformBitmap<BitmapType::G8>> > > tasks;

	for (auto && light : lights)
	{
		tasks.push_back(std::async([=](Camera* camera, CanvasType* canvas, Light* light, int i) {
			std::ostringstream o1, o2;

			o1 << "<" << i;
			std::cout << o1.str() << std::flush;

			auto sh = calcShadow(camera, canvas, light, i);
			o2 << i << ">";
			std::cout << o2.str() << std::flush;
			return sh;
		}, camera.get(), canvas.get(), light.get(), ++i));
	}

	for (auto && task : tasks)
	{
		auto sh = task.get();
		canvas->applyShadow(sh.get());
	}

	for (auto && light : lights)
	{
		[=](Camera* camera, CanvasType* canvas, Light* light){
			auto p0 = light->position();
			camera->transform(p0, math::Matrix::identity());
			auto p1 = camera->project(p0);
			p1 = canvas->tr(p1);
			canvas->plot(cast<int>(p1.x()), cast<int>(p1.y()), Color(0xFF, 1, 1));
		}(camera.get(), canvas.get(), light.get());
	}

#endif
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

	scene.reset();

	return 0;
}