#!/usr/bin/env python3
"""Capture public JX3BOX sources separately from executable combat rules."""

import argparse
import concurrent.futures
import datetime
import hashlib
import html
import json
from pathlib import Path
import re
import sys
import urllib.error
import urllib.parse
import urllib.request


ROOT = Path(__file__).resolve().parents[1]
TALENT_INDEX = "https://data.jx3box.com/talent/std/index.json"
CHANGE_INDEX = "https://cms.jx3box.com/api/cms/pve/skill/changelog?client=std"
LUA_INDEX = "https://lua.jx3box.com/api/lua/bps/index.json?client=std"
PROFILES = (
    {"key": "0", "name": "\u592a\u865a\u5251\u610f", "force": 4, "kungfu": "4", "mount": 10015},
    {"key": "1", "name": "\u83ab\u95ee", "force": 22, "kungfu": "22", "mount": 10447},
)


def plain_text(value):
    return re.sub(r"\s+", " ", html.unescape(re.sub(r"<[^>]+>", " ", value))).strip()


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    data = (json.dumps(value, ensure_ascii=False, indent=2) + "\n").encode("utf-8")
    path.write_bytes(data)


class SourceCache:
    def __init__(self, directory, offline=False):
        self.directory = directory
        self.offline = offline

    def load(self, key, url):
        path = self.directory / (key + ".json")
        metadata_path = self.directory / (key + ".source.json")
        if self.offline:
            data = path.read_bytes()
            source = json.loads(metadata_path.read_bytes())
            if source["url"] != url or source["sha256"] != hashlib.sha256(data).hexdigest():
                raise ValueError("Cached source URL or checksum mismatch: " + key)
        else:
            request = urllib.request.Request(url, headers={"User-Agent": "JX3DPS-source-sync/1.0"})
            with urllib.request.urlopen(request, timeout=40) as response:
                data = response.read()
                source = {
                    "url": url,
                    "sha256": hashlib.sha256(data).hexdigest(),
                    "retrieved_at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
                    "last_modified": response.headers.get("Last-Modified"),
                    "etag": response.headers.get("ETag"),
                }
            json.loads(data)
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
            write_json(metadata_path, source)
        return json.loads(data), source

    def probe_lua(self):
        path = self.directory / "lua_access.json"
        if self.offline:
            return json.loads(path.read_bytes())
        try:
            _, source = self.load("lua_index", LUA_INDEX)
            result = {"url": LUA_INDEX, "http_status": 200, "available": True, "source": source}
        except urllib.error.HTTPError as error:
            if error.code not in (401, 403):
                raise
            result = {"url": LUA_INDEX, "http_status": error.code, "available": False,
                      "reason": "Authentication or account access is required for original skill scripts."}
        write_json(path, result)
        return result


def normalize_talents(raw, version, title, checked_at):
    result = {"schema": 1, "version": version, "title": title,
              "source": "https://data.jx3box.com/talent/std/" + version + ".json",
              "checked_at": checked_at, "profiles": {}}
    for profile in PROFILES:
        tiers = raw[profile["name"]]
        if set(tiers) != {str(i) for i in range(1, 10)}:
            raise ValueError("Unexpected talent tier layout: " + profile["name"])
        groups = []
        seen = set()
        mixed_ids = None
        for index in range(1, 10):
            tier = tiers[str(index)]
            if index >= 8:
                if tier.get("_follow") != "7" or any(not key.startswith("_") for key in tier):
                    raise ValueError("The last three tiers no longer share a talent pool.")
                continue
            entries = [value for key, value in tier.items() if not key.startswith("_")]
            entries.sort(key=lambda value: int(value["pos"]))
            choices = [{key: value[key] for key in ("id", "name", "icon", "desc", "color")} for value in entries]
            ids = [str(choice["id"]) for choice in choices]
            expected_size = 4 if index <= 6 else 12
            if len(ids) != expected_size or len(set(ids)) != len(ids) or any(not i.isdecimal() for i in ids):
                raise ValueError("Invalid talent choices: " + profile["name"])
            if index <= 6:
                if seen.intersection(ids):
                    raise ValueError("Duplicate regular talent identity: " + profile["name"])
                seen.update(ids)
            else:
                if mixed_ids is None:
                    if seen.intersection(ids):
                        raise ValueError("Regular and mixed talents overlap: " + profile["name"])
                    mixed_ids = ids
                elif ids != mixed_ids:
                    raise ValueError("The last three tiers no longer share a talent pool.")
                if index != 7:
                    continue
            groups.append({"kind": "core" if index == 1 else "mixed" if index == 7 else "regular",
                           "choices": choices})
        result["profiles"][profile["key"]] = {"name": profile["name"],
            "slot_groups": [0, 1, 2, 3, 4, 5, 6, 6, 6], "groups": groups}
    return result


def select_skills(raw, profile):
    roots = [value for value in raw if str(value.get("kungfuId")) == profile["kungfu"]]
    if len(roots) != 1:
        raise ValueError("Missing or ambiguous kungfu skills: " + profile["name"])
    result = {}
    for group in roots[0]["remarks"]:
        for skill in group["forceSkills"]:
            if profile["kungfu"] not in map(str, skill.get("kungfuIds", [])):
                continue
            key = str(skill["_id"])
            if not key.isdecimal():
                raise ValueError("Invalid game skill identity")
            result[key] = skill
    if not result:
        raise ValueError("No skills for kungfu: " + profile["name"])
    return list(result.values())


def fetch_snapshot(cache):
    versions, talent_index_source = cache.load("talent_index", TALENT_INDEX)
    versions = [row for row in versions if re.fullmatch(r"v\d{8}", row["version"])]
    if not versions:
        raise ValueError("No valid standard-client talent versions")
    latest = max(versions, key=lambda row: row["version"])
    version = latest["version"]
    raw_talents, talent_source = cache.load("talents_" + version,
        "https://data.jx3box.com/talent/std/" + version + ".json")
    talent_catalog = normalize_talents(raw_talents, version, latest["name"], talent_source["retrieved_at"][:10])
    changes, change_index_source = cache.load("changelog_index", CHANGE_INDEX)
    if changes.get("code") != 0:
        raise ValueError("Changelog index returned an error")
    recent = sorted((row for row in changes["data"]["list"] if row["client"] == "std"),
                    key=lambda row: (row["date"], row["id"]), reverse=True)[:4]

    jobs = []
    for profile in PROFILES:
        jobs.append(("skills_" + profile["key"], "https://data.jx3box.com/bps/std/" + str(profile["force"]) + "/skill.json"))
        jobs.append(("panel_" + profile["key"], "https://node.jx3box.com/api/node/v2/kungfu/panel?client=std&mount_id=" + str(profile["mount"])))
    for change in recent:
        jobs.append(("changelog_" + str(change["id"]),
                     "https://cms.jx3box.com/api/cms/pve/skill/changelog/" + str(change["id"])))
    # Public metadata is useful for dependency checks, but is not a script export.
    talent_ids = [choice["id"] for profile in talent_catalog["profiles"].values()
                  for group in profile["groups"] for choice in group["choices"]]
    for index in range(0, len(talent_ids), 24):
        jobs.append(("talent_skills_" + str(index // 24),
                     "https://node.jx3box.com/skills?client=std&ids=" + ",".join(talent_ids[index:index + 24])))
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
        values = list(pool.map(lambda job: cache.load(*job), jobs))
    loaded = dict(zip((key for key, _ in jobs), values))
    sources = {"talent_index": talent_index_source, "talents": talent_source, "changelog_index": change_index_source}
    sources.update({key: source for key, (_, source) in loaded.items()})

    profiles = {}
    for profile in PROFILES:
        panel = loaded["panel_" + profile["key"]][0]
        if panel.get("code") != 0:
            raise ValueError("Skill panel returned an error: " + profile["name"])
        profiles[profile["key"]] = {"name": profile["name"], "mount_id": profile["mount"],
            "kungfu_id": profile["kungfu"], "skills": select_skills(loaded["skills_" + profile["key"]][0], profile),
            "panel": panel["data"], "talents": talent_catalog["profiles"][profile["key"]]}
    changelogs = []
    official_jobs = {}
    for row in recent:
        change = loaded["changelog_" + str(row["id"])][0]
        if change.get("code") != 0 or change["data"]["id"] != row["id"]:
            raise ValueError("Changelog identity mismatch")
        change = change["data"]
        changelogs.append({**change, "text": plain_text(change["content"]),
                          "applied_to_runtime": False})
        # Keep official details linked by the announcement alongside its summary.
        for href in re.findall(r'href="([^"]+)"', change["content"]):
            href = html.unescape(href)
            if urllib.parse.urlparse(href).hostname != "jx3.xoyo.com":
                continue
            query = urllib.parse.parse_qs(href.split("?", 1)[-1])
            article = query.get("id", [""])[0]
            category = query.get("catid", [""])[0]
            if (article, category) in {("7480", "2466"), ("7488", "2466")}:
                official_jobs["official_" + article] = (
                    "https://jx3.xoyo.com/api.php?op=search_api&action=get_article_detail&catid=" + category
                    + "&id=" + article + "&game=jx3")
    official_details = []
    for key, url in official_jobs.items():
        response, source = cache.load(key, url)
        articles = response.get("data")
        if not isinstance(articles, list) or len(articles) != 1:
            raise ValueError("Missing official school announcement")
        sources[key] = source
        official_details.append({**articles[0], "text": plain_text(articles[0]["content"]),
                                 "applied_to_runtime": False})
    lua_access = cache.probe_lua()
    snapshot = {"schema_version": 1, "client": "std", "talent_version": version,
        "talent_release": latest["name"], "sources": sources, "profiles": profiles,
        "talent_skill_metadata": [record for key, (records, _) in loaded.items()
                                  if key.startswith("talent_skills_") for record in records],
        "changelogs": changelogs, "official_school_details": official_details,
        "script_access": lua_access, "runtime_ready": False,
        "runtime_blockers": ["Select the live or test ruleset explicitly.",
            "Obtain level-specific original skill scripts, coefficients, buff data and attribute scaling constants.",
            "The existing runtime is pinned to level 120; public descriptions do not validate a replacement."]}
    return snapshot, talent_catalog


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "data/jx3box/runtime_catalog_latest.json")
    parser.add_argument("--cache-dir", type=Path, default=ROOT / "data/jx3box/runtime_sources")
    parser.add_argument("--offline", action="store_true", help="Validate cached URLs and hashes without network access")
    parser.add_argument("--talent-output", type=Path, help="Also regenerate the desktop talent display catalog")
    args = parser.parse_args()
    try:
        snapshot, talents = fetch_snapshot(SourceCache(args.cache_dir, args.offline))
        write_json(args.output, snapshot)
        if args.talent_output:
            write_json(args.talent_output, talents)
        print("Captured " + snapshot["talent_version"] + "; latest announcement: " + str(snapshot["changelogs"][0]["id"]))
        print("Original skill scripts available: " + str(snapshot["script_access"]["available"]))
        print("Combat rules were not changed by source synchronization.")
    except (OSError, ValueError, KeyError, TypeError) as error:
        print("Source synchronization failed: " + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
