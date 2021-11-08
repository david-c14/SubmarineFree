#!/bin/sh

set -eu

GITHUB_API_URL=https://api.github.com

GITHUB_TOKEN=$1

# Get release url
curl -o release.json \
    --header "Authorization: token ${GITHUB_TOKEN}" \
    --request GET \
    ${GITHUB_API_URL}/repos/${GITHUB_REPOSITORY}/releases/${GITHUB_REF#"refs/"}

UPLOAD_URL=$(jq -r .upload_url release.json)
sleep 5
ls -al dist
ls -al
ASSET_PATH=$(ls dist/*.vcvplugin)
cp ${ASSET_PATH} .
ASSET_PATH=${ASSET_PATH#"dist/"}
echo ${ASSET_PATH}
ls -al
ls -al dist

curl -i \
    --header "Authorization: token ${GITHUB_TOKEN}" \
    --header "Content-Type: application/zstd" \
    --request POST \
    --data-binary @"${ASSET_PATH}" \
    ${UPLOAD_URL%"{?name,label\}"}?name=${ASSET_PATH#"dist/"}
