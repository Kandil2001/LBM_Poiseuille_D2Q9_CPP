#!/usr/bin/env bash
set -e

REPO_URL="https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP.git"

git init
git branch -M main
git add .
git commit -m "Add simple D2Q9 LBM Poiseuille solver"
git remote remove origin 2>/dev/null || true
git remote add origin "$REPO_URL"
git push -u origin main
