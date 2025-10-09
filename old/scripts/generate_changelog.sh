#!/bin/bash

set -e  # 遇到错误时退出

# 设置 Git 配置以避免行尾符警告
git config --local core.autocrlf input

# 获取最新提交的哈希和日期
latest_commit=$(git rev-parse HEAD)
latest_commit_date=$(git log -1 --format=%ad --date=short)

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

    local commits=$(git log $range --pretty=format:"%at|%s|@%an|%h" --reverse --no-merges)

    if [ -z "$commits" ]; then
        echo "No changes in this version."
        echo
    else
        process_commits "$commits"
    fi
}

process_commits() {
    local commits="$1"
    
    local -a feat_commits fix_commits docs_commits perf_commits refactor_commits test_commits build_commits other_commits

    while IFS='|' read -r timestamp message author hash; do
        if [ -z "$message" ] || [ -z "$author" ] || [ -z "$hash" ]; then
            continue
        fi

        if [[ $message =~ ^(feat|fix|docs|perf|refactor|test|build)([^:]*): ]]; then
            type=${BASH_REMATCH[1]}
            clean_message=${message#${BASH_REMATCH[0]}}
            clean_message=$(echo "$clean_message" | sed -e 's/^[[:space:]]*//')
        else
            type="other"
            clean_message=$(echo "$message" | sed -E 's/^(ci|chore|style|revert)([^:]*):\s*//')
        fi
        
        key="${timestamp}|${clean_message}|${author}|${hash}"
        case $type in
            feat)     feat_commits+=("$key")     ;;
            fix)      fix_commits+=("$key")      ;;
            docs)     docs_commits+=("$key")     ;;
            perf)     perf_commits+=("$key")     ;;
            refactor) refactor_commits+=("$key") ;;
            test)     test_commits+=("$key")     ;;
            build)    build_commits+=("$key")    ;;
            *)        other_commits+=("$key")    ;;
        esac
    done <<< "$commits"

    format_commits "Features" "${feat_commits[@]+"${feat_commits[@]}"}"
    format_commits "Bug Fixes" "${fix_commits[@]+"${fix_commits[@]}"}"
    format_commits "Documentation" "${docs_commits[@]+"${docs_commits[@]}"}"
    format_commits "Performance Improvements" "${perf_commits[@]+"${perf_commits[@]}"}"
    format_commits "Refactor" "${refactor_commits[@]+"${refactor_commits[@]}"}"
    format_commits "Tests" "${test_commits[@]+"${test_commits[@]}"}"
    format_commits "Build System" "${build_commits[@]+"${build_commits[@]}"}"
    format_commits "Others" "${other_commits[@]+"${other_commits[@]}"}"
}

format_commits() {
    local title=$1
    shift
    local commits=("$@")
    
    # 如果数组为空，直接返回
    if [ ${#commits[@]} -eq 0 ]; then
        return
    fi
    
    echo "### $title"
    
    # 按时间戳排序（升序）
    IFS=$'\n' sorted=($(sort -n <<<"${commits[*]}"))
    unset IFS
    
    declare -A merged_commits
    
    for commit in "${sorted[@]}"; do
        IFS='|' read -r timestamp message author hash <<< "$commit"
        key="${message}|${author}"
        if [[ -v merged_commits[$key] ]]; then
            merged_commits[$key]+=" $hash"
        else
            merged_commits[$key]="$timestamp|$hash"
        fi
    done
    
    # 按时间戳排序合并后的提交（升序）
    IFS=$'\n' sorted_keys=($(
        for key in "${!merged_commits[@]}"; do
            echo "${merged_commits[$key]}|$key"
        done | sort -n
    ))
    unset IFS
    
    for entry in "${sorted_keys[@]}"; do
        IFS='|' read -r timestamp hashes message author <<< "$entry"
        # 反转哈希顺序，使最新的哈希在前面
        hashes_reversed=$(echo $hashes | tr ' ' '\n' | tac | tr '\n' ' ' | sed 's/ $//')
        echo "- $message by $author in $hashes_reversed"
    done
    echo
}

{
    echo "# CHANGELOG"
    echo

    # 按创建日期排序标签
    tags=($(git tag --sort=-creatordate))
    latest_tag="${tags[0]}"
    
    # 生成未发布的更改
    unreleased_commits=$(git log $latest_tag..$latest_commit --pretty=format:"%at|%s|@%an|%h" --reverse --no-merges)
    
    if [ -n "$unreleased_commits" ]; then
        echo "## Unreleased ($latest_commit_date)"
        echo
        process_commits "$unreleased_commits"
    fi

    # 生成所有标签的更改
    for ((i=0; i<${#tags[@]}; i++)); do
        current_tag="${tags[i]}"
        next_tag="${tags[i+1]}"
        generate_changelog_for_tag "$current_tag" "$next_tag"
    done

    # 如果没有生成任何内容，添加一条提示信息
    if [ $(wc -l < CHANGELOG.md) -le 3 ]; then
        echo "No changes found in the repository."
    fi

} > CHANGELOG.md

if [ -s CHANGELOG.md ]; then
    git add CHANGELOG.md
else
    echo "Error: CHANGELOG.md is empty" >&2
    exit 1
fi

# 重置 Git 配置
git config --local --unset core.autocrlf