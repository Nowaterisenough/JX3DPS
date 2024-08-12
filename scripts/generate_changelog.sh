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

    declare -A feat_commits fix_commits docs_commits perf_commits refactor_commits test_commits other_commits

    while IFS='|' read -r message author hash; do
        if [[ $message =~ ^(feat|fix|docs|perf|refactor|test): ]]; then
            type=${BASH_REMATCH[1]}
            clean_message=${message#$type: }
        else
            type="other"
            clean_message=$message
        fi
        
        key="${clean_message}|${author}"
        case $type in
            feat)     array_ref="feat_commits"     ;;
            fix)      array_ref="fix_commits"      ;;
            docs)     array_ref="docs_commits"     ;;
            perf)     array_ref="perf_commits"     ;;
            refactor) array_ref="refactor_commits" ;;
            test)     array_ref="test_commits"     ;;
            *)        array_ref="other_commits"    ;;
        esac
        
        if [[ -v ${array_ref}[$key] ]]; then
            eval "${array_ref}[$key]+=' $hash'"
        else
            eval "${array_ref}[$key]='$hash'"
        fi
    done <<< "$commits"

    format_commits() {
        local -n commit_array=$1
        local title=$2
        local formatted=""
        for key in "${!commit_array[@]}"; do
            IFS='|' read -r message author <<< "$key"
            hashes=${commit_array[$key]}
            formatted+="- $message by $author in $hashes"$'\n'
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

    if [ "$(git rev-parse HEAD)" != "$(git rev-parse $(echo $tags | head -n1))" ]; then
        generate_changelog_for_tag "$(echo $tags | head -n1)" "HEAD"
    fi

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

if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
fi