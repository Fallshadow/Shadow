#include "sdpch.h"
#include "Font.h"

#undef INFINITE
#include "msdf-atlas-gen.h"
#include "FontGeometry.h"
#include "GlyphGeometry.h"

#include "MSDFData.h"

namespace Shadow
{
    template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
    // 根据输入的字体和字形信息生成 MSDF 纹理贴图，并返回生成的纹理对象。通过模板函数的灵活性，可以适用于不同类型和生成函数的 MSDF 纹理生成。
    static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
        const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
    {
        // 用于存储生成器的属性设置
        msdf_atlas::GeneratorAttributes attributes;
        // 生成 MSDF 纹理时允许字形之间的重叠。在某些情况下，为了更好地处理字形之间的边缘，允许字形重叠可能是必要的。
        attributes.config.overlapSupport = true;
        // 生成 MSDF 纹理时采用扫描线渲染的方式。扫描线渲染是一种常用的技术，用于生成平滑的字形边缘信息，并可以提高 MSDF 纹理的质量。
        attributes.scanlinePass = true;

        // MSDF 字形贴图生成器对象
        msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(glyphs.data(), (int)glyphs.size());

        // 将生成器生成的字形贴图数据转换为位图常量引用类型，以便后续可以通过 bitmap 对象访问和处理生成的 MSDF 字形贴图数据。
        msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

        TextureSpecification spec;
        spec.Width = bitmap.width;
        spec.Height = bitmap.height;
        spec.Format = ImageFormat::RGB8;
        spec.GenerateMips = false;

        Ref<Texture2D> texture = Texture2D::Create(spec);
        texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
        return texture;
    }

    Font::Font(const std::filesystem::path& filepath) : m_Data(new MSDFData())
    {
        // 初始化了 FreeType 字体库的句柄 ft，用于加载字体文件。
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        SD_CORE_ASSERT(ft);

        std::string fileString = filepath.string();

        // TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need 
        // 加载指定路径的字体文件，将返回的字体句柄存储在 font 变量中。
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
        if (!font)
        {
            SD_CORE_ERROR("Failed to load font: {}", fileString);
            return;
        }

        // 定义了字符集范围 charsetRanges，然后通过循环将字符添加到一个名为 charset 的对象中。
        struct CharsetRange
        {
            uint32_t Begin, End;
        };

        // From imgui_draw.cpp
        static const CharsetRange charsetRanges[] =
        {
            { 0x0020, 0x00FF }
        };

        // 表示字符集的类，用于存储一组字符的数据
        msdf_atlas::Charset charset;
        for (CharsetRange range : charsetRanges)
        {
            for (uint32_t c = range.Begin; c <= range.End; c++)
                charset.add(c);
        }

        double fontScale = 1.0;
        // 创建字体几何信息
        m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
        // 加载字符集到字体几何信息
        int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
        SD_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

        // 打包字形数据，并设置像素范围、尖头限制、填充等参数。
        double emSize = 40.0;
        msdf_atlas::TightAtlasPacker atlasPacker;
        // atlasPacker.setDimensionsConstraint()
        atlasPacker.setPixelRange(2.0);
        atlasPacker.setMiterLimit(1.0);
        atlasPacker.setPadding(0);
        atlasPacker.setScale(emSize);
        int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
        SD_CORE_ASSERT(remaining == 0);

        int width, height;
        atlasPacker.getDimensions(width, height);
        emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
        // if MSDF || MTSDF

        // 判断是否需要对生成的 MSDF 字形进行额外的颜色处理。根据 expensiveColoring 变量值，决定采用不同的颜色处理方式。
        uint64_t coloringSeed = 0;
        bool expensiveColoring = false;
        if (expensiveColoring)
        {
            msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
                unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
                glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
                return true;
                }, m_Data->Glyphs.size()).finish(THREAD_COUNT);
        }
        else {
            unsigned long long glyphSeed = coloringSeed;
            for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
            {
                glyphSeed *= LCG_MULTIPLIER;
                glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
            }
        }

        m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);

#if 0
        // 这段代码加载字体中字符 'C' 的轮廓数据，进行归一化和边缘着色处理，然后生成 MSDF 字形数据，并最终将生成的 MSDF 数据保存为 PNG 图像文件。
        // 这些步骤是 MSDF 字体生成过程中的关键步骤，用于创建高质量的多通道有符号距离场字形。
        msdfgen::Shape shape;
        if (msdfgen::loadGlyph(shape, font, 'C'))
        {
            shape.normalize();
            //                      max. angle
            msdfgen::edgeColoringSimple(shape, 3.0);
            //           image width, height
            msdfgen::Bitmap<float, 3> msdf(32, 32);
            //                     range, scale, translation
            msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
            msdfgen::savePng(msdf, "output.png");
        }
#endif

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
    }

    Font::~Font()
    {
        delete m_Data;
    }

    Ref<Font> Font::GetDefault()
    {
        static Ref<Font> DefaultFont;
        if (!DefaultFont)
            DefaultFont = CreateRef<Font>("assets/fonts/opensans/OpenSans-Regular.ttf");

        return DefaultFont;
    }
}
