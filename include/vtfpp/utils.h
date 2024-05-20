#include <cstring>
#include "structs/VTF.h"
#include "sourcepp/math/Floats.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#undef STBI_NO_FAILURE_STRINGS
#include "sourcepp/stb/stb_image.h"
#include "sourcepp/stb/stb_image_resize.h"
#include "Compressonator.h"

namespace vtfpp {

    float luminanceWeightRed = 0.299f;
    float luminanceWeightGreen = 0.587f;
    float luminanceWeightBlue = 0.114f;

    uShort blueScreenMaskRed = 0x0000;
    uShort blueScreenMaskGreen = 0x0000;
    uShort blueScreenMaskBlue = 0xffff;

    uShort blueScreenClearRed = 0x0000;
    uShort blueScreenClearGreen = 0x0000;
    uShort blueScreenClearBlue = 0x0000;
    
    const struct VTFImageFormatInfo& getimageFormatInfo(VTFImageFormat imageFormat) {
        assert(imageFormat >= 0 && imageFormat < IMAGE_FORMAT_COUNT);
        return VTFImageFormatInfo[imageFormat];
    }

    uInt computeImageSize(uInt width, uInt height, uInt depth, VTFImageFormat imageFormat) {
        switch (imageFormat) {
            case IMAGE_FORMAT_DXT1:
            case IMAGE_FORMAT_DXT1_ONEBITALPHA:
            case IMAGE_FORMAT_ATI1N:
                if (width < 4 && width > 0)
                    width = 4;

                if (height < 4 && height > 0)
                    height = 4;

                return ((width + 3) / 4) * ((height + 3) / 4) * 8 * depth;
            case IMAGE_FORMAT_DXT3:
            case IMAGE_FORMAT_DXT5:
            case IMAGE_FORMAT_ATI2N:
            case IMAGE_FORMAT_BC7:
                if (width < 4 && width > 0)
                    width = 4;

                if (height < 4 && height > 0)
                    height = 4;

                return ((width + 3) / 4) * ((height + 3) / 4) * 16 * depth;
            default:
                return width * height * depth * getimageFormatInfo(imageFormat).bytesPerPixel;
        }
    }

    uInt ComputeImageSize( uInt width, uInt height, uInt depth, uInt mipmaps, VTFImageFormat ImageFormat )
    {
        uInt imageSize = 0;

        assert( width != 0 && height != 0 && depth != 0 );

        for ( uInt i = 0; i < mipmaps; i++ )
        {
            imageSize += computeImageSize( width, height, depth, ImageFormat );

            width >>= 1;
            height >>= 1;
            depth >>= 1;

            if ( width < 1 )
                width = 1;

            if ( height < 1 )
                height = 1;

            if ( depth < 1 )
                depth = 1;
        }

        return imageSize;
    }

    uInt computeMipMapCount( uInt width, uInt height, uInt depth )
    {
        uInt count = 0;

        assert( width != 0 && height != 0 && depth != 0 );

        while ( true )
        {
            count++;

            width >>= 1;
            height >>= 1;
            depth >>= 1;

            if ( width == 0 && height == 0 && depth == 0 )
                break;
        }

        return count;
    }

    void computeMipMapDimensions( uInt width, uInt height, uInt depth, uInt mipmapLevel, uInt &mipmapWidth, uInt &mipmapHeight, uInt &mipmapDepth )
    {
        mipmapWidth = width >> mipmapLevel;
        mipmapHeight = height >> mipmapLevel;
        mipmapDepth = depth >> mipmapLevel;
        
        if ( mipmapWidth < 1 )
            mipmapWidth = 1;

        if ( mipmapHeight < 1 )
            mipmapHeight = 1;

        if ( mipmapDepth < 1 )
            mipmapDepth = 1;
    }

    uInt computeMipMapSize( uInt width, uInt height, uInt depth, uInt mipmapLevel, VTFImageFormat imageFormat )
    {
        uInt mipmapWidth, mipmapHeight, mipmapDepth;
        computeMipMapDimensions( width, height, depth, mipmapLevel, mipmapWidth, mipmapHeight, mipmapDepth );

        return computeImageSize( mipmapWidth, mipmapHeight, mipmapDepth, imageFormat );
    }

    stbir_filter mipmapFilterToSTBFilter(VTFMipmapFilter filter)
    {
        switch (filter) {

            case MIPMAP_FILTER_POINT:
                return STBIR_FILTER_DEFAULT;
            case MIPMAP_FILTER_BOX:
                return STBIR_FILTER_BOX;
            case MIPMAP_FILTER_TRIANGLE:
                return STBIR_FILTER_TRIANGLE;
            case MIPMAP_FILTER_QUADRATIC:
                return STBIR_FILTER_DEFAULT;
            case MIPMAP_FILTER_CUBIC:
                return STBIR_FILTER_CUBICBSPLINE;
            case MIPMAP_FILTER_CATROM:
                return STBIR_FILTER_DEFAULT;
            case MIPMAP_FILTER_MITCHELL:
                return STBIR_FILTER_MITCHELL;
            case MIPMAP_FILTER_GAUSSIAN:
            case MIPMAP_FILTER_SINC:
            case MIPMAP_FILTER_BESSEL:
            case MIPMAP_FILTER_HANNING:
            case MIPMAP_FILTER_HAMMING:
            case MIPMAP_FILTER_BLACKMAN:
            case MIPMAP_FILTER_KAISER:
            case MIPMAP_FILTER_COUNT:
                return STBIR_FILTER_DEFAULT;
        }
    }

    // TODO: NOTICE: Look into upgrading to stb_image_resize2, it's faster, but changed it's API at on, have no idea how it works.
    bool resize( std::vector<byte> &sourceDataRGBA32,std::vector<byte> &destinationDataRGBA32, uInt sourceWidth, uInt sourceHeight, uInt destinationWidth, uInt destinationHeight, VTFMipmapFilter resizeFilter, bool isSRGB )
    {
        assert( resizeFilter >= 0 && resizeFilter < MIPMAP_FILTER_COUNT );

        if ( !stbir_resize_uint8_generic(
                reinterpret_cast<const unsigned char *>(sourceDataRGBA32.data()), sourceWidth, sourceHeight, 0,
                reinterpret_cast<unsigned char *>(destinationDataRGBA32.data()), destinationWidth, destinationHeight, 0,
                4, 3, 0, STBIR_EDGE_CLAMP, mipmapFilterToSTBFilter(resizeFilter), isSRGB ? STBIR_COLORSPACE_SRGB : STBIR_COLORSPACE_LINEAR, nullptr ) )
            return false;

        return true;
    }

    bool resizeFloat( std::vector<byte> &lpSourceRGBAFP32, std::vector<byte> &lpDestRGBFP32, uInt sourceWidth, uInt sourceHeight, uInt destinationWidth, uInt destinationHeight, VTFMipmapFilter ResizeFilter, bool isSRGB )
    {
        assert( ResizeFilter >= 0 && ResizeFilter < MIPMAP_FILTER_COUNT );

        if ( !stbir_resize_float_generic(
                reinterpret_cast<float *>( lpSourceRGBAFP32.data() ), sourceWidth, sourceHeight, 0,
                reinterpret_cast<float *>( lpDestRGBFP32.data() ), destinationWidth, destinationHeight, 0,
                4, 3, 0, STBIR_EDGE_CLAMP, mipmapFilterToSTBFilter(ResizeFilter), isSRGB ? STBIR_COLORSPACE_SRGB : STBIR_COLORSPACE_LINEAR, nullptr ) )
            return false;

        return true;
    }

    // TODO: One day write FP gamma correction.
    void correctImageGamma( std::vector<byte> &lpImageDataRGBA8888, float gammaCorrection )
    {
        if ( gammaCorrection == 1.0f )
        {
            return;
        }

        byte valueTable[256];

        gammaCorrection = 1.0f / gammaCorrection;
        
        for ( uInt i = 0; i < 256; i++ )
        {
            valueTable[i] = (byte)( pow( (float)i / 255.0f, gammaCorrection ) * 255.0f );
        }
        
        for ( auto itr = lpImageDataRGBA8888.begin(); itr != lpImageDataRGBA8888.end(); itr += 4 )
        {
            *(itr) = valueTable[static_cast<int>(itr[0])];
            *(itr + 1) = valueTable[static_cast<int>(itr[1])];
            *(itr + 2) = valueTable[static_cast<int>(itr[2])];
        }
    }

    void computeImageReflectivity(std::vector<byte> &lpImageDataRGBA8888, uInt width, uInt height, float &x, float &y, float &z )
    {
        x = y = z = 0.0f;

        float valueTable[256];

        for ( uInt i = 0; i < 256; i++ )
        {
            valueTable[i] = pow( (float)i / 255.0f, 2.2f );
        }

        float tempX, tempY, tempZ, inverse;

        for ( uInt j = 0; j < height; j++ )
        {
            tempX = tempY = tempZ = 0.0f;

            for ( uInt i = 0; i < width; i++ )
            {
                uInt index = ( i + j * width ) * 4;

                tempX += valueTable[static_cast<int>(lpImageDataRGBA8888[index])];
                tempY += valueTable[static_cast<int>(lpImageDataRGBA8888[index + 1])];
                tempZ += valueTable[static_cast<int>(lpImageDataRGBA8888[index + 2])];
            }

            inverse = 1.0f / (float)width;

            x += tempX * inverse;
            y += tempY * inverse;
            z += tempZ * inverse;
        }

        inverse = 1.0f / (float)height;

        x *= inverse;
        y *= inverse;
        z *= inverse;
    }

    void flipImage( std::vector<byte> &lpImageDataRGBA8888, uInt width, uInt height )
    {
        uInt *lpImageData = (uInt *)lpImageDataRGBA8888.data();

        for ( uInt i = 0; i < width; i++ )
        {
            for ( uInt j = 0; j < height / 2; j++ )
            {
                uInt *pOne = lpImageData + ( i + j * width );
                uInt *pTwo = lpImageData + ( i + ( height - j - 1 ) * width );

                uInt temp = *pOne;
                *pOne = *pTwo;
                *pTwo = temp;
            }
        }
    }
    void MirrorImage( std::vector<byte> &lpImageDataRGBA8888, uInt width, uInt height )
    {
        uInt *lpImageData = (uInt *)lpImageDataRGBA8888.data();

        for ( uInt i = 0; i < width / 2; i++ )
        {
            for ( uInt j = 0; j < height; j++ )
            {
                uInt *pOne = lpImageData + ( i + j * width );
                uInt *pTwo = lpImageData + ( ( width - i - 1 ) + j * width );

                uInt temp = *pOne;
                *pOne = *pTwo;
                *pTwo = temp;
            }
        }
    }

    inline float FP16ToFP32( uInt16 input )
    {
        static const uInt32 fp32Bias = 127;
        static const uInt32 fp16Bias = 15;
        static const float maxFP16Bits = 65504.0f;

        FP16 fp16{0};

        std::memcpy( &fp16, &input, sizeof( uInt16 ) );

        if (fp16.exponent == 31 )
        {
            if (fp16.mantissa == 0 )
                return maxFP16Bits * ((fp16.sign == 1 ) ? -1.0f : 1.0f );
            else if (fp16.mantissa != 0 )
                return 0.0f;
        }

        if (fp16.exponent == 0 && fp16.mantissa != 0 )
        {
            const float halfDenorm = 1.0f / float(1 << 14 );
            const float mantissa = float(fp16.mantissa ) / float(1 << 10 );
            const float sign = fp16.sign ? -1.0f : 1.0f;

            return sign * mantissa * halfDenorm;
        }
        else
        {
            const uInt32 mantissa = fp16.mantissa;
            const uInt32 exponent = fp16.exponent != 0 ? fp16.exponent - fp16Bias + fp32Bias : 0;
            const uInt32 sign = fp16.sign;

            uInt32 bits = ( mantissa << 13 ) | ( exponent << 23 ) | ( sign << 31 );
            float value;
            std::memcpy(&value, &bits, sizeof( value ) );
            return value;
        }
    }

    //TODO: improve this.
    /* https://stackoverflow.com/a/76815873 */
    
#define Bit(N)  ((uInt32) 1 << (N))
#define Mask(N) (((uInt32) 1 << (N)) - 1)
    inline uShort FP32ToFP16( float input )
    {
        uInt32 x = static_cast<uInt32>(input);
        uInt32 SignBit          = x >> 31;
        uInt32 ExponentField    = x >> 23 & Mask( 8);
        uInt32 SignificandField = x       & Mask(23);

        if (ExponentField == Mask(8))
        {
            if (SignificandField == 0)
                return SignBit << 15 | Mask(5) << 10;
            else
            {
                SignificandField >>= 23-10;

                if (SignificandField == 0)
                    SignificandField = 1;

                return SignBit << 15 | Mask(5) << 10 | SignificandField;
            }
        }
        else if (ExponentField == 0)
            return SignBit << 15;
        else
        {
            int Exponent = (int) ExponentField - 127 + 15;

            if (Exponent < -11)
                return SignBit << 15;

            uInt32 Significand = Bit(23) | SignificandField;

            if (Exponent < 1)
            {
                uInt32 T = Significand << (32 - (1 - Exponent) - 13);
                Significand >>= 1 - Exponent + 13;
                Exponent = 1;

                if (Bit(31) < T)
                    ++Significand;

                if (Bit(31) == T)
                    Significand += Significand & 1;

                if (Bit(10) <= Significand)
                    return SignBit << 15 | 1 << 10 | 0;

                return SignBit << 15 | 0 << 10 | (Significand & Mask(10));
            }

            uInt32 T = Significand & Mask(13);
            if (Bit(12) < T || Bit(12) == T && (Significand & Bit(13)))
                Significand += Bit(13);
            Significand >>= 13;
            if (Bit(11) <= Significand)
            {
                ++Exponent;
                Significand >>= 1;
            }

            if (31 <= Exponent)
                return SignBit << 15 | Mask(5) << 10;

            return SignBit << 15 | Exponent << 10 | (Significand & Mask(10));
        }
    }
#undef Bit
#undef Mask

    bool convertToRGBA8888( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat SourceFormat );

    bool convertFromRGBA8888( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat DestFormat );

    bool convert( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat SourceFormat, VTFImageFormat DestFormat );

    bool HALF_HDR_TO_LDR( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, const struct VTFImageFormatInfo &sourceFormatInfo, const struct VTFImageFormatInfo &destinationFormatInfo )
    {
        int count = 0;
        count += sourceFormatInfo.redBitsPerPixel > 0;
        count += sourceFormatInfo.greenBitsPerPixel > 0;
        count += sourceFormatInfo.blueBitsPerPixel > 0;
        count += sourceFormatInfo.alphaBitsPerPixel > 0;

        uInt srcSize = computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA16161616F );

        uInt midSize = computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA32323232F );
        
        float *tmp = static_cast<float *>( malloc( midSize ) );

        unsigned short *fp16Data = reinterpret_cast<unsigned short *>( sourceData.data() );

        float *tmpStart = tmp;

        for ( int i = 0; i < ( srcSize / sizeof( unsigned short ) ); i++ )
        {
            *tmp = FP16ToFP32( fp16Data[i] );
            tmp++;
        }

        auto data = stbi__hdr_to_ldr( tmpStart, width, height, 4 );

        if ( !data )
            return false;

        uInt dataSize = computeImageSize(width,height,1, count > 3 ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888);

        std::vector<byte> vecData{};
        vecData.resize(dataSize);
        memcpy(vecData.data(), data, dataSize);

        convert( vecData, destinationData, width, height, count > 3 ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888, destinationFormatInfo.imageFormat );

        stbi_image_free( data );
        return true;
    }

    bool HDR_TO_LDR( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, const struct VTFImageFormatInfo &sourceFormatInfo, const struct VTFImageFormatInfo &destinationFormatInfo )
    {
        int count = 0;
        count += sourceFormatInfo.redBitsPerPixel > 0;
        count += sourceFormatInfo.greenBitsPerPixel > 0;
        count += sourceFormatInfo.blueBitsPerPixel > 0;
        count += sourceFormatInfo.alphaBitsPerPixel > 0;

        uInt srcSize = computeImageSize( width, height, 1, sourceFormatInfo.imageFormat );

        // due to the nature of stbi__hdr_to_ldr
        // tmp is freed inside the function.
        // so no need to free it ourselves.
        float *tmp = static_cast<float *>( malloc( srcSize ) );

        memcpy( tmp, sourceData.data(), srcSize );

        auto data = stbi__hdr_to_ldr( reinterpret_cast<float *>( tmp ), width, height, count );

        if ( !data )
            return false;

        uInt dataSize = computeImageSize(width,height,1, count > 3 ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888);

        std::vector<byte> vecData{};
        vecData.resize(dataSize);
        memcpy(vecData.data(), data, dataSize);

        convert( vecData, destinationData, width, height, count > 3 ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_RGB888, destinationFormatInfo.imageFormat );

        stbi_image_free( data );
        return true;
    }

    bool LDR_TO_HDR( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, const struct VTFImageFormatInfo &sourceFormatInfo, const struct VTFImageFormatInfo &destinationFormatInfo )
    {
        int count = 0;
        count += sourceFormatInfo.redBitsPerPixel > 0;
        count += sourceFormatInfo.greenBitsPerPixel > 0;
        count += sourceFormatInfo.blueBitsPerPixel > 0;
        count += sourceFormatInfo.alphaBitsPerPixel > 0;

        uInt srcSize = computeImageSize( width, height, 1, sourceFormatInfo.imageFormat );

        // due to the nature of stbi__ldr_to_hdr
        // tmp is freed inside the function.
        // so no need to free it ourselves.
        byte *tmp = static_cast<byte *>( malloc( srcSize ) );

        memcpy( tmp, sourceData.data(), srcSize );

        auto data = stbi__ldr_to_hdr( reinterpret_cast<stbi_uc *>( tmp ), width, height, count );

        if ( !data )
            return false;

        uInt destSize = computeImageSize( width, height, 1, destinationFormatInfo.imageFormat );
        destinationData.resize(destSize);
        memcpy( destinationData.data(), data, destSize );

        stbi_image_free( data );
        return true;
    }

    CMP_FORMAT getCMPFormatConversion(VTFImageFormat imageFormat, bool isDXT5GA )
    {
        if ( isDXT5GA )
            return CMP_FORMAT_ATI2N_DXT5;

        switch ( imageFormat )
        {
            case IMAGE_FORMAT_BGR888:
                return CMP_FORMAT_BGR_888;
            case IMAGE_FORMAT_RGB888:
                return CMP_FORMAT_RGB_888;
            case IMAGE_FORMAT_RGBA8888:
                return CMP_FORMAT_RGBA_8888;
            case IMAGE_FORMAT_BGRA8888:
                return CMP_FORMAT_BGRA_8888;

            case IMAGE_FORMAT_DXT1_ONEBITALPHA:
            case IMAGE_FORMAT_DXT1:
                return CMP_FORMAT_DXT1;
            case IMAGE_FORMAT_DXT3:
                return CMP_FORMAT_DXT3;
            case IMAGE_FORMAT_DXT5:
                return CMP_FORMAT_DXT5;
            case IMAGE_FORMAT_ATI1N:
                return CMP_FORMAT_ATI1N;
            case IMAGE_FORMAT_ATI2N:
                return CMP_FORMAT_ATI2N;

            case IMAGE_FORMAT_BC7:
                return CMP_FORMAT_BC7;

            default:
                return CMP_FORMAT_Unknown;
        }
    }

    CMP_ERROR decompressBCn( std::vector<byte> &src, std::vector<byte> &dst, uInt width, uInt height, VTFImageFormat SourceFormat )
    {
        CMP_Texture srcTexture = { 0 };
        srcTexture.dwSize = sizeof( srcTexture );
        srcTexture.dwWidth = width;
        srcTexture.dwHeight = height;
        srcTexture.dwPitch = 0;
        srcTexture.format = getCMPFormatConversion(SourceFormat, false);
        srcTexture.dwDataSize = CMP_CalculateBufferSize( &srcTexture );
        srcTexture.pData = (CMP_BYTE *)src.data();

        CMP_CompressOptions options = { 0 };
        options.dwSize = sizeof( options );
        options.dwnumThreads = 0;
        options.bDXT1UseAlpha = false;

        CMP_Texture destTexture = { 0 };
        destTexture.dwSize = sizeof( destTexture );
        destTexture.dwWidth = width;
        destTexture.dwHeight = height;
        destTexture.dwPitch = 4 * width;
        destTexture.format = CMP_FORMAT_RGBA_8888;
        destTexture.dwDataSize = destTexture.dwPitch * height;
        dst.resize(destTexture.dwPitch * height);
        destTexture.pData = (CMP_BYTE *)dst.data();

        CMP_ERROR cmp_status = CMP_ConvertTexture( &srcTexture, &destTexture, &options, nullptr );

        return cmp_status;
    }

    CMP_ERROR compressBCn( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat DestFormat )
    {
        CMP_Texture srcTexture = { 0 };
        srcTexture.dwSize = sizeof( srcTexture );
        srcTexture.dwWidth = width;
        srcTexture.dwHeight = height;
        srcTexture.dwPitch = 4 * width;
        srcTexture.format = CMP_FORMAT_RGBA_8888;
        srcTexture.dwDataSize = height * srcTexture.dwPitch;
        srcTexture.pData = (CMP_BYTE *)sourceData.data();

        CMP_CompressOptions options = { 0 };
        options.dwSize = sizeof( options );
        options.dwnumThreads = 0;
        options.bDXT1UseAlpha = DestFormat == IMAGE_FORMAT_DXT1_ONEBITALPHA;

        CMP_Texture destTexture = { 0 };
        destTexture.dwSize = sizeof( destTexture );
        destTexture.dwWidth = width;
        destTexture.dwHeight = height;
        destTexture.dwPitch = 0;
        destTexture.format = getCMPFormatConversion(DestFormat, false);
        destTexture.dwDataSize = CMP_CalculateBufferSize( &destTexture );
        destinationData.resize(destTexture.dwDataSize);
        destTexture.pData = (CMP_BYTE *)destinationData.data();

        CMP_ERROR err = CMP_ConvertTexture( &srcTexture, &destTexture, &options, nullptr );

        return err;
    }

    template <typename T>
    void GetShiftAndMask( const struct VTFImageFormatInfo &Info, T &redShift, T &greenShift, T &blueShift, T &alphaShift, T &redMask, T &greenMask, T &blueMask, T &alphaMask )
    {
        
        if ( Info.indexRed >= 0 )
        {
            if ( Info.indexGreen >= 0 && Info.indexGreen < Info.indexRed )
                redShift += (T)Info.greenBitsPerPixel;

            if ( Info.indexBlue >= 0 && Info.indexBlue < Info.indexRed )
                redShift += (T)Info.blueBitsPerPixel;

            if ( Info.indexAlpha >= 0 && Info.indexAlpha < Info.indexRed )
                redShift += (T)Info.alphaBitsPerPixel;

            redMask = (T)( ~0 ) >> (T)( ( sizeof( T ) * 8 ) - Info.redBitsPerPixel ); // Mask is for down shifted values.
        }

        if ( Info.indexGreen >= 0 )
        {
            if ( Info.indexRed >= 0 && Info.indexRed < Info.indexGreen )
                greenShift += (T)Info.redBitsPerPixel;

            if ( Info.indexBlue >= 0 && Info.indexBlue < Info.indexGreen )
                greenShift += (T)Info.blueBitsPerPixel;

            if ( Info.indexAlpha >= 0 && Info.indexAlpha < Info.indexGreen )
                greenShift += (T)Info.alphaBitsPerPixel;

            greenMask = (T)( ~0 ) >> (T)( ( sizeof( T ) * 8 ) - Info.greenBitsPerPixel );
        }

        if ( Info.indexBlue >= 0 )
        {
            if ( Info.indexRed >= 0 && Info.indexRed < Info.indexBlue )
                blueShift += (T)Info.redBitsPerPixel;

            if ( Info.indexGreen >= 0 && Info.indexGreen < Info.indexBlue )
                blueShift += (T)Info.greenBitsPerPixel;

            if ( Info.indexAlpha >= 0 && Info.indexAlpha < Info.indexBlue )
                blueShift += (T)Info.alphaBitsPerPixel;

            blueMask = (T)( ~0 ) >> (T)( ( sizeof( T ) * 8 ) - Info.blueBitsPerPixel );
        }

        if ( Info.indexAlpha >= 0 )
        {
            if ( Info.indexRed >= 0 && Info.indexRed < Info.indexAlpha )
                alphaShift += (T)Info.redBitsPerPixel;

            if ( Info.indexGreen >= 0 && Info.indexGreen < Info.indexAlpha )
                alphaShift += (T)Info.greenBitsPerPixel;

            if ( Info.indexBlue >= 0 && Info.indexBlue < Info.indexAlpha )
                alphaShift += (T)Info.blueBitsPerPixel;

            alphaMask = (T)( ~0 ) >> (T)( ( sizeof( T ) * 8 ) - Info.alphaBitsPerPixel );
        }
    }
    
    template <typename T>
    T shrink( T S, T sourceBits, T destinationBits )
    {
        if ( sourceBits == 0 || destinationBits == 0 )
            return 0;

        return S >> ( sourceBits - destinationBits );
    }
    
    template <typename T>
    T expand( T S, T sourceBits, T destinationBits )
    {
        if ( sourceBits == 0 || destinationBits == 0 )
            return 0;

        T D = 0;
        
        while ( destinationBits >= sourceBits )
        {
            D <<= sourceBits;
            D |= S;
            destinationBits -= sourceBits;
        }
        
        if ( destinationBits )
        {
            S >>= sourceBits - destinationBits;
            D <<= destinationBits;
            D |= S;
        }

        return D;
    }

    template <typename T, typename U>
    void Transform( transformProcess pTransform1, transformProcess pTransform2, T SR, T SG, T SB, T SA, T SRBits, T SGBits, T SBBits, T SABits, U &DR, U &DG, U &DB, U &DA, U DRBits, U DGBits, U DBBits, U DABits )
    {
        uInt16 TR, TG, TB, TA;

        // expand from source to 16 bits for transform functions.
        SRBits &&SRBits < 16 ? TR = (uInt16)expand<T>( SR, SRBits, 16 ) : TR = (uInt16)SR;
        SGBits &&SGBits < 16 ? TG = (uInt16)expand<T>( SG, SGBits, 16 ) : TG = (uInt16)SG;
        SBBits &&SBBits < 16 ? TB = (uInt16)expand<T>( SB, SBBits, 16 ) : TB = (uInt16)SB;
        SABits &&SABits < 16 ? TA = (uInt16)expand<T>( SA, SABits, 16 ) : TA = (uInt16)SA;

        // Source transform then dest transform.
        if ( pTransform1 )
            pTransform1( TR, TG, TB, TA );
        if ( pTransform2 )
            pTransform2( TR, TG, TB, TA );

        // shrink to dest from 16 bits.
        DRBits &&DRBits < 16 ? DR = (U)shrink<uInt16>( TR, 16, (uInt16)DRBits ) : DR = (U)TR;
        DGBits &&DGBits < 16 ? DG = (U)shrink<uInt16>( TG, 16, (uInt16)DGBits ) : DG = (U)TG;
        DBBits &&DBBits < 16 ? DB = (U)shrink<uInt16>( TB, 16, (uInt16)DBBits ) : DB = (U)TB;
        DABits &&DABits < 16 ? DA = (U)shrink<uInt16>( TA, 16, (uInt16)DABits ) : DA = (U)TA;
    }

    template <typename T, typename U>
    bool convert(std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, const struct VTFImageFormatInfo &sourceFormatInfo, const struct VTFImageFormatInfo &destinationFormatInfo)
    {


        uInt16 sourceRShift = 0, sourceGShift = 0, sourceBShift = 0, sourceAShift = 0;
        uInt16 sourceRMask = 0, sourceGMask = 0, sourceBMask = 0, sourceAMask = 0;

        uInt16 destinationRShift = 0, destinationGShift = 0, destinationBShift = 0, destinationAShift = 0;
        uInt16 destinationRMask = 0, destinationGMask = 0, destinationBMask = 0, destinationAMask = 0;

        GetShiftAndMask<uInt16>( sourceFormatInfo, sourceRShift, sourceGShift, sourceBShift, sourceAShift, sourceRMask, sourceGMask, sourceBMask, sourceAMask );
        GetShiftAndMask<uInt16>( destinationFormatInfo, destinationRShift, destinationGShift, destinationBShift, destinationAShift, destinationRMask, destinationGMask, destinationBMask, destinationAMask );

        for(auto sourceItr = sourceData.begin(), destinationItr = destinationData.begin(); sourceItr != sourceData.end(); sourceItr+=sourceFormatInfo.bytesPerPixel, destinationItr += destinationFormatInfo.bytesPerPixel)
        {
            // read source into single variable
            uInt i;
            T Source = 0;
            for ( i = 0; i < sourceFormatInfo.bytesPerPixel; i++ )
            {
                Source |= (T)sourceData[i] << ( (T)i * 8 );
            }

            uInt16 SR = 0, SG = 0, SB = 0, SA = ~0;
            uInt16 DR = 0, DG = 0, DB = 0, DA = ~0; // default values

            // read source values
            if ( sourceRMask )
                SR = (uInt16)( Source >> (T)sourceRShift ) & sourceRMask; // isolate R channel

            if ( sourceGMask )
                SG = (uInt16)( Source >> (T)sourceGShift ) & sourceGMask; // isolate G channel

            if ( sourceBMask )
                SB = (uInt16)( Source >> (T)sourceBShift ) & sourceBMask; // isolate B channel

            if ( sourceAMask )
                SA = (uInt16)( Source >> (T)sourceAShift ) & sourceAMask; // isolate A channel

            if ( sourceFormatInfo.pFromTransform || destinationFormatInfo.pToTransform )
            {
                // transform values
                Transform<uInt16, uInt16>( sourceFormatInfo.pFromTransform, destinationFormatInfo.pToTransform, SR, SG, SB, SA, sourceFormatInfo.redBitsPerPixel, sourceFormatInfo.greenBitsPerPixel, sourceFormatInfo.blueBitsPerPixel, sourceFormatInfo.alphaBitsPerPixel, DR, DG, DB, DA, destinationFormatInfo.redBitsPerPixel, destinationFormatInfo.greenBitsPerPixel, destinationFormatInfo.blueBitsPerPixel, destinationFormatInfo.alphaBitsPerPixel );
            }
            else
            {
                // default value transform
                if ( sourceRMask && destinationRMask )
                {
                    if ( destinationFormatInfo.redBitsPerPixel < sourceFormatInfo.redBitsPerPixel )
                        DR = shrink<uInt16>( SR, sourceFormatInfo.redBitsPerPixel, destinationFormatInfo.redBitsPerPixel );
                    else if ( destinationFormatInfo.redBitsPerPixel > sourceFormatInfo.redBitsPerPixel )
                        DR = expand<uInt16>( SR, sourceFormatInfo.redBitsPerPixel, destinationFormatInfo.redBitsPerPixel );
                    else
                        DR = SR;
                }

                if ( sourceGMask && destinationGMask )
                {
                    if ( destinationFormatInfo.greenBitsPerPixel < sourceFormatInfo.greenBitsPerPixel )	  // downsample
                        DG = shrink<uInt16>( SG, sourceFormatInfo.greenBitsPerPixel, destinationFormatInfo.greenBitsPerPixel );
                    else if ( destinationFormatInfo.greenBitsPerPixel > sourceFormatInfo.greenBitsPerPixel ) // upsample
                        DG = expand<uInt16>( SG, sourceFormatInfo.greenBitsPerPixel, destinationFormatInfo.greenBitsPerPixel );
                    else
                        DG = SG;
                }

                if ( sourceBMask && destinationBMask )
                {
                    if ( destinationFormatInfo.blueBitsPerPixel < sourceFormatInfo.blueBitsPerPixel )	  // downsample
                        DB = shrink<uInt16>( SB, sourceFormatInfo.blueBitsPerPixel, destinationFormatInfo.blueBitsPerPixel );
                    else if ( destinationFormatInfo.blueBitsPerPixel > sourceFormatInfo.blueBitsPerPixel ) // upsample
                        DB = expand<uInt16>( SB, sourceFormatInfo.blueBitsPerPixel, destinationFormatInfo.blueBitsPerPixel );
                    else
                        DB = SB;
                }

                if ( sourceAMask && destinationAMask )
                {
                    if ( destinationFormatInfo.alphaBitsPerPixel < sourceFormatInfo.alphaBitsPerPixel )	  // downsample
                        DA = shrink<uInt16>( SA, sourceFormatInfo.alphaBitsPerPixel, destinationFormatInfo.alphaBitsPerPixel );
                    else if ( destinationFormatInfo.alphaBitsPerPixel > sourceFormatInfo.alphaBitsPerPixel ) // upsample
                        DA = expand<uInt16>( SA, sourceFormatInfo.alphaBitsPerPixel, destinationFormatInfo.alphaBitsPerPixel );
                    else
                        DA = SA;
                }
            }
            
            U Dest = ( (U)( DR & destinationRMask ) << (U)destinationRShift ) | ( (U)( DG & destinationGMask ) << (U)destinationGShift ) | ( (U)( DB & destinationBMask ) << (U)destinationBShift ) | ( (U)( DA & destinationAMask ) << (U)destinationAShift );
            for ( i = 0; i < destinationFormatInfo.bytesPerPixel; i++ )
            {
                destinationItr[i] = (byte)( ( Dest >> ( (T)i * 8 ) ) & 0xff );
            }
        }
        return true;
    }

    bool convert( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat SourceFormat, VTFImageFormat DestFormat )
    {
        assert( !sourceData.empty() );
        //! Notice: The destination is allowed to be empty, but needs to be accounted for
        //! By whatever inserts data (either push_back, back_inserter or resize).

        assert( SourceFormat >= 0 && SourceFormat < IMAGE_FORMAT_COUNT );
        assert( DestFormat >= 0 && DestFormat < IMAGE_FORMAT_COUNT );

        auto& sourceFormatInfo = getimageFormatInfo(SourceFormat);
        auto& destinationFormatInfo = getimageFormatInfo(DestFormat);

        if ( !sourceFormatInfo.isSupported || !destinationFormatInfo.isSupported )
            return false;

        if ( SourceFormat == DestFormat )
        {
            destinationData = sourceData;
            return true;
        }

        if ( SourceFormat == IMAGE_FORMAT_RGB888 && DestFormat == IMAGE_FORMAT_RGBA8888 )
        {
            destinationData.clear();
            for(auto itr = sourceData.begin(); itr != sourceData.end(); itr +=4)
            {
                std::copy(itr, itr + 2, std::back_inserter(destinationData));
                destinationData.push_back(static_cast<byte>(255));
            }
            return true;
        }

        if ( SourceFormat == IMAGE_FORMAT_RGBA8888 && DestFormat == IMAGE_FORMAT_RGB888 )
        {
            destinationData.clear();
            for(auto itr = sourceData.begin(); itr != sourceData.end(); itr +=3)
                std::copy(itr, itr + 2, std::back_inserter(destinationData));
            return true;
        }

        //! NOTICE: This is (as far as I know) the most performant way of converting to/from
        //! floating point formats as any other solution would result in a lot of copying.
        //! Given this function is called A LOT internally for creating a single VTF
        //! It has to be as performant as possible.

        if ( SourceFormat == IMAGE_FORMAT_RGBA16161616F && ( DestFormat == IMAGE_FORMAT_RGBA32323232F || DestFormat == IMAGE_FORMAT_RGB323232F ) )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            uShort *lpSourceHFP = reinterpret_cast<unsigned short *>( sourceData.data() );
            uShort *lpLastHFP = reinterpret_cast<unsigned short *>( sourceData.data() + sourceData.size() );
            float *lpDestFP = reinterpret_cast<float *>( destinationData.data() );

            int channelCount = (DestFormat == IMAGE_FORMAT_RGBA32323232F ? 4 : 3);

            for ( ; lpSourceHFP < lpLastHFP; lpSourceHFP += 4, lpDestFP += channelCount )
            {
                lpDestFP[0] = FP16ToFP32( lpSourceHFP[0] );
                lpDestFP[1] = FP16ToFP32( lpSourceHFP[1] );
                lpDestFP[2] = FP16ToFP32( lpSourceHFP[2] );
                if ( channelCount == 4 )
                    lpDestFP[3] = FP16ToFP32( lpSourceHFP[3] );
            }
            return true;
        }

        if ( SourceFormat == IMAGE_FORMAT_RGBA16161616F )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            auto lpIntermediateRGBA = std::vector<byte>(computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA8888 ));

            if ( !HALF_HDR_TO_LDR( sourceData, lpIntermediateRGBA, width, height, sourceFormatInfo, destinationFormatInfo ) )
                return false;

            bool didConvert = convertFromRGBA8888( lpIntermediateRGBA, destinationData, width, height, DestFormat );

            return didConvert;
        }

        if ( DestFormat == IMAGE_FORMAT_RGBA16161616F )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            if ( SourceFormat == IMAGE_FORMAT_RGBA32323232F || SourceFormat == IMAGE_FORMAT_RGB323232F )
            {
                bool hasAlpha = SourceFormat == IMAGE_FORMAT_RGBA32323232F;

                float *lpSourceFP = reinterpret_cast<float *>( sourceData.data() );
                float *lpLastFP = reinterpret_cast<float *>( sourceData.data() + sourceData.size() );
                unsigned short *lpDestFP = reinterpret_cast<unsigned short *>( destinationData.data() );

                for ( ; lpSourceFP < lpLastFP; lpSourceFP += hasAlpha ? 4 : 3, lpDestFP += 4 )
                {
                    lpDestFP[0] = FP32ToFP16( lpSourceFP[0] );
                    lpDestFP[1] = FP32ToFP16( lpSourceFP[1] );
                    lpDestFP[2] = FP32ToFP16( lpSourceFP[2] );
                    lpDestFP[3] = hasAlpha ? FP32ToFP16( lpSourceFP[3] ) : FP32ToFP16( 1.f );
                }

                return true;
            }

            auto lpIntermediateRGBA = std::vector<byte>(computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA32323232F ));

            if ( !LDR_TO_HDR( sourceData, lpIntermediateRGBA, width, height, sourceFormatInfo, VTFImageFormatInfo[IMAGE_FORMAT_RGBA32323232F] ) )
                return false;

            bool didConvert = convert( lpIntermediateRGBA, destinationData, width, height, IMAGE_FORMAT_RGBA32323232F, DestFormat );

            return didConvert;
        }

        if ( SourceFormat == IMAGE_FORMAT_RGBA32323232F && DestFormat == IMAGE_FORMAT_RGB323232F )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            float *lpSourceFP = reinterpret_cast<float *>( sourceData.data() );
            float *lpLastFP = reinterpret_cast<float *>( sourceData.data() + sourceData.size() );
            float *lpDestFP = reinterpret_cast<float *>( destinationData.data() );

            for ( ; lpSourceFP < lpLastFP; lpSourceFP += 4, lpDestFP += 3 )
            {
                lpDestFP[0] = lpSourceFP[0];
                lpDestFP[1] = lpSourceFP[1];
                lpDestFP[2] = lpSourceFP[2];
            }
            return true;
        }

        if ( SourceFormat == IMAGE_FORMAT_RGB323232F && DestFormat == IMAGE_FORMAT_RGBA32323232F )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            float *lpSourceFP = reinterpret_cast<float *>( sourceData.data() );
            float *lpLastFP = reinterpret_cast<float *>( sourceData.data() + sourceData.size() );
            float *lpDestFP = reinterpret_cast<float *>( destinationData.data() );

            for ( ; lpSourceFP < lpLastFP; lpSourceFP += 3, lpDestFP += 4 )
            {
                lpDestFP[0] = lpSourceFP[0];
                lpDestFP[1] = lpSourceFP[1];
                lpDestFP[2] = lpSourceFP[2];
                lpDestFP[3] = 1.f;
            }

            return true;
        }

        if ( DestFormat == IMAGE_FORMAT_RGBA32323232F || DestFormat == IMAGE_FORMAT_RGB323232F )
        {
            destinationData.resize(computeImageSize( width, height, 1, DestFormat ));
            std::vector<byte> lpRGBA8888Data = std::vector<byte>(computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA8888 ));
            if ( !convertToRGBA8888( sourceData, lpRGBA8888Data, width, height, SourceFormat ) )
            {
                return false;
            }

            if ( DestFormat == IMAGE_FORMAT_RGBA32323232F )
                LDR_TO_HDR( lpRGBA8888Data, destinationData, width, height, VTFImageFormatInfo[IMAGE_FORMAT_RGBA8888], destinationFormatInfo );
            else
            {
                auto lpRGBAFP32Data = std::vector<byte>(computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA32323232F ));

                LDR_TO_HDR( lpRGBA8888Data, lpRGBAFP32Data, width, height, VTFImageFormatInfo[IMAGE_FORMAT_RGBA8888], VTFImageFormatInfo[IMAGE_FORMAT_RGBA32323232F] );

                if ( !convert( lpRGBAFP32Data, sourceData, width, height, IMAGE_FORMAT_RGBA32323232F, DestFormat ) )
                    return false;

            }

            return true;
        }

        if ( sourceFormatInfo.isCompressed || destinationFormatInfo.isCompressed )
        {
            std::vector<byte> lpSourceRGBA = sourceData;
            bool bResult = true;

            if ( SourceFormat != IMAGE_FORMAT_RGBA8888 )
                lpSourceRGBA.resize(computeImageSize( width, height, 1, IMAGE_FORMAT_RGBA8888 ));

            switch ( SourceFormat )
            {
                case IMAGE_FORMAT_RGBA8888:
                    break;
                case IMAGE_FORMAT_DXT1:
                case IMAGE_FORMAT_DXT1_ONEBITALPHA:
                case IMAGE_FORMAT_DXT3:
                case IMAGE_FORMAT_DXT5:
                case IMAGE_FORMAT_ATI2N:
                case IMAGE_FORMAT_ATI1N:
                case IMAGE_FORMAT_BC7:
                    bResult = decompressBCn( sourceData, lpSourceRGBA, width, height, SourceFormat ) == CMP_OK;
                    break;
                default:
                    bResult = convert( sourceData, lpSourceRGBA, width, height, SourceFormat, IMAGE_FORMAT_RGBA8888 );
                    break;
            }

            if ( bResult )
            {
                switch ( DestFormat )
                {
                    case IMAGE_FORMAT_DXT1:
                    case IMAGE_FORMAT_DXT1_ONEBITALPHA:
                    case IMAGE_FORMAT_DXT3:
                    case IMAGE_FORMAT_DXT5:
                    case IMAGE_FORMAT_ATI2N:
                    case IMAGE_FORMAT_ATI1N:
                    case IMAGE_FORMAT_BC7:
                        bResult = compressBCn( lpSourceRGBA, destinationData, width, height, DestFormat ) == CMP_OK;
                        break;
                    default:
                        bResult = convert( lpSourceRGBA, destinationData, width, height, IMAGE_FORMAT_RGBA8888, DestFormat );
                        break;
                }
            }

            return bResult;
        }
        else
        {
            destinationData.resize(computeImageSize(width,height,1,destinationFormatInfo.imageFormat));

            if ( sourceFormatInfo.bytesPerPixel <= 1 )
            {
                if ( destinationFormatInfo.bytesPerPixel <= 1 )
                    return convert<uInt8, uInt8>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 2 )
                    return convert<uInt8, uInt16>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 4 )
                    return convert<uInt8, uInt32>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 8 )
                    return convert<uInt8, uInt64>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
            }
            else if ( sourceFormatInfo.bytesPerPixel <= 2 )
            {
                if ( destinationFormatInfo.bytesPerPixel <= 1 )
                    return convert<uInt16, uInt8>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 2 )
                    return convert<uInt16, uInt16>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 4 )
                    return convert<uInt16, uInt32>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 8 )
                    return convert<uInt16, uInt64>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
            }
            else if ( sourceFormatInfo.bytesPerPixel <= 4 )
            {
                if ( destinationFormatInfo.bytesPerPixel <= 1 )
                    return convert<uInt32, uInt8>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 2 )
                    return convert<uInt32, uInt16>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 4 )
                    return convert<uInt32, uInt32>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 8 )
                    return convert<uInt32, uInt64>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
            }
            else if ( sourceFormatInfo.bytesPerPixel <= 8 )
            {
                if ( destinationFormatInfo.bytesPerPixel <= 1 )
                    return convert<uInt64, uInt8>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 2 )
                    return convert<uInt64, uInt16>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 4 )
                    return convert<uInt64, uInt32>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
                else if ( destinationFormatInfo.bytesPerPixel <= 8 )
                    return convert<uInt64, uInt64>( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
            }
            else if ( sourceFormatInfo.bytesPerPixel <= 16 )
            {
                return HDR_TO_LDR( sourceData, destinationData, width, height, sourceFormatInfo, destinationFormatInfo );
            }
            return false;
        }
    }

    bool convertToRGBA8888( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat SourceFormat )
    {
        return convert( sourceData, destinationData, width, height, SourceFormat, IMAGE_FORMAT_RGBA8888 );
    }

    bool convertFromRGBA8888( std::vector<byte> &sourceData, std::vector<byte> &destinationData, uInt width, uInt height, VTFImageFormat DestFormat )
    {
        return convert( sourceData, destinationData, width, height, IMAGE_FORMAT_RGBA8888, DestFormat );
    }

    void toLuminance(uInt16 &R, uInt16 &G, uInt16 &B, uInt16 &A )
    {
        R = G = B = (uInt16)(luminanceWeightRed * (float)R + luminanceWeightGreen * (float)G + luminanceWeightBlue * (float)B );
    }

    void fromLuminance(uInt16 &R, uInt16 &G, uInt16 &B, uInt16 &A )
    {
        B = G = R;
    }

    void FromBlueScreen( uInt16 &R, uInt16 &G, uInt16 &B, uInt16 &A )
    {
        if ( R == blueScreenMaskRed && G == blueScreenMaskGreen && B == blueScreenMaskBlue )
        {
            R = blueScreenClearRed;
            G = blueScreenClearGreen;
            B = blueScreenClearBlue;
            A = 0x0000;
        }
        else
            A = 0xffff;
    }

    void ToBlueScreen( uInt16 &R, uInt16 &G, uInt16 &B, uInt16 &A )
    {
        if ( A == 0x0000 )
        {
            R = blueScreenMaskRed;
            G = blueScreenMaskGreen;
            B = blueScreenMaskBlue;
        }
        A = 0xffff;
    }

}