# Projet CHORD - Nicolas Peugnet - Sylvain Joube

# ![slipchord](./docs/logo.png)

Une implementation de l'algorithme distribué CHORD à l'aide de MPI par slipsoft.

Attention, il est important de laisser les trois dossiers ex1, ex2 et ex3 ensemble, car les fichiers `ex1/utils.h` et `ex1/utils.c` sont partagés avec tous les exercices.

Chaque exercice est dans un dossier indépendant, chaque dossier comprend un `Makefile` et un fichier `README.md` contenant une explication du code et les réponses aux questions posées par le sujet.

Pour compiler et exécuter un des exercices il suffit de faire `make run` dans le dossier de l'exercice. La commande `make` se contente de compiler simplement.

## Prérequis

- MPI

## Compile

    make

## Lancer

    make run [NP=<nb-procs>]

