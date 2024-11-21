# Compilation
Plusieurs étapes sont nécessaire afin de compiler le projet. 
Tout d'abord, il est nécessaire de télécharger [Raylib](https://github.com/raysan5/raylib/releases/tag/5.0) et [Raygui](https://github.com/raysan5/raygui/releases/tag/4.0). 
Ensuite, extraire Raygui et Raylib à la racine de l'ordinateur de préférence (C:\). 
Puis glisser le fichier raygui.h dans le dossier  
```bash
C:\raylib\raylib\src
```
Par la suite, modifier les macros dans le fichier C:\raylib\raylib\src\config.h: 
Les deux macros suivantes doivent être mis en commentaire: 
> #define SUPPORT_PARTIALBUSY_WAIT_LOOP  
> #define SUPPORT_BUSY_WAIT_LOOP 
Et celle-ci décommenté: 
> #define SUPPORT_CUSTOM_FRAME_CONTROL 
A présent on peut compiler raylib en entrant dans le terminal  
à partir du dossier src: 
```bash
MAKE
```

Pour travailler avec VSCode, il reste à créer une configuration pour compiler et executer le projet. 
Il suffit d'aller dans le répertoire: 
> C:\raylib\raylib\projects\VSCode\ 
Puis copier le dossier .vscode et le coller à la racine du projet. 
Il ne reste plus qu'a indiquer l'emplacement de Raylib dans le fichier task.json.  
