#include <stdio.h>
#include <string.h>

#include "jx3dps.h"

int main(void)
{
    jx3dps_engine *session = jx3dps_create();
    if (!session) {
        return 1;
    }
    const char *result =
        jx3dps_call(session, "{\"op\":\"batch\",\"macro\":\"/cast sanchai\",\"scenario\":\"tx\",\"frames\":4800,\"workers\":1,\"iterations\":100,\"seed\":\"42\"}");
    puts(result);
    int failed = strstr(result, "\"error\"") != NULL;
    jx3dps_destroy(session);
    return failed;
}
