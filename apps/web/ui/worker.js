import createModule from "../jx3dps.mjs";

let session;
let module;
function call(request) {
    const text = module.ccall(
        "jx3dps_call",
        "string",
        ["number", "string"],
        [session, JSON.stringify(request)],
    );
    const reply = JSON.parse(text);
    if (reply.error) throw new Error(reply.error);
    return reply;
}
const ready = createModule()
    .then((instance) => {
        module = instance;
        session = module._jx3dps_create();
        if (!session || module._jx3dps_abi_version() !== 1) throw new Error("内核初始化失败");
        self.postMessage({ type: "ready", defaults: call({ op: "defaults" }) });
    })
    .catch((error) => self.postMessage({ type: "fatal", error: error.message }));

self.onmessage = async ({ data }) => {
    await ready;
    if (!module || !session) return;
    try {
        if (data.type === "step") {
            self.postMessage({ type: "debug", state: call({ op: "debug", action: data.action }) });
            return;
        }
        const start = performance.now();
        const state = call({ op: "start", ...data.request, debug: data.type === "debug" });
        if (data.type === "debug") {
            self.postMessage({ type: "debug", state });
            return;
        }
        let result;
        do {
            result = call({ op: "next" });
            if (!result.skills) self.postMessage({ type: "progress", completed: result.completed });
        } while (!result.skills);
        self.postMessage({ type: "result", result, elapsed: (performance.now() - start) / 1000 });
    } catch (error) {
        self.postMessage({ type: "error", error: error.message });
    }
};
