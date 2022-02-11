// Copyright (c) 2021 MIT Digital Currency Initiative,
//                    Federal Reserve Bank of Boston
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OPENCBDC_TX_SRC_COMMON_HASHMAP_H_
#define OPENCBDC_TX_SRC_COMMON_HASHMAP_H_

#include "crypto/siphash.h"
#include "hash.hpp"

#include <array>
#include <cstring>

namespace cbdc::hashing {
    /// \brief SipHash function to generate STL data structure hash keys for
    ///        system IDs.
    ///
    /// [TODO: Uses randomly generated initialization keys to insure that
    /// colliding values will differ from run to run.] For transaction data
    /// that comes from users, it is important to key data in sets and hashmaps
    /// non-deterministically. Otherwise, malicious actors might pre-calculate
    /// colliding values and spam the system with them to cripple performance.
    /// \tparam T hash-like type to hash.
    template<class T>
    struct const_sip_hash {
        static_assert(std::is_trivial_v<T>);
        auto operator()(T const& tx) const noexcept -> size_t {
            // TODO: pick the initialization key at random.
            static constexpr std::array<uint64_t, 2> siphash_key{0x1337,
                                                                 0x1337};
            CSipHasher hasher(siphash_key[0], siphash_key[1]);
            std::array<unsigned char, sizeof(tx)> tx_arr{};
            std::memcpy(tx_arr.data(), &tx, sizeof(tx));
            hasher.Write(tx_arr.data(), sizeof(tx));
            return hasher.Finalize();
        }
    };

    /// \brief Uses the raw value of a provided hash as an STL data structure
    ///        hash key.
    ///
    /// \warning Only use with hashes generated by functions where the entropy
    ///          source is sufficiently random to guarantee a low likelihood of
    ///          collisions.
    struct null {
        auto operator()(const hash_t& hash) const noexcept -> size_t;
    };
}

#endif // OPENCBDC_TX_SRC_COMMON_HASHMAP_H_
