#ifndef DirectX11SharedDefines_h__
#define DirectX11SharedDefines_h__

#define MAX_LIGHTS_POWER 10
#define MAX_LIGHTS (1<<MAX_LIGHTS_POWER)

#define BLOCK_SIZE 16
#define GROUP_SIZE (BLOCK_SIZE*BLOCK_SIZE)

#ifndef MSAA_SAMPLES
#define MSAA_SAMPLES 1
#endif

#endif // DirectX11SharedDefines_h__