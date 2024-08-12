#!/bin/bash

# 获取最新的标签
latestTag=$(git describe --tags --abbrev=0 2>/dev/null)

# 如果没有标签，使用初始提交
if [ -z "$latestTag" ]; then
  range=$(git rev-list --max-parents=0 HEAD)
else
  range="${latestTag}..HEAD"
fi

# 使用 Git 命令获取提交记录，排除合并提交，包括作者信息
commits=$(git log $range --pretty=format:"- %s by @%an in %h" --reverse --no-merges)

# 如果没有新的提交，使用最新标签到上一个标签的范围
if [ -z "$commits" ]; then
  previousTag=$(git describe --tags --abbrev=0 ${latestTag}^ 2>/dev/null)
  if [ -n "$previousTag" ]; then
    range="${previousTag}..${latestTag}"
    commits=$(git log $range --pretty=format:"- %s by @%an in %h" --reverse --no-merges)
  fi
fi

# 按类型分类提交并移除类型前缀，保留作者信息
featCommits=$(echo "$commits" | grep '^- feat' | sed 's/^- feat: \?/- /')
fixCommits=$(echo "$commits" | grep '^- fix' | sed 's/^- fix: \?/- /')
docsCommits=$(echo "$commits" | grep '^- docs' | sed 's/^- docs: \?/- /')
perfCommits=$(echo "$commits" | grep '^- perf' | sed 's/^- perf: \?/- /')
refactorCommits=$(echo "$commits" | grep '^- refactor' | sed 's/^- refactor: \?/- /')
testCommits=$(echo "$commits" | grep '^- test' | sed 's/^- test: \?/- /')
otherCommits=$(echo "$commits" | grep -v '^- \(feat\|fix\|docs\|perf\|refactor\|test\)' | sed 's/^- [^:]*: \?/- /')

# 构建 changelog
{
  echo "## CHANGELOG"
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
} > CHANGELOG.md

# 如果 changelog 不为空，则添加到暂存区
if [ -s CHANGELOG.md ]; then
  git add CHANGELOG.md
fi