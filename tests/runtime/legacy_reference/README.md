# Pinned arithmetic oracle

Source commit: `ba960f776dad2b30c8d43b529cd06d4fcaed5c2c`.

`Damage.hpp` and `Global/*` are byte-for-byte copies from that commit, preserving
their original headers. Tests compile them under `LegacyReference` without
linking the unfinished application or external libraries. `skill_parameters.inc`
extracts constructor expressions from the original skill/Buff files. Its values
are not generated from the new runtime. `SHA256.txt` records the source hashes.

To regenerate, verify the reference worktree with `git rev-parse HEAD` and
`git diff HEAD -- src`, then run:

```text
python tests/runtime/capture_legacy_reference.py PATH_TO_BA960F7_WORKTREE
```

The script rejects changed source hashes. On the first capture, the caller must
verify the commit before creating the manifest.

The oracle validates integer arithmetic, formula parameters and supported
modifiers. It is not a full old-engine timeline replay. Timing/proc tests use
explicit expectations from the old source; fast/debug differential tests share
the new rules. They cannot establish full-game or old-RNG-seed equivalence.
