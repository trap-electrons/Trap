//
// Class to load images from DDS files,
// based on ogl_dds_texture_loader.cpp by Kevin Harris (kevin@codesampler.com)
//
// Author: Alex V. Boreskoff
//

#include    <stdio.h>

#ifdef  _WIN32
    #include    <windows.h>
#endif

#ifdef	MACOSX
	#include	<OpenGL/gl.h>
	#include	<OpenGL/glu.h>
	#include	<OpenGL/glext.h>
	#include	<GLUT/glut.h>
#else
#include    <GL/gl.h>
	#include    <GL/glu.h>
	#include    "../GL/glut.h"
#include    "../GL/glext.h"
#endif

#include    <string.h>
#include    "Data.h"
#include    "DdsLoader.h"
#include    "CompressedTexture.h"
#include    "DdsDefs.h"

const unsigned long FOURCC_DXT1 = 0x31545844; 		//(MAKEFOURCC('D','X','T','1'))
const unsigned long FOURCC_DXT3 = 0x33545844; 		//(MAKEFOURCC('D','X','T','3'))
const unsigned long FOURCC_DXT5 = 0x35545844; 		//(MAKEFOURCC('D','X','T','5'))

static	int	scale [] = { 1, 1, 3, 7, 15, 31, 63, 127, 255 };

struct	BitMask
{
	int	start, length;
};

static	void	getMask ( dword mask, BitMask& maskData )
{
	if ( mask == 0 )
	{
		maskData.start  = 0;
		maskData.length = 0;
		
		return;
	}
	
	for ( maskData.start = 0; (mask & 1) == 0; maskData.start++ )
		mask >>= 1;
		
	for ( maskData.length = 0; mask & 1; maskData.length++ )
		mask >>= 1;
}

Texture * DdsLoader :: load ( Data * data )
{
    DDS_HEADER      ddsd;
    char            filecode [4];
    int             factor;
    int             bufferSize;
    int             format;

                                        // Verify the file is a true .dds file
    data -> seekAbs  ( 0 );
    data -> getBytes ( filecode, 4 );

    if( strncmp ( filecode, "DDS ", 4 ) != 0 )
        return NULL;

                                        // Get the surface descriptor
    data -> getBytes ( &ddsd, sizeof ( ddsd ) );

    int numComponents = 0;

    if ( ddsd.ddspf.dwFlags & DDS_ALPHA_PIXELS )        // ARGB
        numComponents = 4;
    else                                                // RGB
	if ( ddsd.ddspf.dwRGBBitCount == 24 )
        numComponents = 3;
	else
	if ( ddsd.ddspf.dwRGBBitCount == 8 )				// 8 -bit
	{
		numComponents = 1;
		
		if ( ddsd.ddspf.dwRBitMask == 255 )
			return loadUncompressed8BitRGB ( data, ddsd, 0 );
		else
		if ( ddsd.ddspf.dwGBitMask == 255 )
			return loadUncompressed8BitRGB ( data, ddsd, 1 );
		else
		if ( ddsd.ddspf.dwBBitMask == 255 )
			return loadUncompressed8BitRGB ( data, ddsd, 2 );
		else
		if ( ddsd.ddspf.dwABitMask == 255 )
			return loadUncompressed8BitAlpha ( data, ddsd);
		
	}
	else
	if ( ddsd.ddspf.dwRGBBitCount == 16 )
	{
		return loadUncompressed16BitRGB ( data, ddsd );
	}
	
    if ( (ddsd.ddspf.dwFlags & DDS_FOURCC) == 0 )       // not compressed
    {
    	return loadUncompressed ( data, ddsd, numComponents );
    }

    //
    // This .dds loader supports the loading of compressed formats DXT1, DXT3
    // and DXT5.
    //

    int numBlocks = ((ddsd.dwWidth + 3)/4) * ((ddsd.dwHeight + 3)/4);   // number of 4*4 texel blocks
    int blockSize = 0;

    switch ( ddsd.ddspf.dwFourCC )
    {
        case FOURCC_DXT1:
                            // DXT1's compression ratio is 8:1
            if ( numComponents == 4 )
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            else
                format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

            factor    = 2;
            blockSize = numBlocks * 8;                              // get size of byte image (8 bytes per block)
            break;

        case FOURCC_DXT3:
                            // DXT3's compression ratio is 4:1
            format     = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            factor     = 4;
            blockSize  = numBlocks * 16;                            // get size of byte image (16 bytes per block)
            break;

        case FOURCC_DXT5:
                            // DXT5's compression ratio is 4:1
            format    = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            factor    = 4;
            blockSize = numBlocks * 16;                         // get size of byte image (16 bytes per block)
            break;

        default:
            return NULL;
    }

                                            // check for RGBA-only formats
    if ( format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
        numComponents = 4;
    else
        numComponents = 3;

    if( ddsd.dwPitchOrLinearSize != 0 )
        if( ddsd.dwMipMapCount > 1 )
            bufferSize = ddsd.dwPitchOrLinearSize * factor;
        else
            bufferSize = ddsd.dwPitchOrLinearSize;
    else
        bufferSize = blockSize;

    int mipMapsCount = ddsd.dwMipMapCount;

    if ( mipMapsCount < 1 )
        mipMapsCount = 1;

    CompressedTexture * texture = new CompressedTexture ( ddsd.dwWidth, ddsd.dwHeight, numComponents, format, mipMapsCount, bufferSize );

    data -> getBytes ( texture -> getData (), bufferSize );

    return texture;
}

bool	DdsLoader :: loadCubemap ( Data * data, Texture * images [6] )
{
    DDS_HEADER      ddsd;
    char            filecode [4];
    int             factor;
    int             bufferSize;
    int             format;
    int				i;
                                        // Verify the file is a true .dds file
    data -> seekAbs  ( 0 );
    data -> getBytes ( filecode, 4 );

    if( strncmp ( filecode, "DDS ", 4 ) != 0 )
        return false;

                                        // Get the surface descriptor
    data -> getBytes ( &ddsd, sizeof ( ddsd ) );

    int numComponents = 0;

    if ( ddsd.ddspf.dwFlags & DDS_ALPHA_PIXELS )        // ARGB
        numComponents = 4;
    else                                                // RGB
        numComponents = 3;

	if ( (ddsd.dwFlags & DDS_CAPS) == 0 )				// for cubemap must have caps fields
		return false;

	if ( (ddsd.dwCaps1 & (DDS_TEXTURE | DDS_COMPLEX)) != (DDS_TEXTURE | DDS_COMPLEX) )
		return false;

	if ( (ddsd.dwCaps2 & DDS_CUBEMAP) == 0 )
		return false;

	int	sides [6] = {
		DDS_CUBEMAP_POSITIVEX, DDS_CUBEMAP_NEGATIVEX,
		DDS_CUBEMAP_POSITIVEY, DDS_CUBEMAP_NEGATIVEY,
		DDS_CUBEMAP_POSITIVEZ, DDS_CUBEMAP_NEGATIVEZ
	};

    int mipMapsCount = ddsd.dwMipMapCount;

    if ( mipMapsCount < 1 )
        mipMapsCount = 1;

    if ( (ddsd.ddspf.dwFlags & DDS_FOURCC) == 0 )       // not compressed
    {
    	for ( i = 0; i < 6; i++ )
    		if ( ddsd.dwCaps2 & sides [i] )
		    	images [i] = loadUncompressed ( data, ddsd, numComponents );
		    else
		    	images [i] = NULL;

		return true;
    }

    //
    // This .dds loader supports the loading of compressed formats DXT1, DXT3
    // and DXT5.
    //

    int numBlocks = ((ddsd.dwWidth + 3)/4) * ((ddsd.dwHeight + 3)/4);   // number of 4*4 texel blocks
    int blockSize = 0;

    switch ( ddsd.ddspf.dwFourCC )
    {
        case FOURCC_DXT1:
                            // DXT1's compression ratio is 8:1
            if ( numComponents == 4 )
                format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            else
                format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

            factor    = 2;
            blockSize = numBlocks * 8;                              // get size of byte image (8 bytes per block)
            break;

        case FOURCC_DXT3:
                            // DXT3's compression ratio is 4:1
            format        = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            numComponents = 4;
            factor        = 4;
            blockSize     = numBlocks * 16;                         // get size of byte image (16 bytes per block)
            break;

        case FOURCC_DXT5:
                            // DXT5's compression ratio is 4:1
            format        = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            numComponents = 4;
            factor        = 4;
            blockSize     = numBlocks * 16;                         // get size of byte image (16 bytes per block)
            break;

        default:
            return false;
    }

    if( ddsd.dwPitchOrLinearSize != 0 )
        bufferSize = ddsd.dwPitchOrLinearSize;
    else
        bufferSize = blockSize;

	int	w = (int)ddsd.dwWidth;
	int	h = (int)ddsd.dwHeight;
	int	blocks;

	for ( i = 1; i < mipMapsCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		blocks      = ((w + 3)/4) * ((h + 3)/4);   							// number of 4*4 texel blocks
		bufferSize += blocks * factor * 4;
	}

	for ( i = 0; i < 6; i++ )
		if ( ddsd.dwCaps2 & sides [i] )
		{
			images [i] = new CompressedTexture ( ddsd.dwWidth, ddsd.dwHeight, numComponents, format, mipMapsCount, bufferSize );

	    	data -> getBytes ( images [i] -> getData (), bufferSize );
	    }
	    else
	    	images [i] = NULL;

    return true;
}

Texture * DdsLoader :: loadUncompressed ( Data * data, const DDS_HEADER& ddsd, int numComponents )
{
	int	w            = (int)ddsd.dwWidth;
	int	h            = (int)ddsd.dwHeight;
	int bytesPerLine = w * numComponents;
	int	i;

    if ( (bytesPerLine & 3) != 0 )                  // do dword alignment
    	bytesPerLine += 4 - (bytesPerLine & 3);

    byte    * buf     = new byte [bytesPerLine];
    Texture * texture = new Texture ( w, h, numComponents );

    for ( i = 0; i < h; i++ )
    {
    	data -> getBytes ( buf, bytesPerLine );

                                                        // rearrange components
        byte * dest = texture -> getData () + i * w * numComponents;
        byte * src  = buf;

        for ( register int j = 0; j < w; j++ )
        {
        	dest [0] = src [2];                     // red
            dest [1] = src [1];                     // green
            dest [2] = src [0];                     // blue

            if ( numComponents == 4 )
            	dest [3] = src [3];                 // alpha

            dest += numComponents;
            src  += numComponents;
        }
    }

													// do mipmap adjustement
	skipMipmaps ( data, w, h, bytesPerLine, numComponents, (int) ddsd.dwMipMapCount );
/*													
	for ( i = 1; i < (int) ddsd.dwMipMapCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		bytesPerLine = w * numComponents;

	    if ( (bytesPerLine & 3) != 0 )                // do dword alignment
    		bytesPerLine += 4 - (bytesPerLine & 3);

    	data -> seekCur ( bytesPerLine * h );
	}
*/
	delete buf;

    return texture;
}

Texture * DdsLoader :: loadUncompressed8BitRGB ( Data * data, const DDS_HEADER& ddsd, int component )
{
	int	w            = (int)ddsd.dwWidth;
	int	h            = (int)ddsd.dwHeight;
	int bytesPerLine = w;

    if ( (bytesPerLine & 3) != 0 )                  // do dword alignment
    	bytesPerLine += 4 - (bytesPerLine & 3);

    Texture * texture = new Texture ( w, h, 3 );

	loadUncompressed8BitRGBData ( data, ddsd, component, bytesPerLine, texture -> getData () );
	
	return texture;
}

bool	DdsLoader :: loadUncompressed8BitRGBData ( Data * data, const DDS_HEADER& ddsd, int component, int bytesPerLine, byte * ptr )
{
 	int	   w   = (int)ddsd.dwWidth;
	int	   h   = (int)ddsd.dwHeight;
	byte * buf = new byte [bytesPerLine];
	int	   i;

    if ( (ddsd.dwFlags & DDS_DEPTH) != 0 )              // 3D texture
		h *= ddsd.dwDepth;

    for ( i = 0; i < h; i++ )
    {
    	data -> getBytes ( buf, bytesPerLine );

                                                        // rearrange components
        byte * dest = ptr + i * w * 3;
        byte * src  = buf;

        for ( register int j = 0; j < w; j++ )
        {
        	dest [0]         = 0;                     // red
            dest [1]         = 0;                     // green
            dest [2]         = 0;                     // blue
			dest [component] = src [0];
            dest            += 3;
            src             += 1;
        }
    }

													// do mipmap adjustement
	skipMipmaps ( data, w, h, bytesPerLine, 3, (int) ddsd.dwMipMapCount );
/*
	for ( i = 1; i < (int) ddsd.dwMipMapCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		bytesPerLine = w * 3;

	    if ( (bytesPerLine & 3) != 0 )                // do dword alignment
    		bytesPerLine += 4 - (bytesPerLine & 3);

    	data -> seekCur ( bytesPerLine * h );
	}
*/
	delete buf;

    return true;
}

Texture * DdsLoader :: loadUncompressed8BitAlpha ( Data * data, const DDS_HEADER& ddsd )
{
	int	w            = (int)ddsd.dwWidth;
	int	h            = (int)ddsd.dwHeight;
	int bytesPerLine = w;

    if ( (bytesPerLine & 3) != 0 )                  // do dword alignment
    	bytesPerLine += 4 - (bytesPerLine & 3);

    Texture * texture = new Texture ( w, h, 1 );
		
	loadUncompressed8BitAlphaData ( data, ddsd, bytesPerLine, texture -> getData () );
	
	return texture;
}

Texture * DdsLoader :: loadUncompressed16BitRGB  ( Data * data, const DDS_HEADER& ddsd )
{
	int		w            = (int)ddsd.dwWidth;
	int		h            = (int)ddsd.dwHeight;
	int 	bytesPerLine = 2*w;
	
    if ( (bytesPerLine & 3) != 0 )                  // do dword alignment
    	bytesPerLine += 4 - (bytesPerLine & 3);

    Texture * texture = new Texture ( w, h, 3 );

	loadUncompressed16BitRGBData ( data, ddsd, bytesPerLine, texture -> getData () );
	
	return texture;
}

bool	DdsLoader :: loadUncompressed8BitAlphaData ( Data * data, const DDS_HEADER& ddsd, int bytesPerLine, byte * ptr )
{
	int	   w   = (int)ddsd.dwWidth;
	int	   h   = (int)ddsd.dwHeight;
    byte * buf = new byte [bytesPerLine];
	int	   i;

    if ( (ddsd.dwFlags & DDS_DEPTH) != 0 )              // 3D texture
		h *= ddsd.dwDepth;

    for ( i = 0; i < h; i++ )
    {
    	data -> getBytes ( buf, bytesPerLine );

                                                        // rearrange components
        byte * dest = ptr + i * w ;
        byte * src  = buf;

        for ( register int j = 0; j < w; j++ )
        {
			dest [0] = src [0];
            dest    += 1;
            src     += 1;
        }
    }

													// do mipmap adjustement
	skipMipmaps ( data, w, h, bytesPerLine, 1, (int) ddsd.dwMipMapCount );
/*													
	for ( i = 1; i < (int) ddsd.dwMipMapCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		bytesPerLine = w;

	    if ( (bytesPerLine & 3) != 0 )                // do dword alignment
    		bytesPerLine += 4 - (bytesPerLine & 3);

    	data -> seekCur ( bytesPerLine * h );
	}
*/
	delete buf;

    return true;
}

bool	DdsLoader :: loadUncompressed16BitRGBData ( Data * data, const DDS_HEADER& ddsd, int bytesPerLine, byte * ptr )
{
	int		w     = (int)ddsd.dwWidth;
	int		h     = (int)ddsd.dwHeight;
    word    * buf = new word [bytesPerLine];
	int		i;
	BitMask	redMask, greenMask, blueMask;
	
	getMask ( ddsd.ddspf.dwRBitMask, redMask   );
	getMask ( ddsd.ddspf.dwGBitMask, greenMask );
	getMask ( ddsd.ddspf.dwBBitMask, blueMask  );

    if ( (ddsd.dwFlags & DDS_DEPTH) != 0 )              // 3D texture
		h *= ddsd.dwDepth;

    for ( i = 0; i < h; i++ )
    {
    	data -> getBytes ( buf, bytesPerLine );

                                                        // rearrange components
        byte * dest = ptr + i * w * 3;
        word * src  = buf;

        for ( register int j = 0; j < w; j++ )
        {
			word	val = src [0];
			
        	dest [0]         = (255*((val & ddsd.ddspf.dwRBitMask) >> redMask.start))  / scale [redMask.length]; 
            dest [1]         = (255*((val & ddsd.ddspf.dwGBitMask) >> greenMask.start))/ scale [greenMask.length];
            dest [2]         = (255*((val & ddsd.ddspf.dwBBitMask) >> blueMask.start)) / scale [blueMask.length];
            dest            += 3;
            src             += 1;
        }
    }

													// do mipmap adjustement
	skipMipmaps ( data, w, h, bytesPerLine, 2, (int) ddsd.dwMipMapCount );
/*	
	for ( i = 1; i < (int) ddsd.dwMipMapCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		bytesPerLine = w * 2;

	    if ( (bytesPerLine & 3) != 0 )                // do dword alignment
    		bytesPerLine += 4 - (bytesPerLine & 3);

    	data -> seekCur ( bytesPerLine * h );
	}
*/
	delete buf;

    return true;
}

void	  DdsLoader :: skipMipmaps ( Data * data, int w, int h, int bytesPerLine, int cnt, int mipMapCount )
{
	for ( int i = 1; i < mipMapCount; i++ )
	{
		w /= 2;
		h /= 2;

		if ( w < 1 )
			w = 1;

		if ( h < 1 )
			h = 1;

		bytesPerLine = w * cnt;

	    if ( (bytesPerLine & 3) != 0 )                // do dword alignment
    		bytesPerLine += 4 - (bytesPerLine & 3);

    	data -> seekCur ( bytesPerLine * h );
	}
}
