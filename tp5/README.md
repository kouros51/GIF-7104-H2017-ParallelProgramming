# GIF-7104 - Simulation thermique 2D
Vincent, Thierry et Houssem.

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
~/build/executables/tp5_cpu_sequential
~/build/executables/tp5_opencl
~/build/executables/tp5_unified
```
S'assurer que les exécutables ont les permissions pour supprimer et créer des fichiers.
### Lancer
Lancer avec les options suivantes :
```
h [défaut : -h] : aide sur l'utilisation de l'application
d [défaut : -d 100 100] : dimension de la matrice
s [défaut : -s 0.01] : seuil de variation maximale pour le critère d'arrêt
f [défaut : -f ../configs/configFile] : nom du fichier de configuration
i [défaut : -i 1] : nombre d'itérations entre deux images consécutives
```
Puisque c'est un algorithme massivement parralèle, on ne peut pas changer le nombre de coeurs à exploiter.
#### Exemple
```
    $ ./executables/tp5_opencl -d 150 100 -s 0.001 -f ../configs/configFile -i 15
Size: 150x100
Frame save interval: 15
Threshold: 0.001
```
