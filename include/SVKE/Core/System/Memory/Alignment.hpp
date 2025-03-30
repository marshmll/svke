#pragma once

#define ALIGNAS_SCLR(T)                alignas(sizeof(T))
#define ALIGNAS_VEC2                   alignas(8)
#define ALIGNAS_VEC3                   alignas(16)
#define ALIGNAS_VEC4                   alignas(16)
#define ALIGNAS_MAT2                   alignas(16)
#define ALIGNAS_MAT3                   alignas(16)
#define ALIGNAS_MAT4                   alignas(16)
#define ALIGNAS_NESTED_UNIFORM         alignas(16)
#define ALIGNAS_NESTED_STORAGE(NESTED) alignas(alignof(NESTED))
