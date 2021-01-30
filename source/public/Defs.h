#pragma once

#if _DEBUG
#define DPRINT(x) (puts(x))
#define DPRINTF(x, ...) (printf(x,  __VA_ARGS__))
#else
#define DPRINT(X)
#define DPRINTF(x, ...)
#endif
