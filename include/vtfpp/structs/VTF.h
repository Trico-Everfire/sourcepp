#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <cassert>
#include "vtfpp/utils.h"

#define VTF_MAJOR_VERSION                       7
#define VTF_MINOR_VERSION                       6
#define VTF_MINOR_VERSION_DEFAULT               3

#define VTF_MINOR_VERSION_MIN_SPHERE_MAP        1
#define VTF_MINOR_VERSION_MIN_VOLUME            2
#define VTF_MINOR_VERSION_MIN_RESOURCE          3
#define VTF_MINOR_VERSION_MIN_NO_SPHERE_MAP     5

#define KERNEL_FILTER_BASE                      1040
#define HEIGHT_CONVERSION_METHOD_BASE           1009
#define NORMAL_ALPHA_RESULT_BASE                1033

#define MAKE_VTF_RSRC_ID(a, b, c) ((uInt)(((byte)a) | ((byte)b << 8) | ((byte)c << 16)))
#define MAKE_VTF_RSRC_IDF(a, b, c, d) ((uInt)(((byte)a) | ((byte)b << 8) | ((byte)c << 16) | ((byte)d << 24)))

namespace vtfpp {


    typedef unsigned int uInt;
    typedef std::byte byte;
    typedef unsigned short uShort;
    typedef u_int64_t uInt64;
    typedef int64_t int64;
    typedef u_int32_t uInt32;
    typedef int32_t int32;
    typedef u_int16_t uInt16;
    typedef int16_t int16;
    typedef u_int8_t uInt8;
    typedef int8_t int8;
    
    typedef void ( *transformProcess )( uInt16 &R, uInt16 &G, uInt16 &B, uInt16 &A );

    enum VTFImageFormat {
        IMAGE_FORMAT_RGBA8888 = 0,
        IMAGE_FORMAT_ABGR8888,
        IMAGE_FORMAT_RGB888,
        IMAGE_FORMAT_BGR888,
        IMAGE_FORMAT_RGB565,
        IMAGE_FORMAT_I8,
        IMAGE_FORMAT_IA88,
        IMAGE_FORMAT_P8,
        IMAGE_FORMAT_A8,
        IMAGE_FORMAT_RGB888_BLUESCREEN,
        IMAGE_FORMAT_BGR888_BLUESCREEN,
        IMAGE_FORMAT_ARGB8888,
        IMAGE_FORMAT_BGRA8888,
        IMAGE_FORMAT_DXT1,
        IMAGE_FORMAT_DXT3,
        IMAGE_FORMAT_DXT5,
        IMAGE_FORMAT_BGRX8888,
        IMAGE_FORMAT_BGR565,
        IMAGE_FORMAT_BGRX5551,
        IMAGE_FORMAT_BGRA4444,
        IMAGE_FORMAT_DXT1_ONEBITALPHA,
        IMAGE_FORMAT_BGRA5551,
        IMAGE_FORMAT_UV88,
        IMAGE_FORMAT_UVWQ8888,
        IMAGE_FORMAT_RGBA16161616F,
        IMAGE_FORMAT_RGBA16161616,
        IMAGE_FORMAT_UVLX8888,
        IMAGE_FORMAT_R32F,
        IMAGE_FORMAT_RGB323232F,
        IMAGE_FORMAT_RGBA32323232F,
        IMAGE_FORMAT_NV_NULL = 33,
        IMAGE_FORMAT_ATI2N,
        IMAGE_FORMAT_ATI1N,
        IMAGE_FORMAT_BC7 = 70,
        IMAGE_FORMAT_COUNT,
        IMAGE_FORMAT_NONE = -1
    };

    enum VTFImageFlag {
        TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
        TEXTUREFLAGS_TRILINEAR = 0x00000002,
        TEXTUREFLAGS_CLAMPS = 0x00000004,
        TEXTUREFLAGS_CLAMPT = 0x00000008,
        TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
        TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
        TEXTUREFLAGS_SRGB = 0x00000040,
        TEXTUREFLAGS_DEPRECATED_NOCOMPRESS = 0x00000040,
        TEXTUREFLAGS_NORMAL = 0x00000080,
        TEXTUREFLAGS_NOMIP = 0x00000100,
        TEXTUREFLAGS_NOLOD = 0x00000200,
        TEXTUREFLAGS_MINMIP = 0x00000400,
        TEXTUREFLAGS_PROCEDURAL = 0x00000800,
        TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
        TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
        TEXTUREFLAGS_ENVMAP = 0x00004000,
        TEXTUREFLAGS_RENDERTARGET = 0x00008000,
        TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
        TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
        TEXTUREFLAGS_SINGLECOPY = 0x00040000,
        TEXTUREFLAGS_UNUSED0 = 0x00080000,
        TEXTUREFLAGS_DEPRECATED_ONEOVERMIPLEVELINALPHA = 0x00080000,
        TEXTUREFLAGS_UNUSED1 = 0x00100000,
        TEXTUREFLAGS_DEPRECATED_PREMULTCOLORBYONEOVERMIPLEVEL = 0x00100000,
        TEXTUREFLAGS_UNUSED2 = 0x00200000,
        TEXTUREFLAGS_DEPRECATED_NORMALTODUDV = 0x00200000,
        TEXTUREFLAGS_UNUSED3 = 0x00400000,
        TEXTUREFLAGS_DEPRECATED_ALPHATESTMIPGENERATION = 0x00400000,
        TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
        TEXTUREFLAGS_UNUSED4 = 0x01000000,
        TEXTUREFLAGS_DEPRECATED_NICEFILTERED = 0x01000000,
        TEXTUREFLAGS_CLAMPU = 0x02000000,
        TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
        TEXTUREFLAGS_SSBUMP = 0x08000000,
        TEXTUREFLAGS_UNUSED5 = 0x10000000,
        TEXTUREFLAGS_DEPRECATED_UNFILTERABLE_OK = 0x10000000,
        TEXTUREFLAGS_BORDER = 0x20000000,
        TEXTUREFLAGS_DEPRECATED_SPECVAR_RED = 0x40000000,
        TEXTUREFLAGS_DEPRECATED_SPECVAR_ALPHA = 0x80000000,
        TEXTUREFLAGS_LAST = 0x20000000,
        TEXTUREFLAGS_COUNT = 30
    };

    enum VTFCubeMapFace {
        CUBEMAP_FACE_RIGHT = 0,
        CUBEMAP_FACE_LEFT,
        CUBEMAP_FACE_BACK,
        CUBEMAP_FACE_FRONT,
        CUBEMAP_FACE_UP,
        CUBEMAP_FACE_DOWN,
        CUBEMAP_FACE_SphereMap,
        CUBEMAP_FACE_COUNT
    };

    enum VTFMipmapFilter {
        MIPMAP_FILTER_POINT = 0,
        MIPMAP_FILTER_BOX,
        MIPMAP_FILTER_TRIANGLE,
        MIPMAP_FILTER_QUADRATIC,
        MIPMAP_FILTER_CUBIC,
        MIPMAP_FILTER_CATROM,
        MIPMAP_FILTER_MITCHELL,
        MIPMAP_FILTER_GAUSSIAN,
        MIPMAP_FILTER_SINC,
        MIPMAP_FILTER_BESSEL,
        MIPMAP_FILTER_HANNING,
        MIPMAP_FILTER_HAMMING,
        MIPMAP_FILTER_BLACKMAN,
        MIPMAP_FILTER_KAISER,
        MIPMAP_FILTER_COUNT
    };

    enum VTFKernelFilter {
        KERNEL_FILTER_4X = 0,
        KERNEL_FILTER_3X3,
        KERNEL_FILTER_5X5,
        KERNEL_FILTER_7X7,
        KERNEL_FILTER_9X9,
        KERNEL_FILTER_DUDV,
        KERNEL_FILTER_COUNT
    };

    enum VTFHeightConversionMethod {
        HEIGHT_CONVERSION_METHOD_ALPHA = 0,
        HEIGHT_CONVERSION_METHOD_AVERAGE_RGB,
        HEIGHT_CONVERSION_METHOD_BIASED_RGB,
        HEIGHT_CONVERSION_METHOD_RED,
        HEIGHT_CONVERSION_METHOD_GREEN,
        HEIGHT_CONVERSION_METHOD_BLUE,
        HEIGHT_CONVERSION_METHOD_MAX_RGB,
        HEIGHT_CONVERSION_METHOD_COLORSPACE,
        HEIGHT_CONVERSION_METHOD_COUNT
    };

    enum VTFNormalAlphaResult {
        NORMAL_ALPHA_RESULT_NOCHANGE = 0,
        NORMAL_ALPHA_RESULT_HEIGHT,
        NORMAL_ALPHA_RESULT_BLACK,
        NORMAL_ALPHA_RESULT_WHITE,
        NORMAL_ALPHA_RESULT_COUNT
    };

    enum VTFResizeMethod {
        RESIZE_NEAREST_POWER2 = 0,
        RESIZE_BIGGEST_POWER2,
        RESIZE_SMALLEST_POWER2,
        RESIZE_SET,
        RESIZE_COUNT
    };

    enum VTFLookDir {
        LOOK_DOWN_X = 0,
        LOOK_DOWN_NEGX,
        LOOK_DOWN_Y,
        LOOK_DOWN_NEGY,
        LOOK_DOWN_Z,
        LOOK_DOWN_NEGZ
    };

    enum VTFResourceEntryTypeFlag {
        RSRCF_HAS_NO_DATA_CHUNK = 0x02
    };

    enum VTFResourceEntryType {
        VTF_LEGACY_RSRC_LOW_RES_IMAGE = MAKE_VTF_RSRC_ID(0x01, 0, 0),
        VTF_LEGACY_RSRC_IMAGE = MAKE_VTF_RSRC_ID(0x30, 0, 0),
        VTF_RSRC_SHEET = MAKE_VTF_RSRC_ID(0x10, 0, 0),
        VTF_RSRC_CRC = MAKE_VTF_RSRC_IDF('C', 'R', 'C', RSRCF_HAS_NO_DATA_CHUNK),
        VTF_RSRC_TEXTURE_LOD_SETTINGS = MAKE_VTF_RSRC_IDF('L', 'O', 'D', RSRCF_HAS_NO_DATA_CHUNK),
        VTF_RSRC_TEXTURE_SETTINGS_EX = MAKE_VTF_RSRC_IDF('T', 'S', 'O', RSRCF_HAS_NO_DATA_CHUNK),
        VTF_RSRC_KEY_VALUE_DATA = MAKE_VTF_RSRC_ID('K', 'V', 'D'),
        VTF_RSRC_AUX_COMPRESSION_INFO = MAKE_VTF_RSRC_ID('A', 'X', 'C'),
        VTF_RSRC_MAX_DICTIONARY_ENTRIES = 32
    };

    struct VTFImageFormatInfo
    {
        std::string_view name;
        uInt bitsPerPixel;		  
        uInt bytesPerPixel;		  
        uInt redBitsPerPixel;		  
        uInt greenBitsPerPixel;		  
        uInt blueBitsPerPixel;		  
        uInt alphaBitsPerPixel;		  
        int indexRed;					  
        int indexGreen;					  
        int indexBlue;					  
        int indexAlpha;					  
        bool isCompressed;		  
        bool isSupported;		  
        transformProcess pToTransform;	  
        transformProcess pFromTransform; 
        VTFImageFormat imageFormat;
    };














    struct VTFInitOptions
    {
        uInt width;
        uInt height;

        uInt slices;
        uInt frames;
        uInt faces;

        VTFImageFormat ImageFormat;

        bool hasThumbnail;
        uInt mipMaps;

        bool hasNullImageData;
    };

#pragma pack(1)

    struct VTFFileHeader
    {
        char			identifier[4];
        uInt			version[2];
        uInt			headerSize;
    };

    struct VTFHeader_70 : public VTFFileHeader
    {
        uShort		width;
        uShort		height;
        uInt			flags;
        uShort		frames;
        uShort		startFrame;
        byte			padding1[4];
        float		reflectivity[3];
        byte			padding2[4];
        float		bumpScale;
        VTFImageFormat	imageFormat;
        byte			mipCount;
        VTFImageFormat	lowResImageFormat;
        byte			lowResImageWidth;
        byte			lowResImageHeight;
    };

    struct alignas(16) VTFHeader_70_Aligned: public VTFHeader_70 {};

    struct VTFHeader_71 : public VTFHeader_70 {};

    struct VTFHeader_72 : public VTFHeader_71
    {
        uShort		depth;
    };

    struct alignas(16) VTFHeader_72_Aligned: public VTFHeader_72 {};

    struct VTFHeader_73 : public VTFHeader_72
    {
        byte		padding[3];
        uInt		resourceCount;
    };

    struct alignas(16) VTFHeader_73_Aligned: public VTFHeader_73 {};

    struct VTFHeader_74 : public VTFHeader_73 {};

    struct alignas(16) VTFHeader_74_Aligned: public VTFHeader_74 {};

    struct VTFHeader_75 : public VTFHeader_74 {};

    struct alignas(16) VTFHeader_75_Aligned: public VTFHeader_75 {};

    struct VTFHeader_76 : public VTFHeader_75 {};

    struct alignas(16) VTFHeader_76_Aligned: public VTFHeader_76 {};
    
    struct VTFResource
    {
        union
        {
            uInt Type;
            struct
            {
                byte ID[3];	
                byte Flags;	
            };
        };
        uInt Data;	
    };

    struct VTFResourceData
    {
        uInt size;
        byte* data;
        [[nodiscard]] inline std::vector<byte> getData() const
        {
            auto vecData = std::vector<byte>{size};
            vecData.assign(data, data + size);
            return vecData;
        }
    };

    struct VTFTextureLODControlResource
    {
        byte resolutionClampU;
        byte resolutionClampV;
        byte padding[2];
    };

    struct VTFTextureSettingsResource
    {
        byte flags[4];
    };

    struct VTFAuxCompressionInfoHeader
    {
        static constexpr int32 DEFAULT_COMPRESSION = -1;

        int32 compressionLevel;
    };

    struct VTFAuxCompressionInfoEntry
    {
        uInt32 compressedSize;
    };

    struct VTFHeader : public VTFHeader_74_Aligned
    {
        byte			padding3[8];
        VTFResource		resources[VTF_RSRC_MAX_DICTIONARY_ENTRIES];
        VTFResourceData	data[VTF_RSRC_MAX_DICTIONARY_ENTRIES];
    };

    struct VTFCreateOptions
    {
        uInt version[2];		  
        VTFImageFormat imageFormat;	  

        uInt flags;				  
        uInt startFrame;		  
        float bumpScale;		  
        float reflectivity[3];	  

        bool mipmaps;			  
        VTFMipmapFilter mipmapFilter; 

        bool thumbnail;			  
        bool computeReflectivity;		  

        bool shouldResize;				  
        VTFResizeMethod resizeMethod; 
        VTFMipmapFilter resizeFilter; 
        uInt resizeWidth;		  
        uInt resizeHeight;		  

        bool clampResize;		  
        uInt resizeClampWidth;	  
        uInt resizeClampHeight;	  

        bool applyGammaCorrection;	  
        float gammaCorrectionAmount;	  

        bool generateSphereMap;			  
        bool isSRGB;				  
    };

#pragma pack()

    static constexpr VTFImageFormatInfo VTFImageFormatInfo[IMAGE_FORMAT_COUNT] =
            {
                    { "RGBA8888",           32, 4, 8, 8, 8, 8, 0, 1, 2, 3, false, true, nullptr, nullptr,               IMAGE_FORMAT_RGBA8888 },			   
                    { "ABGR8888",           32, 4, 8, 8, 8, 8, 3, 2, 1, 0, false, true, nullptr, nullptr,               IMAGE_FORMAT_ABGR8888 },
                    { "RGB888",             24, 3, 8, 8, 8, 0, 0, 1, 2, -1, false, true, nullptr, nullptr,              IMAGE_FORMAT_RGB888 },			   
                    { "BGR888",             24, 3, 8, 8, 8, 0, 2, 1, 0, -1, false, true, nullptr, nullptr,              IMAGE_FORMAT_BGR888 },			   
                    { "RGB565",             16, 2, 5, 6, 5, 0, 0, 1, 2, -1, false, true, nullptr, nullptr,              IMAGE_FORMAT_RGB565 },			   
                    { "I8",                 8, 1, 0, 0, 0, 0, 0, -1, -1, -1, false, true, toLuminance,  fromLuminance,  IMAGE_FORMAT_I8 },				   
                    { "IA88",               16, 2, 0, 0, 0, 8, 0, -1, -1, 1, false, true, toLuminance,  fromLuminance,  IMAGE_FORMAT_IA88 },				   
                    { "P8",                 8, 1, 0, 0, 0, 0, -1, -1, -1, -1, false, false, nullptr, nullptr,           IMAGE_FORMAT_P8 },				   
                    { "A8",                 8, 1, 0, 0, 0, 8, -1, -1, -1, 0, false, true, nullptr, nullptr,             IMAGE_FORMAT_A8 },				   
                    { "RGB888 Bluescreen",  24, 3, 8, 8, 8, 0, 0, 1, 2, -1, false, true,  ToBlueScreen, FromBlueScreen, IMAGE_FORMAT_RGB888_BLUESCREEN },   
                    { "BGR888 Bluescreen",  24, 3, 8, 8, 8, 0, 2, 1, 0, -1, false, true,  ToBlueScreen, FromBlueScreen, IMAGE_FORMAT_BGR888_BLUESCREEN },   
                    { "ARGB8888",           32, 4, 8, 8, 8, 8, 3, 0, 1, 2, false, true, nullptr, nullptr,                     IMAGE_FORMAT_ARGB8888 },			   
                    { "BGRA8888",           32, 4, 8, 8, 8, 8, 2, 1, 0, 3, false, true, nullptr, nullptr,                     IMAGE_FORMAT_BGRA8888 },			   
                    { "DXT1",               4, 0, 0, 0, 0, 0, -1, -1, -1, -1, true, true, nullptr, nullptr,                   IMAGE_FORMAT_DXT1 },				   
                    { "DXT3",               8, 0, 0, 0, 0, 8, -1, -1, -1, -1, true, true, nullptr, nullptr,                   IMAGE_FORMAT_DXT3 },				   
                    { "DXT5",               8, 0, 0, 0, 0, 8, -1, -1, -1, -1, true, true, nullptr, nullptr,                   IMAGE_FORMAT_DXT5 },				   
                    { "BGRX8888",           32, 4, 8, 8, 8, 0, 2, 1, 0, -1, false, true, nullptr, nullptr,                    IMAGE_FORMAT_BGRX8888 },			   
                    { "BGR565",             16, 2, 5, 6, 5, 0, 2, 1, 0, -1, false, true, nullptr, nullptr,                    IMAGE_FORMAT_BGR565 },			   
                    { "BGRX5551",           16, 2, 5, 5, 5, 0, 2, 1, 0, -1, false, true, nullptr, nullptr,                    IMAGE_FORMAT_BGRX5551 },			   
                    { "BGRA4444",           16, 2, 4, 4, 4, 4, 2, 1, 0, 3, false, true, nullptr, nullptr,                     IMAGE_FORMAT_BGRA4444 },			   
                    { "DXT1 One Bit Alpha", 4, 0, 0, 0, 0, 1, -1, -1, -1, -1, true, true, nullptr, nullptr,                   IMAGE_FORMAT_DXT1_ONEBITALPHA },	   
                    { "BGRA5551",           16, 2, 5, 5, 5, 1, 2, 1, 0, 3, false, true, nullptr, nullptr,                     IMAGE_FORMAT_BGRA5551 },			   
                    { "UV88",               16, 2, 8, 8, 0, 0, 0, 1, -1, -1, false, true, nullptr, nullptr,                   IMAGE_FORMAT_UV88 },				   
                    { "UVWQ8888",           32, 4, 8, 8, 8, 8, 0, 1, 2, 3, false, true, nullptr, nullptr,                     IMAGE_FORMAT_UVWQ8888 },			   
                    { "RGBA16161616F",      64, 8, 16, 16, 16, 16, 0, 1, 2, 3, false, true, nullptr, nullptr,                 IMAGE_FORMAT_RGBA16161616F },   
                    { "RGBA16161616",       64, 8, 16, 16, 16, 16, 0, 1, 2, 3, false, true, nullptr, nullptr,                 IMAGE_FORMAT_RGBA16161616 },	   
                    { "UVLX8888",           32, 4, 8, 8, 8, 8, 0, 1, 2, 3, false, true, nullptr, nullptr,                     IMAGE_FORMAT_UVLX8888 },			   
                    { "R32F",               32, 4, 32, 0, 0, 0, 0, -1, -1, -1, false, false, nullptr, nullptr,                IMAGE_FORMAT_R32F },			   
                    { "RGB323232F",         96, 12, 32, 32, 32, 0, 0, 0, 1, 2, -1, false, true, nullptr, nullptr,             IMAGE_FORMAT_RGB323232F },	   
                    { "RGBA32323232F",      128, 16, 32, 32, 32, 32, 0, 1, 2, 3, false, true, nullptr, nullptr,               IMAGE_FORMAT_RGBA32323232F }, 
                    {},
                    {},
                    {},
                    { "NULL",               32, 4, 0, 0, 0, 0, -1, -1, -1, -1, false, false, nullptr, nullptr, IMAGE_FORMAT_NV_NULL }, 
                    { "ATI2N",              8, 0, 0, 0, 0, 0, -1, -1, -1, -1, true, true, nullptr, nullptr, IMAGE_FORMAT_ATI2N },	 
                    { "ATI1N",              4, 0, 0, 0, 0, 0, -1, -1, -1, -1, true, true, nullptr, nullptr, IMAGE_FORMAT_ATI1N },	 
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    {},
                    { "BC7",                8, 0, 0, 0, 0, 0, -1, -1, -1, -1, true, true, nullptr, nullptr, IMAGE_FORMAT_BC7 } 
            };




}