/*
 *	Quiver - A graph theory library
 *	Copyright (C) 2018 Josua Rieder (josua.rieder1996@gmail.com)
 *	Distributed under the MIT License.
 *	See the enclosed file LICENSE.txt for further information.
 */

#ifndef QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED
#define QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <quiver/adjacency_list.hpp>
#include <quiver/disjoint_set.hpp>
#include <cstddef>
#include <vector>
#include <unordered_map>

namespace quiver
{
	template<typename graph_t>
	disjoint_set<> get_disjoint_set(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "connected components exist in undirected graphs");

		disjoint_set<> cc(graph.V());
		const auto v_end = graph.v_end();
		std::size_t v_index = 0;
		for(auto vert = graph.v_begin(); vert != v_end; ++vert, ++v_index) {
			auto const& out_edges = vert->out_edges;
			const auto e_end = out_edges.end();
			for(auto edge = out_edges.begin(); edge != e_end; ++edge)
				if(v_index < edge->to) // we only need half of the actual edges
					cc.unite(v_index, edge->to);
		}
		return cc;
	}

	template<
		directivity_t dir,
		typename edge_properties_t,
		typename vertex_properties_t,
		template<typename> class out_edge_container,
		template<typename> class vertex_container
	>
	std::vector<adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>>
	split_ccs(adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container> const& graph)
	{
		auto ds = get_disjoint_set(graph);					// vertex [0..V] -> root [0..V]

		std::vector<std::size_t> cc_index(graph.V());		// vertex [0..V] -> cc index [0..|CC|]
		{
			std::size_t counter = 0;
			std::unordered_map<std::size_t, std::size_t> cc_map;
			for(vertex_index_t v = 0; v < graph.V(); ++v) {
				const std::size_t root = ds.find(v);
				auto insertion = cc_map.emplace(root, counter);
				if(insertion.second)
					++counter;
				cc_index[v] = insertion.first->second;
			}
		}

		std::vector<std::size_t> cc_relative(graph.V());	// vertex [0..V] -> index in cc [0..V-|CC|+1]
		{
			std::vector<std::size_t> counters(ds.sets(), 0);
			for(vertex_index_t v = 0; v < graph.V(); ++v)
				cc_relative[v] = counters[cc_index[ds.find(v)]]++;
		}

		std::vector<adjacency_list<dir, edge_properties_t, vertex_properties_t, out_edge_container, vertex_container>> result(ds.sets());
		// for(vertex_index_t i = 0; i < result.size(); ++i)
		// 	result[i].reserve(...);
		for(vertex_index_t v = 0; v < graph.V(); ++v) {
			auto& cc = result[cc_index[v]];
			auto index = cc.add_vertex(std::move(graph.vertex(v)));
			for(auto& out_edge : cc.vertex(index).out_edges)
				out_edge.to = cc_relative[out_edge.to];
		}
		return result;
	}

	template<typename graph_t>
	std::size_t ccs(graph_t const& graph)
	{
		static_assert(is_undirected_v<graph_t>, "connected components exist in undirected graphs");
		return get_disjoint_set(graph).sets();
	}
}

#endif // !QUIVER_CONNECTED_COMPONENTS_HPP_INCLUDED