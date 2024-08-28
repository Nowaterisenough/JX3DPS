#!/bin/bash

# 设置 Git 配置以避免行尾符警告
git config --local core.autocrlf input

generate_changelog_for_tag() {
    local current_tag=$1
    local prev_tag=$2
    local tag_date=$(git log -1 --format=%ad --date=short $current_tag)

    echo "## $current_tag ($tag_date)"
    echo

    local range
    if [ -z "$prev_tag" ]; then
        range="$current_tag"
    else
        range="$prev_tag..$current_tag"
    fi

    echo "正在处理范围: $range" >&2

    local commits=$(git log $range --pretty=format:"%at|%s|@%an|%h" --reverse --no-merges)

    process_commits "$commits"
}

process_commits() {
    local commits="$1"
    
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
            eval "${array_ref}[$key]='$timestamp $hash'"
        fi
    done <<< "$commits"

    format_commits feat_commits "Features"
    format_commits fix_commits "Bug Fixes"
    format_commits docs_commits "Documentation"
    format_commits perf_commits "Performance Improvements"
    format_commits refactor_commits "Refactoring"
    format_commits test_commits "Tests"
    format_commits other_commits "Others"
}

format_commits() {
    local -n commit_array=$1
    local title=$2
    local formatted=""
    
    local sorted_keys=($(
        for key in "${!commit_array[@]}"; do
            IFS=' ' read -r timestamp hash <<< "${commit_array[$key]}"
            echo "$timestamp $key"
        done | sort -rn | cut -d' ' -f2-
    ))
    
    for key in "${sorted_keys[@]}"; do
        IFS='|' read -r message author <<< "$key"
        hashes=${commit_array[$key]}
        IFS=' ' read -r timestamp hashes <<< "$hashes"
        if [ -n "$message" ] && [ -n "$author" ] && [ -n "$hashes" ]; then
            formatted+="- $message by $author in $hashes"$'\n'
        fi
    done
    
    if [ -n "$formatted" ]; then
        echo "### $title"
        echo -n "$formatted"
        echo
    fi
}

{
    echo "# CHANGELOG"
    echo

    # 按创建日期排序标签
    tags=($(git tag --sort=-creatordate))
    latest_tag="${tags[0]}"
    
    # 生成未发布的更改
    unreleased_commits=$(git log $latest_tag..HEAD --pretty=format:"%at|%s|@%an|%h" --reverse --no-merges)
    
    if [ -n "$unreleased_commits" ]; then
        echo "## Unreleased ($(date +%Y-%m-%d))"
        echo
        process_commits "$unreleased_commits"
    fi

    # 生成所有标签的更改
    for ((i=0; i<${#tags[@]}; i++)); do
        current_tag="${tags[i]}"
        next_tag="${tags[i+1]}"
        generate_changelog_for_tag "$current_tag" "$next_tag"
    done

} > CHANGELOG.md

if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
fi

# 重置 Git 配置
git config --local --unset core.autocrlf