#!/bin/bash

generate_changelog_for_tag() {
    local start_tag=$1
    local end_tag=$2
    local tag_date

    if [ "$end_tag" = "HEAD" ]; then
        tag_date=$(date +%Y-%m-%d)
        echo "## Unreleased ($tag_date)"
    else
        tag_date=$(git log -1 --format=%ad --date=short $end_tag)
        echo "## $end_tag ($tag_date)"
    fi
    echo

    local range
    if [ "$start_tag" = "START" ]; then
        range=$(git rev-list --max-parents=0 HEAD)..${end_tag}
    else
        range=${start_tag}..${end_tag}
    fi

    local commits=$(git log $range --pretty=format:"%s|@%an|%h" --reverse --no-merges)

    declare -A commit_map

    while IFS='|' read -r message author hash; do
        key="${message}|${author}"
        if [[ -v commit_map[$key] ]]; then
            commit_map[$key]="${commit_map[$key]} $hash"
        else
            commit_map[$key]=$hash
        fi
    done <<< "$commits"

    local featCommits=""
    local fixCommits=""
    local docsCommits=""
    local perfCommits=""
    local refactorCommits=""
    local testCommits=""
    local otherCommits=""

    for key in "${!commit_map[@]}"; do
        IFS='|' read -r message author <<< "$key"
        hashes=${commit_map[$key]}
        formatted_commit="- $message by $author in $hashes"
        
        case "$message" in
            feat:*) featCommits+="$formatted_commit"$'\n' ;;
            fix:*) fixCommits+="$formatted_commit"$'\n' ;;
            docs:*) docsCommits+="$formatted_commit"$'\n' ;;
            perf:*) perfCommits+="$formatted_commit"$'\n' ;;
            refactor:*) refactorCommits+="$formatted_commit"$'\n' ;;
            test:*) testCommits+="$formatted_commit"$'\n' ;;
            *) otherCommits+="$formatted_commit"$'\n' ;;
        esac
    done

    if [ -n "$featCommits" ]; then
        echo "### Features"
        echo -n "$featCommits"
        echo
    fi

    if [ -n "$fixCommits" ]; then
        echo "### Bug Fixes"
        echo -n "$fixCommits"
        echo
    fi

    if [ -n "$docsCommits" ]; then
        echo "### Documentation"
        echo -n "$docsCommits"
        echo
    fi

    if [ -n "$perfCommits" ]; then
        echo "### Performance Improvements"
        echo -n "$perfCommits"
        echo
    fi

    if [ -n "$refactorCommits" ]; then
        echo "### Refactoring"
        echo -n "$refactorCommits"
        echo
    fi

    if [ -n "$testCommits" ]; then
        echo "### Tests"
        echo -n "$testCommits"
        echo
    fi

    if [ -n "$otherCommits" ]; then
        echo "### Others"
        echo -n "$otherCommits"
        echo
    fi
}

{
    echo "# Changelog"
    echo

    # 获取所有 tag，按版本号排序（最新的在前）
    tags=$(git tag --sort=-version:refname)

    # 如果有未发布的更改，先生成 Unreleased 部分
    if [ "$(git rev-parse HEAD)" != "$(git rev-parse $(echo $tags | head -n1))" ]; then
        generate_changelog_for_tag "$(echo $tags | head -n1)" "HEAD"
    fi

    # 生成每个 tag 的 changelog
    prev_tag=""
    for tag in $tags
    do
        if [ -z "$prev_tag" ]; then
            generate_changelog_for_tag "START" "$tag"
        else
            generate_changelog_for_tag "$tag" "$prev_tag"
        fi
        prev_tag=$tag
    done

} > CHANGELOG.md

# 如果 changelog 不为空，则添加到暂存区
if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
fi