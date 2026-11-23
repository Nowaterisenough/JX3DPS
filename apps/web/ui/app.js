const $ = (id) => document.getElementById(id);
const fmt = (n, digits = 0) => new Intl.NumberFormat("zh-CN", { maximumFractionDigits: digits }).format(n);
const names = {};
const schools = new Map();
const selections = new Map();
const panels = new Map();
const permanentDrafts = new Map();
function showRaidDefaults() {
    const preset = schools.get(school)?.raidPermanents ?? {};
    for (const select of $("permanentChoices").querySelectorAll("select")) {
        select.options[0].textContent = $("raidFull").checked ? preset[select.dataset.category] : "无";
    }
}
$("raidFull").addEventListener("change", showRaidDefaults);
function selectedPermanents() {
    return Object.fromEntries([...$("permanentChoices").querySelectorAll("select")]
        .filter(input => input.value).map(input => [input.dataset.category, input.value]));
}
const panelInputs = () => [...document.querySelectorAll("#settings [data-stat]")];
const colors = ["#216753", "#448e71", "#76a97a", "#b7c98a", "#caab6f", "#91a5a1", "#7b8a74"];
const records = [],
    drafts = {};
let worker,
    defaults,
    school = "tx",
    busy = false,
    auto = true,
    request,
    selected,
    iconMap = {};
const el = (tag, text, klass) => {
    const node = document.createElement(tag);
    if (text !== undefined) node.textContent = text;
    if (klass) node.className = klass;
    return node;
};
function message(s) {
    $("error").textContent = s;
    $("error").hidden = !s;
}
function syncEditor() {
    $("gutter").textContent = $("macro")
        .value.split("\n")
        .map((_, i) => i + 1)
        .join("\n");
    $("gutter").scrollTop = $("macro").scrollTop;
}
function setSchool(next) {
    if (schools.has(school) && $("schoolSkills").textContent) {
        drafts[school] = $("macro").value;
        panels.set(school, Object.fromEntries(panelInputs().map(input => [input.dataset.stat, input.value])));
        permanentDrafts.set(school, selectedPermanents());
        selections.set(school, new Set([...$("schoolChoices").querySelectorAll("input:checked")].map(input=>input.value)));
    }
    school = next;
    const spec = schools.get(next);
    $("schoolRules").hidden = !spec;
    $("schoolSkills").textContent = spec?.skills.join("、") ?? "";
    $("schoolChoices").replaceChildren();
    $("permanentChoices").replaceChildren();
    if (spec) {
        const draft = permanentDrafts.get(next) ?? {};
        for (const category of ["阵法","食品增强","食品辅助","药品增强","药品辅助","武器磨石","团队宴席","家园酒品","家园菜品","家园香水"]) {
            const options = spec.permanents[category];
            if (!options?.length) continue;
            const label = el("label", category), select = el("select");
            select.dataset.category = category;
            select.append(new Option("无", ""), ...options.map(name => new Option(name, name)));
            select.value = draft[category] ?? "";
            label.append(select);
            $("permanentChoices").append(label);
        }
        const panel = panels.get(next) ?? spec.attributes;
        for (const input of panelInputs()) input.value = panel[input.dataset.stat];
        for (const group of ["talents", "recipes", "equipment"]) {
            for (const name of spec[group]) {
                const label = el("label"), input = el("input");
                input.type = "checkbox";
                input.dataset.group = group;
                input.value = name;
                input.checked = selections.has(next) ? selections.get(next).has(name)
                    : ({talents: spec.defaultTalents, recipes: spec.defaultRecipes, equipment: spec.defaultEquipment})[group].includes(name);
                if (group === "equipment" && spec.equipmentSlots[name]) {
                    input.dataset.slot = spec.equipmentSlots[name];
                    input.onchange = () => {
                        if (!input.checked) return;
                        const others = [...$("schoolChoices").querySelectorAll("input[data-slot]:checked")]
                            .filter(other => other !== input && other.dataset.slot === input.dataset.slot);
                        const capacity = input.dataset.slot === "戒指" ? 2 : 1;
                        while (others.length >= capacity) others.shift().checked = false;
                    };
                }
                if (group === "recipes" && spec.presetOnlyRecipes?.includes(name)) {
                    label.title = "方案中保留此秘籍，当前木桩模拟尚未结算此效果。";
                }
                if (group === "recipes" && spec.singleTargetRecipeNotes?.[name]) label.title = spec.singleTargetRecipeNotes[name];
                label.append(input, document.createTextNode(name));
                $("schoolChoices").append(label);
            }
        }
    }

    showRaidDefaults();
    $("macro").value = drafts[school] ?? defaults?.[school] ?? "";
    syncEditor();
}
function pending(value) {
    busy = value;
    $("run").disabled = value || !defaults;
    $("debugOpen").disabled = value || !defaults;
    $("cancel").hidden = !value;
    $("progress").hidden = !value;
    document
        .querySelectorAll("#settings input,#settings select,#macro,#restore")
        .forEach((n) => (n.disabled = value));
}
function config() {
    if (!$("settings").reportValidity()) throw new Error("请检查模拟配置");
    if (BigInt($("seed").value) > 18446744073709551615n) throw new Error("随机种子超出 64 位范围");
    return {
        scenario: school,
        ...(schools.has(school) ? {config: {raid_full: $("raidFull").checked, permanents: selectedPermanents(), attributes: Object.fromEntries(panelInputs()
            .filter(input => input.value !== "").map(input => [input.dataset.stat, +input.value])),
            ...Object.fromEntries(["talents", "recipes", "equipment"].map(
            (group) => [group, [...$("schoolChoices").querySelectorAll("input:checked")].filter(
                (input) => input.dataset.group === group).map((input) => input.value)]))}} : {}),
        macro: $("macro").value,
        seconds: +$("seconds").value,
        iterations: +$("iterations").value,
        seed: $("seed").value,
    };
}
function manual(type) {
    try {
        message("");
        request = config();
        if (type === "debug") {
            const terms = $("breakpoints").value.trim();
            request.breakpoints = terms ? terms.split(/[,，]/).map((s) => Number(s.trim())) : [];
            if (
                request.breakpoints.some(
                    (n) => !Number.isInteger(n) || n < 1 || n > request.macro.split("\n").length,
                )
            )
                throw new Error("断点行号超出宏文本范围");
            debugButtons(false);
            $("debugState").textContent = "正在初始化…";
        } else {
            pending(true);
            $("progress").max = request.iterations;
            $("progress").value = 0;
            $("status").textContent = "模拟中";
        }
        worker.postMessage({ type, request });
    } catch (e) {
        message(e.message);
        if (type === "debug") $("debugState").textContent = e.message;
    }
}
function draw() {
    const canvas = $("dist"),
        ctx = canvas.getContext("2d");
    const { width, height } = canvas.getBoundingClientRect();
    if (!width || !height) return;
    const ratio = window.devicePixelRatio || 1;
    canvas.width = Math.round(width * ratio);
    canvas.height = Math.round(height * ratio);
    ctx.scale(ratio, ratio);
    ctx.fillStyle = "#396052";
    for (let i = 1; i <= 3; i++) ctx.fillRect(0, (i * (height - 1)) / 3, width, 1);
    if (!selected) return;
    const bins = selected.result.bins,
        max = Math.max(...bins, 1);
    const dx = width / bins.length;
    ctx.fillStyle = "#bbe88d";
    bins.forEach((n, i) => {
        if (n) {
            const h = Math.max(2, ((height - 5) * n) / max);
            ctx.fillRect(i * dx + 1, height - h, Math.max(dx - 2, 1), h);
        }
    });
}
function display(rec) {
    selected = rec;
    const r = rec.result,
        q = rec.request,
        cls = q.scenario;
    $("dps").textContent = fmt(r.meanDps, 1);
    $("low").textContent = fmt(Number(r.minDamage) / q.seconds, 1);
    $("high").textContent = fmt(Number(r.maxDamage) / q.seconds, 1);
    $("sd").textContent = fmt(r.sdDps, 1);
    $("resultMeta").textContent =
        names[cls] +
        " · " +
        "50 级" +
        " · " +
        fmt(r.completed) +
        " 场";
    $("checksum").textContent = r.checksum;
    $("elapsed").textContent = "耗时 " + fmt(rec.elapsed, 3) + " s";
    const list = [...r.skills].sort((a, b) => Number(BigInt(b.damage) - BigInt(a.damage)));
    const sum = list.reduce((a, s) => a + BigInt(s.damage), 0n);
    $("total").textContent = "总伤害 " + fmt(sum);
    $("skillCount").textContent = list.length + " 项技能";
    $("empty").hidden = list.length > 0;
    $("skills").replaceChildren(
        ...list.map((s, i) => {
            const tr = el("tr"),
                cell = el("td"),
                name = el("div", undefined, "skill");
            const icon = ["tx","mw"].includes(cls) && iconMap[cls === "tx" ? "0" : "1"]?.[s.name]?.Icon;
            if (icon) {
                const img = el("img");
                img.src = "./icons/" + Number(icon) + ".png";
                img.alt = "";
                img.onerror = () => img.remove();
                name.append(img);
            }
            name.append(el("span", s.name));
            cell.append(name);
            tr.append(cell);
            const pct = sum ? (Number(s.damage) / Number(sum)) * 100 : 0;
            const td = el("td"),
                share = el("div", undefined, "share"),
                track = el("div", undefined, "track"),
                fill = el("i", undefined, "fill");
            fill.style.width = pct + "%";
            fill.style.background = colors[Math.min(i, colors.length - 1)];
            track.append(fill);
            share.append(track, el("span", fmt(pct, 1) + "%"));
            td.append(share);
            tr.append(td, el("td", fmt(s.count)), el("td", fmt((100 * s.critical) / s.count, 1) + "%"));
            return tr;
        }),
    );
    draw();
}
function debugButtons(enable) {
    document.querySelectorAll("[data-step]").forEach((b) => (b.disabled = !enable));
}
function showDebug(s) {
    const source = request.macro.split("\n");
    $("debugSource").replaceChildren(
        ...source.map((line, i) =>
            el("span", i + 1 + "  " + line, i + 1 === s.line && !s.finished ? "active" : ""),
        ),
    );
    $("debugState").textContent =
        (s.finished ? "已结束" : s.breakpoint ? "已到断点" : "第 " + s.line + " 行") +
        " · " +
        s.frame +
        " 帧 · " +
        s.hits +
        " 次伤害" +
        (s.finished ? " · 总伤害 " + s.damage : "");
    debugButtons(!s.finished);
}
function boot() {
    defaults = null;
    pending(false);
    $("status").textContent = "内核加载中";
    worker = new Worker("./ui/worker.js", { type: "module" });
    const fail = (s) => {
        defaults = null;
        pending(false);
        debugButtons(false);
        $("status").textContent = "内核错误";
        message(s);
    };
    worker.onerror = (e) => fail(e.message || "无法加载 WASM，请检查站点是否已完整构建");
    worker.onmessage = ({ data }) => {
        switch (data.type) {
            case "ready":
                defaults = data.defaults;
                $("school").replaceChildren(...data.schools.map((spec) => {
                    schools.set(spec.code, spec);
                    names[spec.code] = spec.name;

                    return new Option(spec.name, spec.code);
                }));
                $("school").value = school;
                setSchool(school);
                syncEditor();
                pending(false);
                $("status").textContent = "WASM 内核就绪";
                if (auto) {
                    auto = false;
                    manual("run");
                }
                break;
            case "fatal":
                fail(data.error);
                break;
            case "error":
                pending(false);
                $("status").textContent = "WASM 内核就绪";
                message(data.error);
                $("debugState").textContent = data.error;
                debugButtons(false);
                break;
            case "progress":
                $("progress").value = data.completed;
                break;
            case "debug":
                showDebug(data.state);
                break;
            case "result": {
                pending(false);
                $("status").textContent = "WASM 内核就绪";
                const rec = {
                    result: data.result,
                    request,
                    elapsed: data.elapsed,
                    time: new Date().toLocaleTimeString("zh-CN", { hour12: false }),
                };
                records.unshift(rec);
                if (records.length > 12) records.pop();
                $("historyCount").textContent = records.length;
                display(rec);
                break;
            }
        }
    };
}

$("school").onchange = (event) => setSchool(event.target.value);
$("restore").onclick = () => {
    if (defaults) {
        $("macro").value = defaults[school];
        syncEditor();
    }
};
$("macro").oninput = syncEditor;
$("macro").onscroll = syncEditor;
$("settings").onsubmit = (e) => {
    e.preventDefault();
    if (!busy) manual("run");
};
$("cancel").onclick = () => {
    worker.terminate();
    auto = false;
    boot();
    message("已取消本次运行");
};
$("historyOpen").onclick = () => {
    $("records").replaceChildren(
        ...(records.length
            ? records.map((rec) => {
                  const b = el(
                      "button",
                      rec.time +
                          "　" +
                          names[rec.request.scenario] +
                          " · " +
                          fmt(rec.result.completed) +
                          " 场　" +
                          fmt(rec.result.meanDps, 1) +
                          " DPS",
                      "record",
                  );
                  b.onclick = () => {
                      display(rec);
                      $("history").close();
                  };
                  return b;
              })
            : [el("p", "暂无运行记录", "empty")]),
    );
    $("history").showModal();
};
$("debugOpen").onclick = () => {
    $("debug").showModal();
    manual("debug");
};
$("debugStart").onclick = () => manual("debug");
document.querySelectorAll("[data-step]").forEach(
    (b) =>
        (b.onclick = () => {
            debugButtons(false);
            worker.postMessage({ type: "step", action: b.dataset.step });
        }),
);
document.querySelectorAll("[data-close]").forEach((b) => (b.onclick = () => b.closest("dialog").close()));
fetch("./icons.json")
    .then((r) => r.json())
    .then((v) => {
        iconMap = v;
        if (selected) display(selected);
    })
    .catch(() => {});
new ResizeObserver(draw).observe($("dist"));
boot();
