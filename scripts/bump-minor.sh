#!/bin/bash

# 최신 태그 가져오기
latest_tag=$(git describe --tags --abbrev=0 2>/dev/null)

if [ -z "$latest_tag" ]; then
    latest_tag="v0.0.0"
fi

# 버전 숫자 추출
version=${latest_tag#v}
IFS='.' read -r major minor patch <<< "$version"

# Minor 증가, Patch는 0으로 초기화
minor=$((minor + 1))
patch=0

new_version="v$major.$minor.$patch"
echo "$new_version"
