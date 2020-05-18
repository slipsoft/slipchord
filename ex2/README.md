# Projet CHORD - Nicolas Peugnet - Sylvain JOUBE

## Exercice 2 - Calcul des finger tables (9 points)

### Question 2.1. Calcul des fingertables

Afin de calculer sa fingertable, chaque noeud a besoin de connaître la valeur de l'ensemble des pairs qui constituent la DHT CHORD. Pour cela nous utiliserons l'algorithme de Chang-Roberts vu en cours (une reprise de l'exercice 1 du TME6). Ainsi, une fois qu'un pair a récupéré l'ensemble des valeurs (et rangs MPI) des pairs, il peut générer sa fingertable.  

Cet algorithme demande l'envoi de messages de taille importante, et va un peu à l'encontre du principe de CHORD (ayant pour principe d'optimiser le ratio entre nombre de messages émis et taille de la table de routage), mais offre une complexité en messages très intéressante, en moyenne de (n * log(n)).  

Soit N le nombre total de pairs du système. On suppose qu'il y a entre 1 et N initiateurs, que chaque processus connaît sa propre valeur et son rang MPI. Les messages ne circulent que dans un sens. Chaque processus initiateur envoie un message de taille N + 1. Les N premières cases sont destinées à être remplies par les pairs, lorsqu'ils reçoivent le jeton, et la dernière case est l'identifiant du jeton. Lorsqu'un pair initiateur reçoit un jeton ayant un identifiant inférieur au sien, il le détruit. Il n'y a ainsi à la fin qu'un seul jeton dans le système, ayant l'identifiant le plus élevé. Lorsque le jeton est revenu à son envoyeur, cela veut dire qu'il connaît les identifiants de tous les pairs. Le message est alors envoyé de nouveau, avec un autre TAG (message de type "annonce des identifiants des pairs") et n'est détruit qu'une fois qu'il est revenu au pair qui l'a émis.  

**Notre algorithme a donc une complexité moyenne de (n * log(n) + n) c'est à dire (n * log(n)).** Mais les messages envoyés sont très gros. Cela pourrait être comparé à la constitution d'un index dans une base de données : ça nécessite beaucoup de mémoire et beaucoup de calculs, mais les requêtes qui s'en suivent seront plus légères. Une fois les finger table crées par les pairs, la liste complète des pairs peut être effacée de leur mémoire.  

Dans notre implémentation MPI, lorsqu'un pair ajoute son identifiant au jeton, il met dans la case d'index "rang MPI de ce pair" sa valeur. S'il n'y avait pas de rang MPI, il faudrait faire grandir la taille du jeton à chaque pair, et associer, pour chaque emplacement, une structure contenant la valeur du pair actuel et son adresse (MPI, IP, MAC, ...).  



### Question 2.2. Implémentation MPI

Pour lancer notre programme, il faut se mettre dans le dossier `ex2` et exécuter dans un terminal `make run`. **Attention depuis ce dossier, il est nécessaire d'avoir un accès au chemin `../ex1/utils.h` pour avoir accès aux fonctions partagées nécessaires à l'exécution.**

Un affichage exhaustif d'une exécution est le suivant :

```
mpicc -W -Wall -g -c main.c -o main.o
mpicc -W -Wall -g main.o utils.o ../ex1/utils.o -o main
mpirun --oversubscribe -np 11 main

------- Pairs (non classés) de la DHT -------
 - 39 - 6 - 41 - 51 - 17 - 63 - 10 - 44 - 13 - 58 - 

P 4> Est initiateur (valeur associée 17).
P 7> Est initiateur (valeur associée 44).
P 2> Est initiateur (valeur associée 41).
P 6> Est initiateur (valeur associée 10).
P 9> Est initiateur (valeur associée 58).
[INIT] P 7> Deleted message from 6
[INIT] P 9> Deleted message from 7
[INIT] P 4> Deleted message from 2
[INIT] P 6> Deleted message from 4
[DONE] P 0> Received the whole peer_table

--- Finger table du pair 39 (rang mpi 0) ---
   [0]  clef( 40)  pair( 41)   rang_MPI( 2)
   [1]  clef( 41)  pair( 41)   rang_MPI( 2)
   [2]  clef( 43)  pair( 44)   rang_MPI( 7)
   [3]  clef( 47)  pair( 51)   rang_MPI( 3)
   [4]  clef( 55)  pair( 58)   rang_MPI( 9)
   [5]  clef(  7)  pair( 10)   rang_MPI( 6)


[DONE] P 1> Received the whole peer_table

--- Finger table du pair 6 (rang mpi 1) ---
   [0]  clef(  7)  pair( 10)   rang_MPI( 6)
   [1]  clef(  8)  pair( 10)   rang_MPI( 6)
   [2]  clef( 10)  pair( 10)   rang_MPI( 6)
   [3]  clef( 14)  pair( 17)   rang_MPI( 4)
   [4]  clef( 22)  pair( 39)   rang_MPI( 0)
   [5]  clef( 38)  pair( 39)   rang_MPI( 0)


[DONE] P 2> Received the whole peer_table

--- Finger table du pair 41 (rang mpi 2) ---
   [0]  clef( 42)  pair( 44)   rang_MPI( 7)
   [1]  clef( 43)  pair( 44)   rang_MPI( 7)
   [2]  clef( 45)  pair( 51)   rang_MPI( 3)
   [3]  clef( 49)  pair( 51)   rang_MPI( 3)
   [4]  clef( 57)  pair( 58)   rang_MPI( 9)
   [5]  clef(  9)  pair( 10)   rang_MPI( 6)


[DONE] P 3> Received the whole peer_table

--- Finger table du pair 51 (rang mpi 3) ---
   [0]  clef( 52)  pair( 58)   rang_MPI( 9)
   [1]  clef( 53)  pair( 58)   rang_MPI( 9)
   [2]  clef( 55)  pair( 58)   rang_MPI( 9)
   [3]  clef( 59)  pair( 63)   rang_MPI( 5)
   [4]  clef(  3)  pair(  6)   rang_MPI( 1)
   [5]  clef( 19)  pair( 39)   rang_MPI( 0)


[DONE] P 4> Received the whole peer_table

--- Finger table du pair 17 (rang mpi 4) ---
   [0]  clef( 18)  pair( 39)   rang_MPI( 0)
   [1]  clef( 19)  pair( 39)   rang_MPI( 0)
   [2]  clef( 21)  pair( 39)   rang_MPI( 0)
   [3]  clef( 25)  pair( 39)   rang_MPI( 0)
   [4]  clef( 33)  pair( 39)   rang_MPI( 0)
   [5]  clef( 49)  pair( 51)   rang_MPI( 3)


[DONE] P 5> Received the whole peer_table

--- Finger table du pair 63 (rang mpi 5) ---
   [0]  clef(  0)  pair(  6)   rang_MPI( 1)
   [1]  clef(  1)  pair(  6)   rang_MPI( 1)
   [2]  clef(  3)  pair(  6)   rang_MPI( 1)
   [3]  clef(  7)  pair( 10)   rang_MPI( 6)
   [4]  clef( 15)  pair( 17)   rang_MPI( 4)
   [5]  clef( 31)  pair( 39)   rang_MPI( 0)


[DONE] P 6> Received the whole peer_table

--- Finger table du pair 10 (rang mpi 6) ---
   [0]  clef( 11)  pair( 13)   rang_MPI( 8)
   [1]  clef( 12)  pair( 13)   rang_MPI( 8)
   [2]  clef( 14)  pair( 17)   rang_MPI( 4)
   [3]  clef( 18)  pair( 39)   rang_MPI( 0)
   [4]  clef( 26)  pair( 39)   rang_MPI( 0)
   [5]  clef( 42)  pair( 44)   rang_MPI( 7)


[DONE] P 7> Received the whole peer_table

--- Finger table du pair 44 (rang mpi 7) ---
   [0]  clef( 45)  pair( 51)   rang_MPI( 3)
   [1]  clef( 46)  pair( 51)   rang_MPI( 3)
   [2]  clef( 48)  pair( 51)   rang_MPI( 3)
   [3]  clef( 52)  pair( 58)   rang_MPI( 9)
   [4]  clef( 60)  pair( 63)   rang_MPI( 5)
   [5]  clef( 12)  pair( 13)   rang_MPI( 8)


[DONE] P 8> Received the whole peer_table

--- Finger table du pair 13 (rang mpi 8) ---
   [0]  clef( 14)  pair( 17)   rang_MPI( 4)
   [1]  clef( 15)  pair( 17)   rang_MPI( 4)
   [2]  clef( 17)  pair( 17)   rang_MPI( 4)
   [3]  clef( 21)  pair( 39)   rang_MPI( 0)
   [4]  clef( 29)  pair( 39)   rang_MPI( 0)
   [5]  clef( 45)  pair( 51)   rang_MPI( 3)


[DONE] P 9> Received the whole peer_table

--- Finger table du pair 58 (rang mpi 9) ---
   [0]  clef( 59)  pair( 63)   rang_MPI( 5)
   [1]  clef( 60)  pair( 63)   rang_MPI( 5)
   [2]  clef( 62)  pair( 63)   rang_MPI( 5)
   [3]  clef(  2)  pair(  6)   rang_MPI( 1)
   [4]  clef( 10)  pair( 10)   rang_MPI( 6)
   [5]  clef( 26)  pair( 39)   rang_MPI( 0)

```