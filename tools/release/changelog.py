"""Build release notes from published release tags and the actual Git commit range."""
import argparse
import json
import os
import re
import subprocess
from pathlib import Path
from urllib.parse import quote
from urllib.request import Request, urlopen

CATEGORIES = {
    "Features": {"feat", "feature"},
    "Bug Fixes": {"fix", "bugfix", "revert"},
    "Performance": {"perf"},
    "Documentation": {"docs", "doc"},
    "Build System": {"build", "ci"},
    "Refactoring": {"refactor"},
    "Tests": {"test", "tests"},
    "Others": set(),
}
CONVENTIONAL = re.compile(r"^([a-zA-Z]+)(?:\(([^()]+)\))?(!)?:\s*(.+)$")


def git(*args):
    return subprocess.check_output(["git", *args]).decode("utf-8", "replace")


def github(path, payload=None):
    request = Request("https://api.github.com/" + path,
                      data=json.dumps(payload).encode() if payload else None,
                      headers={"Authorization": "Bearer "+os.environ["GH_TOKEN"],
                               "Accept": "application/vnd.github+json", "Content-Type": "application/json",
                               "User-Agent": "JX3DPS-release"})
    with urlopen(request, timeout=30) as response:
        return json.load(response)


def published_releases(repo):
    page = 1
    while True:
        items = github(f"repos/{repo}/releases?per_page=100&page={page}")
        yield from items
        if len(items) < 100:
            return
        page += 1


def previous_release(head, tag, releases):
    releases = list(releases)
    published_at = next((r.get("published_at") for r in releases if r["tag_name"] == tag), None)
    candidates = {}
    for r in releases:  # GitHub lists the newest releases first.
        name = r["tag_name"]
        if name == tag or r["draft"] or r["prerelease"]:
            continue
        # A rerun of an existing release must not pick a subsequently published tag on the same commit.
        if published_at and r["published_at"] > published_at:
            continue
        rev = subprocess.run(["git", "rev-parse", "--verify", "--quiet",
                              f"refs/tags/{name}^{{commit}}"], capture_output=True, text=True)
        if rev.returncode == 0:
            candidates.setdefault(rev.stdout.strip(), name)
    # Prefer releases on this release line; other merged branches are a fallback.
    for order in ("--first-parent", "--topo-order"):
        for sha in git("rev-list", order, head).splitlines():
            if sha in candidates:
                return candidates[sha]
    return None


def published_benchmarks(repo, release):
    if release is None:
        return None
    asset = next((a for a in release["assets"] if a["name"] == "performance.json"), None)
    if asset is None:
        return None
    data = subprocess.check_output(["gh", "api", "-H", "Accept: application/octet-stream",
                                    f"repos/{repo}/releases/assets/{asset['id']}"])
    reports = json.loads(data)
    sha = git("rev-parse", f"refs/tags/{release['tag_name']}^{{commit}}").strip()
    if not reports or any(r["sha"] != sha for r in reports):
        raise ValueError(f"performance.json does not match release {release['tag_name']}")
    return reports


def changes(head, base):
    revision = f"refs/tags/{base}..{head}" if base else head
    fields = git("log", "--no-merges", "--topo-order", "--reverse", "-z",
                 "--format=%H%x00%an%x00%s", revision).split("\0")
    return list(zip(fields[0::3], fields[1::3], fields[2::3]))


def author_logins(repo, commits):
    owner, name = repo.split("/")
    result = {}
    for start in range(0, len(commits), 50):
        chunk = commits[start:start+50]
        objects = " ".join(f'c{i}:object(oid:"{sha}"){{... on Commit{{author{{user{{login}}}}}}}}'
                           for i, (sha, _, _) in enumerate(chunk))
        query = "query($owner:String!,$name:String!){repository(owner:$owner,name:$name){" + objects + "}}"
        data = github("graphql", {"query": query, "variables": {"owner": owner, "name": name}})
        for i, (sha, _, _) in enumerate(chunk):
            node = data["data"]["repository"].get(f"c{i}") or {}
            result[sha] = ((node.get("author") or {}).get("user") or {}).get("login")
    return result


def escape(text):
    text = text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    return re.sub(r"([\\`*_\[\]~|])", r"\\\1", text)


def render(repo, tag, base, commits, logins):
    url = "https://github.com/" + repo
    groups = {category: {} for category in CATEGORIES}
    for sha, author, subject in commits:
        m = CONVENTIONAL.match(subject)
        kind, scope, breaking, title = m.groups() if m else ("", None, None, subject)
        category = next((c for c, kinds in CATEGORIES.items() if kind.lower() in kinds), "Others")
        item = groups[category].setdefault((scope, breaking, title), ([], []))
        login = logins.get(sha)
        credit = f"[@{escape(login)}](https://github.com/{quote(login, safe='')})" if login else escape(author)
        if credit not in item[0]:
            item[0].append(credit)
        item[1].append(f"[`{sha[:7]}`]({url}/commit/{sha})")
    lines = ["## CHANGELOG", "", f"共 {len(commits)} 个非合并提交。"]
    if base:
        lines[-1] += f" [完整差异 · {escape(base)} → {escape(tag)}]({url}/compare/{quote(base,safe='')}...{quote(tag,safe='')})"
    for category, entries in groups.items():
        if entries:
            count = sum(len(shas) for _, shas in entries.values())
            lines += ["", f"### {category}（{count}）", ""]
            for (scope, breaking, title), (authors, shas) in entries.items():
                prefix = ("**破坏性变更** " if breaking else "") + (f"**{escape(scope)}** · " if scope else "")
                lines.append(f"- {prefix}{escape(title)} by {', '.join(authors)} in {' '.join(shas)}")
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    for arg in ("repo", "sha", "tag", "output"):
        parser.add_argument("--"+arg, required=True)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--performance", type=Path, help="Append an existing performance report")
    source.add_argument("--benchmarks", type=Path, help="Build the performance report from bench-*.json in this directory")
    args = parser.parse_args()
    head = git("rev-parse", "--verify", args.sha+"^{commit}").strip()
    releases = list(published_releases(args.repo))
    base = previous_release(head, args.tag, releases)
    performance = args.performance
    if args.benchmarks:
        import chart

        previous = next((r for r in releases if r["tag_name"] == base), None)
        reports = [json.loads(f.read_bytes()) for f in args.benchmarks.glob("bench-*.json")]
        chart.render(reports, args.benchmarks, args.sha, args.repo, args.tag,
                     previous_tag=base, previous_reports=published_benchmarks(args.repo, previous))
        performance = args.benchmarks / "performance.md"
    commits = changes(head, base)
    notes = render(args.repo, args.tag, base, commits, author_logins(args.repo, commits))
    Path(args.output).write_text(notes+"\n"+performance.read_text(encoding="utf-8"), encoding="utf-8")
