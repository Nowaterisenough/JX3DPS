#!/bin/bash

generate_changelog_between_tags() {
    local start_tag=$1
    local end_tag=$2
    local range

    if [ "$start_tag" = "START" ]; then
        range=$(git rev-list --max-parents=0 HEAD)..${end_tag}
    else
        range=${start_tag}..${end_tag}
    fi

    local commits=$(git log $range --pretty=format:"- %s by @%an in %h" --reverse --no-merges)

    local featCommits=$(echo "$commits" | grep '^- feat' | sed 's/^- feat: \?/- /')
    local fixCommits=$(echo "$commits" | grep '^- fix' | sed 's/^- fix: \?/- /')
    local docsCommits=$(echo "$commits" | grep '^- docs' | sed 's/^- docs: \?/- /')
    local perfCommits=$(echo "$commits" | grep '^- perf' | sed 's/^- perf: \?/- /')
    local refactorCommits=$(echo "$commits" | grep '^- refactor' | sed 's/^- refactor: \?/- /')
    local testCommits=$(echo "$commits" | grep '^- test' | sed 's/^- test: \?/- /')
    local otherCommits=$(echo "$commits" | grep -v '^- \(feat\|fix\|docs\|perf\|refactor\|test\)' | sed 's/^- [^:]*: \?/- /')

    echo "## ${end_tag} ($(git log -1 --format=%ad --date=short ${end_tag}))"
    echo

    if [ -n "$featCommits" ]; then
        echo "### Features"
        echo "$featCommits"
        echo
    fi

    if [ -n "$fixCommits" ]; then
        echo "### Bug Fixes"
        echo "$fixCommits"
        echo
    fi

    if [ -n "$docsCommits" ]; then
        echo "### Documentation"
        echo "$docsCommits"
        echo
    fi

    if [ -n "$perfCommits" ]; then
        echo "### Performance Improvements"
        echo "$perfCommits"
        echo
    fi

    if [ -n "$refactorCommits" ]; then
        echo "### Refactoring"
        echo "$refactorCommits"
        echo
    fi

    if [ -n "$testCommits" ]; then
        echo "### Tests"
        echo "$testCommits"
        echo
    fi

    if [ -n "$otherCommits" ]; then
        echo "### Others"
        echo "$otherCommits"
        echo
    fi
}

{
    echo "# Changelog"
    echo

    # 获取所有 tag，按版本号排序
    tags=$(git tag --sort=-version:refname)

    # 如果没有 tag，使用 HEAD
    if [ -z "$tags" ]; then
        tags="HEAD"
    fi

    prev_tag="START"
    for tag in $tags
    do
        generate_changelog_between_tags $prev_tag $tag
        prev_tag=$tag
    done

    # 如果最新的 tag 不是 HEAD，添加未发布的更改
    if [ "$(git rev-parse ${prev_tag})" != "$(git rev-parse HEAD)" ]; then
        echo "## Unreleased ($(date +%Y-%m-%d))"
        echo
        generate_changelog_between_tags $prev_tag HEAD
    fi
} > CHANGELOG.md

# 如果 changelog 不为空，则添加到暂存区
if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
fi