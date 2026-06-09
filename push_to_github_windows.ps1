git init
git branch -M main
git add .
git commit -m "Add simple D2Q9 LBM Poiseuille solver"
git remote remove origin 2>$null
git remote add origin "https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP.git"
git push -u origin main
