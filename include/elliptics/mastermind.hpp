// elliptics-fastcgi - FastCGI-module component for Elliptics file storage
// Copyright (C) 2011 Leonid A. Movsesjan <lmovsesjan@yandex-team.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#ifndef INCLUDE__ELLIPTICS__MASTERMIND_HPP
#define INCLUDE__ELLIPTICS__MASTERMIND_HPP

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace elliptics {

struct group_info_response_t {
	std::vector<std::string> nodes;
	std::vector<int> couples;
	int status;
};

class mastermind_t {
public:
	mastermind_t(const std::string &host, uint16_t port, int wait_timeout = 0, int group_info_update_period = 60);
	~mastermind_t();

	std::vector<int> get_metabalancer_groups(uint64_t count = 0);
	group_info_response_t get_metabalancer_group_info(int group);
	std::vector<std::vector<int> > get_symmetric_groups();
	std::vector<int> get_symmetric_groups(int group);
	std::map<int, std::vector<int> > get_bad_groups();
	std::vector<int> get_all_groups();
private:
	struct data;
	std::unique_ptr<data> m_data;
};

} // namespace elliptics

#endif /* INCLUDE__ELLIPTICS__MASTERMIND_HPP */
