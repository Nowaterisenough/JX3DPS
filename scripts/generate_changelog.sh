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

    local range="${start_tag}..${end_tag}"
    local commits=$(git log $range --pretty=format:"%at|%s|@%an|%h" --reverse --no-merges)

    declare -A feat_commits fix_commits docs_commits perf_commits refactor_commits test_commits other_commits

    while IFS='|' read -r timestamp message author hash; do
        if [ -z "$message" ] || [ -z "$author" ] || [ -z "$hash" ]; then
            continue
        fi

        if [[ $message =~ ^(feat|fix|docs|perf|refactor|test): ]]; then
            type=${BASH_REMATCH[1]}
            clean_message=${message#$type: }
        else
            type="other"
            clean_message=$(echo "$message" | sed -E 's/^(ci|chore|style|build|revert):\s*//')
        fi
        
        key="${timestamp}|${clean_message}|${author}"
        case $type in
            feat)     array_ref="feat_commits"     ;;
            fix)      array_ref="fix_commits"      ;;
            docs)     array_ref="docs_commits"     ;;
            perf)     array_ref="perf_commits"     ;;
            refactor) array_ref="refactor_commits" ;;
            test)     array_ref="test_commits"     ;;
            *)        array_ref="other_commits"    ;;
        esac
        
        eval "${array_ref}[\$key]='$hash'"
    done <<< "$commits"

    format_commits() {
        local -n commit_array=$1
        local title=$2
        local formatted=""
        
        local sorted_keys=($(
            for key in "${!commit_array[@]}"; do
                echo "$key"
            done | sort -n
        ))
        
        for key in "${sorted_keys[@]}"; do
            IFS='|' read -r timestamp message author <<< "$key"
            hash=${commit_array[$key]}
            if [ -n "$message" ] && [ -n "$author" ] && [ -n "$hash" ]; then
                formatted+="- $message by $author in $hash"$'\n'
            fi
        done
        
        if [ -n "$formatted" ]; then
            echo "### $title"
            echo -n "$formatted"
            echo
        fi
    }

    format_commits feat_commits "Features"
    format_commits fix_commits "Bug Fixes"
    format_commits docs_commits "Documentation"
    format_commits perf_commits "Performance Improvements"
    format_commits refactor_commits "Refactoring"
    format_commits test_commits "Tests"
    format_commits other_commits "Others"
}

{
    echo "# Changelog"
    echo

    tags=$(git tag --sort=-version:refname)
    
    # 生成未发布的更改
    if [ "$(git rev-parse HEAD)" != "$(git rev-parse $(echo $tags | head -n1))" ]; then
        generate_changelog_for_tag "$(echo $tags | head -n1)" "HEAD"
    fi

    # 生成所有标签的更改
    prev_tag=""
    for tag in $tags
    do
        if [ -z "$prev_tag" ]; then
            generate_changelog_for_tag "$(git rev-list --max-parents=0 HEAD)" "$tag"
        else
            generate_changelog_for_tag "$tag" "$prev_tag"
        fi
        prev_tag=$tag
    done

} > CHANGELOG.md

if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
fi