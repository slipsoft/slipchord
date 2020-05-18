# Projet CHORD - Nicolas Peugnet - Sylvain Joube

## Exercice 3 - Insertion d’un pair (5 points + bonus)

### Question 3.1. Proposez un algorithme permettant une insertion

Dans un premier temps, le nouveau noeud contacte un pair voisin MPI pour faire la recherche du noeud responsable de sa valeur + 1. De cette manière il connait son successeur.

Il demande ensuite à son successeur sa finger-table ainsi que sa reverse-finger-table afin de contacter l'ensemble des noeuds qu'elle contient pour les informer de son entrée dans la DHT.

A l'aide de cette table il peut également contacter son plus proche prédécesseur pour récupérer sa finger-table.