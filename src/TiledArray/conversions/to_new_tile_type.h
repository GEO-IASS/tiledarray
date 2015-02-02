#pragma once
#ifndef TILEDARRAY_TONEWTILETYPE_H__INCLUDED
#define TILEDARRAY_TONEWTILETYPE_H__INCLUDED

#include <TiledArray/array.h>

namespace TiledArray {
namespace conversion {

/// Function to convert an array to a new array with a different tile type.

template <
    typename T, unsigned int DIM, typename Tile, typename Policy, typename Fn,
    typename std::enable_if<!std::is_same<Tile, typename Fn::TileType>::value,
                            Tile>::type * = nullptr>
Array<T, DIM, typename std::result_of<Fn(Tile)>::type, Policy> to_new_tile_type(
    Array<T, DIM, Tile, Policy> const &old_array, Fn converting_function) {
    using TileType = typename std::result_of<Fn(Tile)>::type;

    auto new_array = Array<T, DIM, TileType, Policy>{
        old_array.get_world(), old_array.trange(), old_array.get_shape()};

    const auto end = old_array.end();
    for (auto it = old_array.begin(); it != end; ++it) {
        auto const &old_tile = it->get();
        const auto ord = it.ordinal();

        new_array.set(ord, converting_function(old_tile));
    }

    return new_array;
}

}  // namespace conversion
}  // namespace TiledArray
#endif /* end of include guard: TILEDARRAY_TONEWTILETYPE_H__INCLUDED */
