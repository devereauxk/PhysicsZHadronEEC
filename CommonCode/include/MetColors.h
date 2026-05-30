#ifndef METCOLORS_H
#define METCOLORS_H

// MetColors.h — MetBrewer color palettes for ROOT
// Palettes from https://github.com/BlakeRMills/MetBrewer (CC0-1.0)
// All 59 palettes included as vector<Int_t> compatible with ROOT's color system.
//
// Usage:
//   std::vector<Int_t> cols = MetColors("Egypt");          // all colors, discrete order
//   std::vector<Int_t> cols = MetColors("Hiroshige", 5);   // 5 colors, discrete order
//   std::vector<Int_t> cols = MetColors("VanGogh1", 20, true); // 20 interpolated colors
//   bool cb = MetIsColorblind("Egypt");                    // colorblind-friendly check
//   auto names = MetPaletteNames();                        // list all palette names

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <cstdio>
#include "TColor.h"

namespace MetBrewInternal {

struct PalDef {
    std::vector<const char*> hex;
    std::vector<int> order; // 1-based indices into hex
    bool colorblind;
};

inline void hexToFloat(const char* h, float& r, float& g, float& b) {
    unsigned int v = 0;
    sscanf(h + 1, "%06x", &v);
    r = ((v >> 16) & 0xFF) / 255.f;
    g = ((v >>  8) & 0xFF) / 255.f;
    b = ( v        & 0xFF) / 255.f;
}

inline Int_t rootColor(const char* h) {
    float r, g, b;
    hexToFloat(h, r, g, b);
    return TColor::GetColor(r, g, b);
}

inline Int_t interpColor(const std::vector<const char*>& hex, float t) {
    int n = (int)hex.size();
    if (n == 1) return rootColor(hex[0]);
    float s = t * (n - 1);
    int lo = (int)s, hi = lo + 1;
    if (hi >= n) { lo = n - 2; hi = n - 1; }
    float f = s - lo;
    float r0, g0, b0, r1, g1, b1;
    hexToFloat(hex[lo], r0, g0, b0);
    hexToFloat(hex[hi], r1, g1, b1);
    return TColor::GetColor(r0 + f*(r1-r0), g0 + f*(g1-g0), b0 + f*(b1-b0));
}

inline const std::map<std::string, PalDef>& palettes() {
    static const std::map<std::string, PalDef> P = {
        {"Archambault", {
            {"#88a0dc","#381a61","#7c4b73","#ed968c","#ab3329","#e78429","#f9d14a"},
            {2,7,5,1,6,4,3}, true}},
        {"Austria", {
            {"#a40000","#16317d","#007e2f","#ffcd12","#b86092","#721b3e","#00b7a7"},
            {1,2,3,4,6,5,7}, false}},
        {"Benedictus", {
            {"#9a133d","#b93961","#d8527c","#f28aaa","#f9b4c9","#f9e0e8","#ffffff",
             "#eaf3ff","#c5daf6","#a1c2ed","#6996e3","#4060c8","#1a318b"},
            {9,5,11,1,7,3,13,4,8,2,12,6,10}, false}},
        {"Cassatt1", {
            {"#b1615c","#d88782","#e3aba7","#edd7d9","#c9c9dd","#9d9dc7","#8282aa","#5a5a83"},
            {3,6,1,8,4,5,2,7}, true}},
        {"Cassatt2", {
            {"#2d223c","#574571","#90719f","#b695bc","#dec5da","#c1d1aa","#7fa074","#466c4b","#2c4b27","#0e2810"},
            {7,3,9,1,5,6,2,10,4,8}, true}},
        {"Cross", {
            {"#c969a1","#ce4441","#ee8577","#eb7926","#ffbb44","#859b6c","#62929a","#004f63","#122451"},
            {4,7,1,8,2,6,3,5,9}, false}},
        {"Degas", {
            {"#591d06","#96410e","#e5a335","#556219","#418979","#2b614e","#053c29"},
            {5,2,1,3,4,7,6}, false}},
        {"Demuth", {
            {"#591c19","#9b332b","#b64f32","#d39a2d","#f7c267","#b9b9b8","#8b8b99","#5d6174","#41485f","#262d42"},
            {9,5,1,7,3,4,8,2,6,10}, true}},
        {"Derain", {
            {"#efc86e","#97c684","#6f9969","#aab5d5","#808fe1","#5c66a8","#454a74"},
            {4,2,5,7,1,3,6}, true}},
        {"Egypt", {
            {"#dd5129","#0f7ba2","#43b284","#fab255"},
            {1,2,3,4}, true}},
        {"Gauguin", {
            {"#b04948","#811e18","#9e4013","#c88a2c","#4c6216","#1a472a"},
            {2,5,4,3,1,6}, false}},
        {"Greek", {
            {"#3c0d03","#8d1c06","#e67424","#ed9b49","#f5c34d"},
            {2,3,5,1,4}, true}},
        {"Hiroshige", {
            {"#e76254","#ef8a47","#f7aa58","#ffd06f","#ffe6b7","#aadce0","#72bcd5","#528fad","#376795","#1e466e"},
            {6,2,9,3,7,5,1,10,4,8}, true}},
        {"Hokusai1", {
            {"#6d2f20","#b75347","#df7e66","#e09351","#edc775","#94b594","#224b5e"},
            {2,7,4,6,5,1,3}, false}},
        {"Hokusai2", {
            {"#abc9c8","#72aeb6","#4692b0","#2f70a1","#134b73","#0a3351"},
            {5,2,4,1,6,3}, true}},
        {"Hokusai3", {
            {"#d8d97a","#95c36e","#74c8c3","#5a97c1","#295384","#0a2e57"},
            {4,2,5,3,1,6}, true}},
        {"Homer1", {
            {"#551f00","#a62f00","#df7700","#f5b642","#fff179","#c3f4f6","#6ad5e8","#32b2da"},
            {6,3,2,7,4,8,5,1}, false}},
        {"Homer2", {
            {"#bf3626","#e9724c","#e9851d","#f9c53b","#aeac4c","#788f33","#165d43"},
            {3,7,1,4,6,2,5}, false}},
        {"Ingres", {
            {"#041d2c","#06314e","#18527e","#2e77ab","#d1b252","#a97f2f","#7e5522","#472c0b"},
            {4,5,3,6,2,7,1,8}, true}},
        {"Isfahan1", {
            {"#4e3910","#845d29","#d8c29d","#4fb6ca","#178f92","#175f5d","#1d1f54"},
            {5,2,4,6,1,7,3}, true}},
        {"Isfahan2", {
            {"#d7aca1","#ddc000","#79ad41","#34b6c6","#4063a3"},
            {4,2,3,5,1}, true}},
        {"Java", {
            {"#663171","#cf3a36","#ea7428","#e2998a","#0c7156"},
            {1,4,2,5,3}, true}},
        {"Johnson", {
            {"#a00e00","#d04e00","#f6c200","#0086a8","#132b69"},
            {3,1,4,2,5}, true}},
        {"Juarez", {
            {"#a82203","#208cc0","#f1af3a","#cf5e4e","#637b31","#003967"},
            {1,2,3,4,5,6}, false}},
        {"Kandinsky", {
            {"#3b7c70","#ce9642","#898e9f","#3b3a3e"},
            {1,2,3,4}, true}},
        {"Klimt", {
            {"#df9ed4","#c93f55","#eacc62","#469d76","#3c4b99","#924099"},
            {5,2,3,4,6,1}, false}},
        {"Lakota", {
            {"#04a3bd","#f0be3d","#931e18","#da7901","#247d3f","#20235b"},
            {1,2,3,4,5,6}, false}},
        {"Manet", {
            {"#3b2319","#80521c","#d29c44","#ebc174","#ede2cc","#7ec5f4","#4585b7","#225e92","#183571","#43429b","#5e65be"},
            {8,3,10,4,7,9,11,2,6,1,5}, false}},
        {"Monet", {
            {"#4e6d58","#749e89","#abccbe","#e3cacf","#c399a2","#9f6e71","#41507b","#7d87b2","#c2cae3"},
            {2,5,8,3,4,9,1,6,7}, false}},
        {"Moreau", {
            {"#421600","#792504","#bc7524","#8dadca","#527baa","#104839","#082844"},
            {2,5,3,4,7,1,6}, false}},
        {"Morgenstern", {
            {"#7c668c","#b08ba5","#dfbbc8","#ffc680","#ffb178","#db8872","#a56457"},
            {7,5,4,6,3,2,1}, true}},
        {"Nattier", {
            {"#52271c","#944839","#c08e39","#7f793c","#565c33","#184948","#022a2a"},
            {1,6,3,4,7,2,5}, false}},
        {"Navajo", {
            {"#660d20","#e59a52","#edce79","#094568","#e1c59a"},
            {1,2,3,4,5}, false}},
        {"NewKingdom", {
            {"#e1846c","#9eb4e0","#e6bb9e","#9c6849","#735852"},
            {2,1,3,4,5}, false}},
        {"Nizami", {
            {"#dd7867","#b83326","#c8570d","#edb144","#8cc8bc","#7da7ea","#5773c0","#1d4497"},
            {5,2,6,8,3,7,4,1}, false}},
        {"OKeeffe1", {
            {"#6b200c","#973d21","#da6c42","#ee956a","#fbc2a9","#f6f2ee","#bad6f9","#7db0ea","#447fdd","#225bb2","#133e7e"},
            {8,6,1,4,10,3,11,5,2,7,9}, true}},
        {"OKeeffe2", {
            {"#fbe3c2","#f2c88f","#ecb27d","#e69c6b","#d37750","#b9563f","#92351e"},
            {7,1,6,4,2,5,3}, true}},
        {"Paquin", {
            {"#831818","#c62320","#f05b43","#f78462","#feac81","#f7dea3","#ced1af","#98ab76","#748f46","#47632a","#275024"},
            {10,6,1,8,4,3,5,9,2,7,11}, false}},
        {"Peru1", {
            {"#b5361c","#e35e28","#1c9d7c","#31c7ba","#369cc9","#3a507f"},
            {3,1,5,2,4,6}, false}},
        {"Peru2", {
            {"#65150b","#961f1f","#c0431f","#b36c06","#f19425","#c59349","#533d14"},
            {4,1,3,5,2,7,6}, false}},
        {"Pillement", {
            {"#a9845b","#697852","#738e8e","#44636f","#2b4655","#0f252f"},
            {4,3,2,5,1,6}, true}},
        {"Pissaro", {
            {"#134130","#4c825d","#8cae9e","#8dc7dc","#508ca7","#1a5270","#0e2a4d"},
            {6,2,4,1,7,5,3}, false}},
        {"Redon", {
            {"#5b859e","#1e395f","#75884b","#1e5a46","#df8d71","#af4f2f","#d48f90","#732f30","#ab84a5","#59385c","#d8b847","#b38711"},
            {1,2,3,4,5,6,7,8,9,10,11,12}, false}},
        {"Renoir", {
            {"#17154f","#2f357c","#6c5d9e","#9d9cd5","#b0799a","#f6b3b0","#e48171","#bf3729","#e69b00","#f5bb50","#ada43b","#355828"},
            {2,5,9,12,3,8,7,10,4,1,6,11}, false}},
        {"Robert", {
            {"#11341a","#375624","#6ca4a0","#487a7c","#18505f","#062e3d"},
            {2,5,3,1,6,4}, false}},
        {"Signac", {
            {"#fbe183","#f4c40f","#fe9b00","#d8443c","#9b3441","#de597c","#e87b89","#e6a2a6","#aa7aa1","#9f5691","#633372","#1f6e9c","#2b9b81","#92c051"},
            {13,3,2,1,11,5,8,14,12,10,7,4,6,9}, false}},
        {"Stevens", {
            {"#042e4e","#307d7f","#598c4c","#ba5c3f","#a13213","#470c00"},
            {4,2,3,5,1,6}, false}},
        {"Tam", {
            {"#ffd353","#ffb242","#ef8737","#de4f33","#bb292c","#9f2d55","#62205f","#341648"},
            {3,8,1,6,2,7,4,5}, true}},
        {"Tara", {
            {"#eab1c6","#d35e17","#e18a1f","#e9b109","#829d44"},
            {1,3,2,5,4}, false}},
        {"Thomas", {
            {"#b24422","#c44d76","#4457a5","#13315f","#b1a1cc","#59386c","#447861","#7caf5c"},
            {3,2,8,6,1,4,7,5}, false}},
        {"Tiepolo", {
            {"#802417","#c06636","#ce9344","#e8b960","#646e3b","#2b5851","#508ea2","#17486f"},
            {1,2,8,4,3,5,7,6}, false}},
        {"Troy", {
            {"#421401","#6c1d0e","#8b3a2b","#c27668","#7ba0b4","#44728c","#235070","#0a2d46"},
            {2,7,4,5,1,8,3,6}, true}},
        {"Tsimshian", {
            {"#582310","#aa361d","#82c45f","#318f49","#0cb4bb","#2673a3","#473d7d"},
            {6,1,7,4,1,5,3}, false}},
        {"VanGogh1", {
            {"#2c2d54","#434475","#6b6ca3","#969bc7","#87bcbd","#89ab7c","#6f9954"},
            {3,5,7,4,6,2,1}, false}},
        {"VanGogh2", {
            {"#bd3106","#d9700e","#e9a00e","#eebe04","#5b7314","#c3d6ce","#89a6bb","#454b87"},
            {1,5,8,2,7,4,6,3}, false}},
        {"VanGogh3", {
            {"#e7e5cc","#c2d6a4","#9cc184","#669d62","#447243","#1f5b25","#1e3d14","#192813"},
            {7,5,1,4,8,2,3,6}, true}},
        {"Veronese", {
            {"#67322e","#99610a","#c38f16","#6e948c","#2c6b67","#175449","#122c43"},
            {5,1,7,2,3,6,4}, true}},
        {"Wissing", {
            {"#4b1d0d","#7c291e","#ba7233","#3a4421","#2d5380"},
            {2,3,5,4,1}, false}},
    };
    return P;
}

} // namespace MetBrewInternal

// Returns ROOT color indices for the named MetBrewer palette.
//   n < 0 (default): return all colors in the palette's canonical discrete order.
//   0 < n <= palette size: first n colors in discrete order.
//   n > palette size or continuous=true: linearly interpolate across all base colors.
inline std::vector<Int_t> MetColors(const std::string& name, int n = -1, bool continuous = false) {
    using namespace MetBrewInternal;
    const auto& P = palettes();
    auto it = P.find(name);
    if (it == P.end())
        throw std::invalid_argument("MetColors: unknown palette '" + name + "'");
    const PalDef& p = it->second;
    int nb = (int)p.hex.size();
    if (n <= 0) n = nb;
    std::vector<Int_t> out;
    out.reserve(n);
    if (continuous || n > nb) {
        for (int i = 0; i < n; ++i)
            out.push_back(interpColor(p.hex, n > 1 ? (float)i / (n - 1) : 0.f));
    } else {
        for (int i = 0; i < n; ++i)
            out.push_back(rootColor(p.hex[p.order[i] - 1]));
    }
    return out;
}

// Returns true if the named palette is colorblind-friendly.
inline bool MetIsColorblind(const std::string& name) {
    const auto& P = MetBrewInternal::palettes();
    auto it = P.find(name);
    return it != P.end() && it->second.colorblind;
}

// Returns a list of all available palette names.
inline std::vector<std::string> MetPaletteNames() {
    std::vector<std::string> names;
    for (const auto& kv : MetBrewInternal::palettes())
        names.push_back(kv.first);
    return names;
}

#endif // METCOLORS_H
