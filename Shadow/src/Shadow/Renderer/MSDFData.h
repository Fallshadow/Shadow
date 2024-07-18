#pragma once

#include <vector>

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Shadow
{
    // Multi-channel Signed Distance Field（多通道有符号距离场）
    struct MSDFData
    {
        // 用于存储单个字形（Glyph）几何信息的类。
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry;
    };
}