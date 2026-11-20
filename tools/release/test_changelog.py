import os
from pathlib import Path
import subprocess
import tempfile
import unittest

from changelog import changes, previous_release, render


class ReleaseNotesTest(unittest.TestCase):
    def test_published_ancestor_and_grouping(self):
        cwd = Path.cwd()
        with tempfile.TemporaryDirectory() as temp:
            os.chdir(temp)
            try:
                def git(*args):
                    return subprocess.check_output(["git", *args], encoding="utf-8").strip()

                def commit(message):
                    git("commit", "--allow-empty", "-m", message)
                    return git("rev-parse", "HEAD")

                git("init", "-q", "-b", "master")
                git("config", "user.email", "dev@example.org")
                git("config", "user.name", "Local Author")
                commit("first")
                git("tag", "-a", "v1", "-m", "first release")
                git("checkout", "-qb", "feature")
                first = commit("fix(core): 修复输出")
                git("tag", "v2-rc")
                git("tag", "draft")
                git("tag", "tag-only")
                second = commit("fix(core): 修复输出")
                third = commit("feat(core)!: 增加会话")
                git("checkout", "-q", "master")
                commit("ordinary [change]")
                git("merge", "--no-ff", "feature", "-m", "merge feature")
                head = git("rev-parse", "HEAD")
                git("tag", "v2")
                releases = [dict(tag_name=n, prerelease=n=="v2-rc", draft=n=="draft")
                            for n in ("v2", "draft", "v2-rc", "v1")]
                base = previous_release(head, "v2", releases)
                self.assertEqual(base, "v1")
                commits = changes(head, base)
                self.assertEqual(len(commits), 4)
                body = render("owner/repo", "v2", base, commits, {first: "one", second: "two"})
                self.assertIn("### Bug Fixes（2）", body)
                self.assertIn("### Features（1）", body)
                self.assertEqual(body.count("修复输出"), 1)
                fix = next(line for line in body.splitlines() if "修复输出" in line)
                for value in (first, second, "@one", "@two"):
                    self.assertIn(value, fix)
                self.assertIn(third, body)
                self.assertIn("破坏性变更", body)
                self.assertIn(r"ordinary \[change\] by Local Author", body)
                self.assertNotIn("merge feature", body)
                self.assertIsNone(previous_release(head, "first", []))
                self.assertEqual(len(changes(head, None)), 5)
                self.assertEqual(changes(head, "v2"), [])
            finally:
                os.chdir(cwd)


if __name__ == "__main__":
    unittest.main()
