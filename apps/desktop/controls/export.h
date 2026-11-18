#ifndef CONTROLS_EXPORT_H
#define CONTROLS_EXPORT_H

#ifdef _WIN32
    #ifdef CONTROLS_BUILD
        #define CONTROLS_API __declspec(dllexport)
    #else
        #define CONTROLS_API __declspec(dllimport)
    #endif
    #define CONTROLS_PRIVATE
#else
    #define CONTROLS_API __attribute__((visibility("default")))
    #define CONTROLS_PRIVATE __attribute__((visibility("hidden")))
#endif

#endif // CONTROLS_EXPORT_H
