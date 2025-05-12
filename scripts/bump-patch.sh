#!/bin/bash

# 가장 최신 태그 가져오기
latest_tag=$(git describe --tags --abbrev=0 2>/dev/null)

if [ -z "$latest_tag" ]; then
    latest_tag="v0.0.0"
fi

# v1.2.3 → 숫자 추출
version=${latest_tag#v}
IFS='.' read -r major minor patch <<< "$version"

# Patch 증가
patch=$((patch + 1))

new_version="v$major.$minor.$patch"
echo "$new_version"
