#ifdef NV3X
  #define PSHADER_VECT_SCALE 20.0
  #define VSHADER_VECT_SCALE (1.0 / (PSHADER_VECT_SCALE) )
#else
  #define PSHADER_VECT_SCALE 1.0
  #define VSHADER_VECT_SCALE 1.0
#endif

// GR - HDR luminance maps to 0..n range
// IMPORTANT: Keep the same value as in materialsystem_global.h
#define MAX_HDR_OVERBRIGHT 16.0f

#define GAMMA_FOG_COLOR 29
#define TONE_MAPPING_GAMMA_SCALE_PSH_CONSTANT 30
#define TONE_MAPPING_LINEAR_SCALE_PSH_CONSTANT 31
