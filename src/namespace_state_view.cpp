/*
	Client library for mastermind
	Copyright (C) 2013-2014 Yandex

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "namespace_state_p.hpp"
#include "couple_sequence_p.hpp"

namespace mastermind {

size_t
namespace_state_t::settings_t::groups_count() const {
	return namespace_state.data->settings.groups_count;
}

const std::string &
namespace_state_t::settings_t::success_copies_num() const {
	return namespace_state.data->settings.success_copies_num;
}

const namespace_state_t::user_settings_t &
namespace_state_t::settings_t::user_settings() const {
	if (!namespace_state.data->settings.user_settings_ptr) {
		throw std::runtime_error("uninitialized user settings cannot be used");
	}

	return *namespace_state.data->settings.user_settings_ptr;
}

namespace_state_t::settings_t::settings_t(const namespace_state_t &namespace_state_)
	: namespace_state(namespace_state_)
{
}

struct namespace_state_t::groupset_t::data_t : public namespace_state_t::data_t::couples_t::groupset_info_t
{
};

namespace_state_t::groupset_t::groupset_t(const data_t &data_)
	: data(data_)
{
}

uint64_t
namespace_state_t::groupset_t::free_effective_space() const {
	return data.free_effective_space;
}

uint64_t
namespace_state_t::groupset_t::free_reserved_space() const {
	return data.free_reserved_space;
}

std::string
namespace_state_t::groupset_t::type() const {
	if (data.type == namespace_state_t::data_t::couples_t::groupset_info_t::type_tag::LRC) {
		return "lrc";
	}
	return "UNKNOWN";
}

std::string
namespace_state_t::groupset_t::status() const {
	if (data.status == namespace_state_t::data_t::couples_t::groupset_info_t::status_tag::BAD) {
		return "BAD";
	}
	return "UNKNOWN";
}

std::string
namespace_state_t::groupset_t::id() const {
	return data.id;
}

const std::vector<int> &
namespace_state_t::groupset_t::group_ids() const {
	return data.groups;
}

const kora::dynamic_t &
namespace_state_t::groupset_t::hosts() const {
	return data.hosts;
}

const kora::dynamic_t &
namespace_state_t::groupset_t::settings() const {
	return data.settings;
}

std::vector<std::string>
namespace_state_t::couples_t::get_couple_read_preference(group_t group) const {
	auto it = namespace_state.data->couples.group_info_map.find(group);

	if (it == namespace_state.data->couples.group_info_map.end() ||
		it->second.couple_info_map_iterator->second.read_preference.empty()) {
		return {"replicas"};
	}

	return it->second.couple_info_map_iterator->second.read_preference;
}

namespace_state_t::groupset_t
namespace_state_t::couples_t::get_couple_groupset(group_t group, const std::string &groupset_id) const {
	auto cit = namespace_state.data->couples.group_info_map.find(group);

	if (cit == namespace_state.data->couples.group_info_map.end()) {
		throw unknown_group_error{group};
	}

	const auto & map = cit->second.couple_info_map_iterator->second.groupset_info_map;
	auto pit = map.find(groupset_id);
	if (pit == map.end()) {
		throw unknown_groupset_error{groupset_id};
	}

	return groupset_t(static_cast<const mastermind::namespace_state_t::groupset_t::data_t&>(pit->second));
}

std::vector<std::string>
namespace_state_t::couples_t::get_couple_groupset_ids(group_t group) const {
	auto cit = namespace_state.data->couples.group_info_map.find(group);

	if (cit == namespace_state.data->couples.group_info_map.end()) {
		throw unknown_group_error{group};
	}

	const auto & map = cit->second.couple_info_map_iterator->second.groupset_info_map;

	std::vector<std::string> groupset_ids;

	for (auto it = map.begin(); it != map.end(); it++) {
		groupset_ids.emplace_back(it->first);
	}

	// TODO: remove when "replicas" becomes a separate groupset in 'groupset_info_map'
	{
		auto it = find(groupset_ids.begin(), groupset_ids.end(), "replicas");
		if (it == groupset_ids.end()) {
			groupset_ids.emplace_back("replicas");
		}
	}

	return groupset_ids;
}

groups_t
namespace_state_t::couples_t::get_couple_groups(group_t group) const {
	auto it = namespace_state.data->couples.group_info_map.find(group);

	if (it == namespace_state.data->couples.group_info_map.end()) {
		return groups_t();
	}

	return it->second.couple_info_map_iterator->second.groups;
}

groups_t
namespace_state_t::couples_t::get_groups(group_t group) const {
	auto groups = get_couple_groups(group);

	if (groups.empty()) {
		return {group};
	}
	return groups;
}

uint64_t
namespace_state_t::couples_t::free_effective_space(group_t group) const {
	auto it = namespace_state.data->couples.group_info_map.find(group);

	if (it == namespace_state.data->couples.group_info_map.end()) {
		return 0;
	}

	return it->second.couple_info_map_iterator->second.free_effective_space;
}

uint64_t
namespace_state_t::couples_t::free_reserved_space(group_t group) const {
	auto it = namespace_state.data->couples.group_info_map.find(group);

	if (it == namespace_state.data->couples.group_info_map.end()) {
		return 0;
	}

	return it->second.couple_info_map_iterator->second.free_reserved_space;
}

kora::dynamic_t
namespace_state_t::couples_t::hosts(group_t group) const {
	auto it = namespace_state.data->couples.group_info_map.find(group);

	if (it == namespace_state.data->couples.group_info_map.end()) {
		throw unknown_group_error{group};
	}

	return it->second.couple_info_map_iterator->second.hosts;
}

namespace_state_t::couples_t::couples_t(const namespace_state_t &namespace_state_)
	: namespace_state(namespace_state_)
{
}

groups_t
namespace_state_t::weights_t::groups(uint64_t size) const {
	return namespace_state.data->weights.get(size).groups;
}

couple_sequence_t
namespace_state_t::weights_t::couple_sequence(uint64_t size) const {
	auto data = std::make_shared<couple_sequence_init_t::data_t>(
			namespace_state.data->weights.get_all(size));
	return couple_sequence_init_t(std::move(data));
}

void
namespace_state_t::weights_t::set_feedback(group_t couple_id
		, feedback_tag feedback) {
	switch (feedback) {
	case feedback_tag::available:
		namespace_state.data->weights.set_coefficient(couple_id, 1);
		break;
	case feedback_tag::partly_unavailable:
		namespace_state.data->weights.set_coefficient(couple_id, 0.1);
		break;
	case feedback_tag::temporary_unavailable:
		namespace_state.data->weights.set_coefficient(couple_id, 0.01);
		break;
	case feedback_tag::permanently_unavailable:
		namespace_state.data->weights.set_coefficient(couple_id, 0);
		break;
	default:
		throw unknown_feedback(couple_id, static_cast<int>(feedback));
	}

}

namespace_state_t::weights_t::weights_t(const namespace_state_t &namespace_state_)
	: namespace_state(namespace_state_)
{
}

bool
namespace_state_t::statistics_t::ns_is_full() {
	return namespace_state.data->statistics.ns_is_full();
}

namespace_state_t::statistics_t::statistics_t(const namespace_state_t &namespace_state_)
	: namespace_state(namespace_state_)
{
}

namespace_state_t::settings_t
namespace_state_t::settings() const {
	return settings_t(*this);
}

namespace_state_t::couples_t
namespace_state_t::couples() const {
	return couples_t(*this);
}

namespace_state_t::weights_t
namespace_state_t::weights() const {
	return weights_t(*this);
}

namespace_state_t::statistics_t
namespace_state_t::statistics() const {
	return statistics_t(*this);
}

const std::string &namespace_state_t::name() const {
	return data->name;
}

namespace_state_t::operator bool() const {
	return static_cast<bool>(data);
}

} // namespace mastermind
