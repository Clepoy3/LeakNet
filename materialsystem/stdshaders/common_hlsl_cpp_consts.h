#ifdef NV3X
  #define PSHADER_VECT_SCALE 20.0
  #define VSHADER_VECT_SCALE (1.0 / (PSHADER_VECT_SCALE) )
#else
  #define PSHADER_VECT_SCALE 1.0
  #define VSHADER_VECT_SCALE 1.0
#endif

// GR - HDR luminance maps to 0..n range
// IMPORTANT: Keep the same value as in materialsystem_global.h
//#define MAX_HDR_OVERBRIGHT 16.0f
//#define MAX_HDR_OVERBRIGHT 4.0f // VXP: Without prefer textures
//#define MAX_HDR_OVERBRIGHT 1.7f // VXP: Good for HL2. but very bright at de_dust2 (CS)
#define MAX_HDR_OVERBRIGHT 16.0f



