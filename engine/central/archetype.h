#pragma once
#include "engine/central/toolbox.h"
#include "engine/central/storage.h"
#include "engine/nekolib/templates/type_union.h"
#include <tuple>
#include <vector>

namespace kodanuki
{

template <typename ... Predicates>
struct Archetype
{
	using iterate_types = type_union_t<typename Predicates::iterate_types...>;
	using include_types = type_union_t<typename Predicates::include_types...>;
	using exclude_types = type_union_t<typename Predicates::exclude_types...>;
	using consume_types = type_union_t<typename Predicates::consume_types...>;
	using produce_types = type_union_t<typename Predicates::produce_types...>;

	static auto iterate(EntityMapping& mapping)
	{
		std::vector<Entity> includes = search_entities<include_types>(mapping);
		std::vector<Entity> excludes = search_entities<exclude_types>(mapping);
		std::vector<Entity> entities;
		std::set_difference(includes.begin(), includes.end(),
			excludes.begin(), excludes.end(), std::back_inserter(entities));
		remove_entity_tags<consume_types>(entities);
		update_entity_tags<produce_types>(entities);
		return EntityIterator<iterate_types>(mapping, entities, 0);
	}
};

template <typename ... T>
struct Iterate
{
	using iterate_types = std::tuple<T...>;
	using include_types = std::tuple<T...>;
	using exclude_types = std::tuple<>;
	using consume_types = std::tuple<>;
	using produce_types = std::tuple<>;
};

template <typename ... T>
struct Require
{
	using iterate_types = std::tuple<>;
	using include_types = std::tuple<T...>;
	using exclude_types = std::tuple<>;
	using consume_types = std::tuple<>;
	using produce_types = std::tuple<>;
};

template <typename ... T>
struct Exclude
{
	using iterate_types = std::tuple<>;
	using include_types = std::tuple<>;
	using exclude_types = std::tuple<T...>;
	using consume_types = std::tuple<>;
	using produce_types = std::tuple<>;
};

template <typename ... T>
struct Consume
{
	using iterate_types = std::tuple<>;
	using include_types = std::tuple<T...>;
	using exclude_types = std::tuple<>;
	using consume_types = std::tuple<T...>;
	using produce_types = std::tuple<>;
};

template <typename ... T>
struct Produce
{
	using iterate_types = std::tuple<>;
	using include_types = std::tuple<>;
	using exclude_types = std::tuple<T...>;
	using consume_types = std::tuple<>;
	using produce_types = std::tuple<T...>;
};

template <typename ... T>
struct Calculate
{
	using iterate_types = std::tuple<T...>;
	using include_types = std::tuple<>;
	using exclude_types = std::tuple<T...>;
	using consume_types = std::tuple<>;
	using produce_types = std::tuple<T...>;
};

}

