# Repository of the fifth and last practical assignment
## Thermical simulation
Vincent, Thierry and Houssem.

### Prérequis
La librairie ffmpeg doit être installée et accessible en ligne de commande.  
https://ffmpeg.org/download.html  
Le programme fait un appel system à ffmpeg pour générer la vidéo.

### Compiler
À la racine du projet, créer un dossier "build" :
```
mkdir build
```
Se déplacer dans le dossier, générer le makefile avec cmake et compiler :
```
cd build
cmake ..
make
```
Les exécutables sont générés selon le backend disponible et se trouvent dans le dossier "executables".
```
~/build/executables/tp5_cpu
~/build/executables/tp5_cuda
~/build/executables/tp5_opencl
~/build/executables/tp5_unified
```
