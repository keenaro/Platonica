#pragma once

#if _DEBUG
#define DPRINT(x) (puts(x))
#else
#define DPRINT(X)
#endif