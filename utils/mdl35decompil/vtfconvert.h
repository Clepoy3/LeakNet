
#ifndef BIT
#define BIT( n )		(1<<( n ))
#endif

#define MAX_STRING		256

typedef unsigned char 	byte;
typedef unsigned short	word;
typedef unsigned long	dword;
typedef unsigned __int64	qword;
typedef unsigned int	uint;
typedef int		BOOL;
typedef signed __int64	int64;
typedef int		func_t;
typedef int		sound_t;
typedef int		model_t;
typedef int		shader_t;
typedef struct cvar_s	cvar_t;
typedef struct edict_s	edict_t;
typedef struct pmove_s	pmove_t;
typedef struct movevars_s	movevars_t;
typedef struct usercmd_s	usercmd_t;
typedef struct cl_priv_s	cl_priv_t;
typedef struct sv_priv_s	sv_priv_t;
typedef unsigned short	CRC16_t;
typedef unsigned long	CRC32_t;
typedef float		vec_t;
typedef char		string[MAX_STRING];

typedef vec_t		vec2_t[2];
typedef vec_t		vec3_t[3];
typedef vec_t		vec4_t[4];

#define VTFHEADER		(('\0'<<24)+('F'<<16)+('T'<<8)+'V')
#define VTF_VERSION		7
#define VTF_SUBVERSION0	0	// oldest textures from beta

typedef enum
{
	PF_UNKNOWN = 0,
	PF_INDEXED_24,	// inflated palette (768 bytes)
	PF_INDEXED_32,	// deflated palette (1024 bytes)
	PF_RGBA_32,	// normal rgba buffer
	PF_BGRA_32,	// big endian RGBA (MacOS)
	PF_ARGB_32,	// uncompressed dds image
	PF_ABGR_64,	// uint image
	PF_RGB_24,	// uncompressed dds or another 24-bit image 
	PF_BGR_24,	// big-endian RGB (MacOS)
	PF_RGB_16,	// 5-6-5 weighted image
	PF_DXT1,		// nvidia DXT1 format
	PF_DXT2,		// nvidia DXT2 format
	PF_DXT3,		// nvidia DXT3 format
	PF_DXT4,		// nvidia DXT5 format
	PF_DXT5,		// nvidia DXT5 format
	PF_RXGB,		// doom3 normal maps
	PF_ATI1N,		// ati 1N texture
	PF_ATI2N,		// ati 2N texture
	PF_LUMINANCE,	// b&w dds image
	PF_LUMINANCE_16,	// b&w hi-res image
	PF_LUMINANCE_ALPHA, // b&w dds image with alpha channel
	PF_UV_16,		// EMBM two signed 8bit components
	PF_UV_32,		// EMBM two signed 16bit components
	PF_R_16F,		// red channel half-float image
	PF_R_32F,		// red channel float image
	PF_GR_32F,	// Green-Red channels half-float image (dudv maps)
	PF_GR_64F,	// Green-Red channels float image (dudv maps)
	PF_ABGR_64F,	// ABGR half-float image
	PF_ABGR_128F,	// ABGR float image
	PF_RGBA_GN,	// internal generated texture
	PF_TOTALCOUNT,	// must be last
} pixformat_t;

// rgbdata output flags
typedef enum
{
	// rgbdata->flags
	IMAGE_CUBEMAP	= BIT(0),		// it's 6-sides cubemap buffer
	IMAGE_HAS_ALPHA	= BIT(1),		// image contain alpha-channel
	IMAGE_HAS_COLOR	= BIT(2),		// image contain RGB-channel
	IMAGE_COLORINDEX	= BIT(3),		// all colors in palette is gradients of last color (decals)
	IMAGE_PREMULT	= BIT(4),		// need to premultiply alpha (DXT2, DXT4)
	IMAGE_HAS_LUMA_Q1	= BIT(5),		// image has luma pixels (q1-style maps)
	IMAGE_HAS_LUMA_Q2	= BIT(6),		// image has luma pixels (q2-style maps)
	IMAGE_HAS_LUMA	= IMAGE_HAS_LUMA_Q1|IMAGE_HAS_LUMA_Q2,
	IMAGE_SKYBOX	= BIT(7),		// only used by FS_SaveImage - for write right suffixes

	// Image_Process manipulation flags
	IMAGE_FLIP_X	= BIT(12),	// flip the image by width
	IMAGE_FLIP_Y	= BIT(13),	// flip the image by height
	IMAGE_ROT_90	= BIT(14),	// flip from upper left corner to down right corner
	IMAGE_ROT180	= IMAGE_FLIP_X|IMAGE_FLIP_Y,
	IMAGE_ROT270	= IMAGE_FLIP_X|IMAGE_FLIP_Y|IMAGE_ROT_90,	
	IMAGE_ROUND	= BIT(15),	// round image to nearest Pow2
	IMAGE_RESAMPLE	= BIT(16),	// resample image to specified dims
	IMAGE_PALTO24	= BIT(17),	// turn 32-bit palette into 24-bit mode (only for indexed images)
	IMAGE_COMP_DXT	= BIT(18),	// compress image to DXT format
	IMAGE_ROUNDFILLER	= BIT(19),	// round image to nearest Pow2 and fill unused entries with single color	
	IMAGE_FORCE_RGBA	= BIT(20),	// force image to RGBA buffer
	IMAGE_MAKE_LUMA	= BIT(21),	// create luma texture from indexed
} imgFlags_t;

typedef enum
{
	IL_HINT_NO = 0,
	IL_HINT_Q1,	// palette choosing
	IL_HINT_Q2,
	IL_HINT_HL,
	IL_HINT_FORCE_RGBA,	// force to unpack any image
} image_hint_t;

// cubemap hints
typedef enum
{
	CB_HINT_NO = 0,

	// dds cubemap hints ( Microsoft sides order )
	CB_HINT_POSX,
	CB_HINT_NEGX,
	CB_HINT_POSZ,
	CB_HINT_NEGZ,
	CB_HINT_POSY,
	CB_HINT_NEGY,

	// vtf format can have 7th cubemap side who called as "spheremap"
	CB_HINT_ENVMAP,	// same as sides count
	CB_FACECOUNT = CB_HINT_ENVMAP,
} side_hint_t;

typedef struct rgbdata_s
{
	word	width;		// image width
	word	height;		// image height
	word	depth;		// multi-layer volume
	byte	numMips;		// mipmap count
	byte	bitsCount;	// RGB bits count
	uint	type;		// compression type
	uint	flags;		// misc image flags
	byte	*palette;		// palette if present
	byte	*buffer;		// image buffer
	size_t	size;		// for bounds checking
} rgbdata_t;

typedef struct loadformat_s
{
	const char *formatstring;
	const char *ext;
	bool (*loadfunc)( const char *name, const byte *buffer, size_t filesize );
	image_hint_t hint;
} loadformat_t;

typedef struct saveformat_s
{
	const char *formatstring;
	const char *ext;
	bool (*savefunc)( const char *name, rgbdata_t *pix );
} saveformat_t;

typedef struct imglib_s
{
	const loadformat_t	*baseformats;	// used for loading internal images
	const loadformat_t	*loadformats;
	const saveformat_t	*saveformats;

	// current 2d image state
	word		width;
	word		height;
	word		depth;		// num layers in
	byte		num_mips;		// build mipmaps
	uint		type;		// main type switcher
	uint		flags;		// additional image flags
	byte		bits_count;	// bits per RGBA
	size_t		size;		// image rgba size (for bounds checking)
	uint		ptr;		// safe image pointer
	byte		*rgba;		// image pointer (see image_type for details)

	// current cubemap state
	int		source_width;	// locked cubemap dims (all wrong sides will be automatically resampled)
	int		source_height;
	uint		source_type;	// shared image type for all mipmaps or cubemap sides
	int		num_sides;	// how much sides is loaded 
	byte		*cubemap;		// cubemap pack
	side_hint_t	filter;		// filtering side

	// indexed images state
	uint		*d_currentpal;	// installed version of internal palette
	int		d_rendermode;	// palette rendermode
	byte		*palette;		// palette pointer

	// global parms
	int		curwidth;		// cubemap side, layer or mipmap width
	int		curheight;	// cubemap side, layer or mipmap height
	int		curdepth;		// current layer number
	int		cur_mips;		// number of mips
	int		bpp;		// PFDesc[type].bpp
	int		bpc;		// PFDesc[type].bpc
	int		bps;		// width * bpp * bpc
	int		SizeOfPlane;	// bps * height
	int		SizeOfData;	// SizeOfPlane * bps
	int		SizeOfFile;	// Image_DxtGetSize
	bool		(*decompress)( uint, int, int, uint, uint, uint, const void* );

	image_hint_t	hint;		// hint for some loaders
	byte		*tempbuffer;	// for convert operations
	int		cmd_flags;
} imglib_t;

bool Image_LoadVTF( const char *name, const byte *buffer, size_t filesize );
