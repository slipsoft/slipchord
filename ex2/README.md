# Projet CHORD

## Exercice 2 - Calcul des finger tables (9 points)

### Question 2.1. Calcul des fingertables

Afin de calculer sa fingertable, chaque noeud a besoin de connaître la valeur de l'ensemble des pairs qui constituent la DHT CHORD. Pour cela nous utiliserons l'algorithme de Chang-Roberts vu en cours (une reprise de l'exercice 1 du TME6). Ainsi, une fois qu'un pair a récupéré l'ensemble des valeurs (et rangs MPI) des pairs, il peut générer sa fingertable.  

Cet algorithme demande l'envoi de messages de taille importante, et va un peu à l'encontre du principe de CHORD (ayant pour principe d'optimiser le ratio entre nombre de messages émis et taille de la table de routage), mais offre une complexité en messages très intéressante, en moyenne de (n * log(n)).  

Soit N le nombre total de pairs du système. On suppose qu'il y a entre 1 et N initiateurs, que chaque processus connaît sa propre valeur et son rang MPI. Les messages ne circulent que dans un sens. Chaque processus initiateur envoie un message de taille N + 1. Les N premières cases sont destinées à être remplies par les pairs, lorsqu'ils reçoivent le jeton, et la dernière case est l'identifiant du jeton. Lorsqu'un pair initiateur reçoit un jeton ayant un identifiant inférieur au sien, il le détruit. Il n'y a ainsi à la fin qu'un seul jeton dans le système, ayant l'identifiant le plus élevé. Lorsque le jeton est revenu à son envoyeur, cela veut dire qu'il connaît les identifiants de tous les pairs. Le message est alors envoyé de nouveau, avec un autre TAG (message de type "annonce des identifiants des pairs") et n'est détruit qu'une fois qu'il est revenu au pair qui l'a émis.  

**Notre algorithme a donc une complexité moyenne de (n * log(n) + n) c'est à dire (n * log(n)).** Mais les messages envoyés sont très gros. Cela pourrait être comparé à la constitution d'un index dans une base de données : ça nécessite beaucoup de mémoire et beaucoup de calculs, mais les requêtes qui s'en suivent seront plus légères. Une fois les finger table crées par les pairs, la liste complète des pairs peut être effacée de leur mémoire.  

Dans notre implémentation MPI, lorsqu'un pair ajoute son identifiant au jeton, il met dans la case d'index "rang MPI de ce pair" sa valeur. S'il n'y avait pas de rang MPI, il faudrait faire grandir la taille du jeton à chaque pair, et associer, pour chaque emplacement, une structure contenant la valeur du pair actuel et son adresse (MPI, IP, MAC, ...).

