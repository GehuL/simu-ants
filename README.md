
# Introduction #
Simulation de fourmis avec intelligence artificielle (NEAT). 
Projet réalisé en 2e année d'école d'ingénieur à Polytech Dijon. 
Étudiants: Yohann Turpin et Lauric Géhu. 

# Compilation
Plusieurs étapes sont nécessaire afin de compiler le projet. 
Tout d'abord, il est nécessaire de télécharger [Raylib](https://github.com/raysan5/raylib/releases/tag/5.0) et [Raygui](https://github.com/raysan5/raygui/releases/tag/4.0). 
Ensuite, extraire Raygui et Raylib à la racine de l'ordinateur de préférence (C:\). 
Puis glisser le fichier `raygui.h` dans le dossier `C:\raylib\raylib\src`.
Par la suite, il faut modifier des macros dans le fichier `C:\raylib\raylib\src\config.h`.
Les deux macros suivantes doivent être mis en commentaire: 
```
#define SUPPORT_PARTIALBUSY_WAIT_LOOP 
#define SUPPORT_BUSY_WAIT_LOOP  
```
Et celle-ci décommenté: 
```
#define SUPPORT_CUSTOM_FRAME_CONTROL 
```
A présent on peut compiler raylib en entrant dans le terminal  
à partir du dossier src: 
```bash
MAKE
```

Pour travailler avec VSCode, il reste à créer une configuration pour compiler et executer le projet. 
Il suffit d'aller dans le répertoire     
`C:\raylib\raylib\projects\VSCode\`  
et copier le dossier `.vscode` afin de le coller à la racine du projet. 
Enfin, dans le fichier task.json, modifier les valeurs de la clé "args" pour avoir la configuration ci-dessous:
```json
"args": [
            "RAYLIB_PATH=C:/raylib/raylib",
            "PROJECT_NAME=${workspaceFolderBasename}.exe",
             "OBJS=src/*.cpp",
            "BUILD_MODE=DEBUG"
          ]
```
