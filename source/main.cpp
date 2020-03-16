/*
 * Controller Poller for OJDS-NX
 * Copyright (C) 2020 Nichole Mattera
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <jansson.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#define BUFFER_SIZE 768

using namespace std::chrono;

int main(int argc, char * argv[]) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <ip-address> [poll-rate]" << std::endl;
		return 1;
	}

	int pollRate = 0;
	if (argc == 3) {
		try {
			pollRate = std::stoi(argv[2]);
		} catch (std::exception const &e) {
			std::cout << "Error: Bad poll rate." << std::endl;
			return 3;
		}
	}

	auto server_sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &(server.sin_addr)) == 0) {
		std::cout << "Error: Bad IP Address." << std::endl;
		return 2;
	}
	server.sin_port = htons(56709);

	auto addr = (struct sockaddr *) &server;
	while (connect(server_sock, addr, sizeof(server)) < 0) {}

	milliseconds start;
	auto buffer = new char[BUFFER_SIZE];
	while (1) {
		if (argc == 3) {
			start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		}
		write(server_sock, "1", 1);

		memset(buffer, 0, BUFFER_SIZE);
		while (recv(server_sock, buffer, BUFFER_SIZE, 0) < 1) {}

		auto json = strchr(buffer, '#');
		if (json == NULL) {
			continue;
		}
		json += 1;

		auto root = json_loads(json, 0, NULL);
		if (!root) {
			continue;
		}

		auto axes = json_object_get(root, "axes");
		if (!axes) {
			json_decref(root);
			continue;
		}

		auto buttons = json_object_get(root, "buttons");
		if (!buttons) {
			json_decref(root);
			continue;
		}

		size_t i = 0;
		auto axesSize = json_array_size(axes);
		for (i = 0; i < axesSize; i++) {
			auto value = json_array_get(axes, i);
			std::cout << json_real_value(value) << ",";
		}

		auto buttonsSize = json_array_size(buttons);
		for (i = 0; i < buttonsSize; i++) {
			auto valueObj = json_array_get(buttons, i);
			auto value = json_object_get(valueObj, "value");
			if (i == buttonsSize - 1) {
				std::cout << json_integer_value(value) << std::endl;
			} else {
				std::cout << json_integer_value(value) << ",";
			}
		}

		json_decref(root);

		if (argc == 3) {
			auto end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			auto duration = (end - start).count();
			if (duration < pollRate) {
				std::this_thread::sleep_for(milliseconds(pollRate - duration));
			}
		}
	}

	close(server_sock);
	delete buffer;
	return 0;
}
