/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_OPERATIONS_COMPLEMENT_HPP_INCLUDED
#define QUIVER_OPERATIONS_COMPLEMENT_HPP_INCLUDED

#include <quiver/util.hpp>
#include <quiver/adjacency_list.hpp>
#include <type_traits>
#include <vector>
#include <algorithm>

namespace quiver
{
	template<typename graph_t>
	std::enable_if_t<is_directed_v<std::decay_t<graph_t>>, std::decay_t<graph_t>> complement(graph_t const& graph)
	{
		std::decay_t<graph_t> result = graph.vertices();
		std::vector<bool> checklist(graph.V());
		for(vertex_index_t v = 0; v < graph.V(); ++v) {
			std::fill(checklist.begin(), checklist.end(), false);
			for(auto&& out_edge : graph.vertex(v).out_edges)
				checklist[out_edge.to] = true;
			vertex_index_t o = 0;
			for(; o < v; ++o)
				if(checklist[o] == false)
					result.add_edge(v, o);
			++o; // skip o = v
			for(; o < graph.V(); ++o)
				if(checklist[o] == false)
					result.add_edge(v, o);
		}
		return result;
	}
	template<typename graph_t>
	std::enable_if_t<is_undirected_v<std::decay_t<graph_t>>, std::decay_t<graph_t>> complement(graph_t const& graph)
	{
		std::decay_t<graph_t> result = graph.vertices();
		std::vector<bool> checklist(graph.V());
		for(vertex_index_t v = 0; v < graph.V(); ++v) {
			std::fill(checklist.begin(), checklist.end(), false);
			for(auto&& out_edge : graph.vertex(v).out_edges)
				checklist[out_edge.to] = true;
			vertex_index_t o = 0;
			for(; o < v; ++o)
				if(checklist[o] == false)
					result.add_edge(v, o);
		}
		return result;
	}
}

#endif // !QUIVER_OPERATIONS_COMPLEMENT_HPP_INCLUDED
