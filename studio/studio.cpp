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

int test(int argc, char* argv [])
{
	Scene scene;

	auto cam = scene.add<Camera>(math::Vertex(0, 500, -1000), math::Vertex(0, 500, 0));
	auto block = scene.add<Block>(100, 100, 100);

	scene.renderTo(cam.get());

	return 0;
}