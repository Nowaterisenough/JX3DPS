#include <stdio.h>
#include <string.h>

#include "jx3dps.h"
#define CHECK(x)                                              \
    do {                                                      \
        if (!(x)) {                                           \
            fprintf(stderr, "failed at line %d\n", __LINE__); \
            return 1;                                         \
        }                                                     \
    } while (0)

int main(void)
{
    jx3dps_engine *a = jx3dps_create();
    jx3dps_engine *b = jx3dps_create();
    CHECK(a && b && jx3dps_abi_version() == 1);
    const char *preset = "{\"op\":\"start\",\"scenario\":\"mw\",\"seconds\":10,\"iterations\":1,\"seed\":\"42\"}";
    CHECK(strstr(jx3dps_call(a, preset), "\"completed\":0"));
    char reference[8192];
    snprintf(reference, sizeof(reference), "%s", jx3dps_call(a, "{\"op\":\"next\"}"));
    CHECK(strstr(reference, "\"checksum\""));
    CHECK(strstr(jx3dps_call(a, "{\"op\":\"start\",\"scenario\":\"mw\",\"seconds\":10,\"iterations\":1,\"seed\":\"42\","
                                "\"config\":{\"attributes\":{\"crit\":2741}}}"), "\"completed\":0"));
    CHECK(strcmp(reference, jx3dps_call(a, "{\"op\":\"next\"}")) == 0);
    const char *start = "{\"op\":\"start\",\"scenario\":\"tx\",\"seconds\":300,\"iterations\":129,\"seed\":\"42\","
                        "\"macro\":\"/cast sanchai\",\"attributes\":{}}";
    CHECK(strstr(jx3dps_call(a, start), "\"completed\":0"));
    CHECK(strstr(jx3dps_call(b, "{"), "\"error\""));
    CHECK(strstr(jx3dps_call(a, "{\"op\":\"next\"}"), "\"completed\":128"));
    CHECK(jx3dps_completed(a) == 128 && jx3dps_completed(b) == 0);
    CHECK(strstr(jx3dps_call(a, "{\"op\":\"next\"}"), "\"checksum\""));
    CHECK(strstr(jx3dps_call(a, "{\"op\":\"next\"}"), "\"error\""));
    const char *debug_start = "{\"op\":\"start\",\"scenario\":\"mw\",\"seconds\":5,\"iterations\":1,\"seed\":\"42\","
                              "\"macro\":\"/cast gong\",\"debug\":true,\"breakpoints\":[]}";
    CHECK(strstr(jx3dps_call(b, start), "\"completed\":0"));
    CHECK(strstr(jx3dps_call(b, debug_start), "\"finished\":false"));
    CHECK(strstr(jx3dps_call(b, "{\"op\":\"debug\",\"action\":\"continue\"}"), "\"finished\":true"));
    CHECK(strstr(jx3dps_call(b, start), "\"completed\":0"));
    CHECK(strstr(jx3dps_call(b, "{\"op\":\"next\"}"), "\"completed\":128"));
    jx3dps_destroy(a);
    jx3dps_destroy(b);
    return 0;
}
