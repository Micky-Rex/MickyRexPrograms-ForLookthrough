#define PROGRAM 0


#if PROGRAM == 0
#include "source/physics_main.cpp"

#elif PROGRAM == 1
#include "source/ray_tracing_main.cpp"

#elif PROGRAM == 2
#include "source/GLtest.cpp"

#elif PROGRAM == 3
#include "source/multi_window_example.cpp"

#endif