# Projet CHORD - Nicolas Peugnet - Sylvain Joube

## Exercice 3 - Insertion d’un pair (5 points + bonus)

### Question 3.1. Proposez un algorithme permettant une insertion

Pour identifier de manière claire les processus dont nous allons parler, nous associons des lettres aux processus importants pour nous :
- (I) le processus à insérer dans le système
- (S) le processus successeur de ce processus (trouvé)
- {R} l'ensemble des processus de la reverse-table du processus (S)
- (P) le prédécesseur du processus (I) à insérer (trouvé)

#### Prérequis

On suppose l'unicité de la valeur de chaque noeud, deux noeuds ne peuvent pas avoir la même valeur. On suppose également que la DHT est correcte et cohérente. On suppose donc aussi que le processus à insérer a une valeur qui n'est pas déjà prise.

#### Première requête

Dans un premier temps, le nouveau noeud contacte un pair quelconque de la DHT, et lui demande quel est le noeud en charge de sa valeur. De cette manière, le noeud à insérer (I) connaît son successeur (S).

#### Mise à jour des reverse-finger-table

(I) enregistre que (S) est son successeur, et lui demande ensuite sa reverse-finger-table. (I) contacte tous les processus {R} de la reverse-finger-table pour les informer qu'il est désormais là, dans l'intervalle entre le prédécesseur de (S) et le nouveau processus (I). Tous ces pairs {R} contactés doivent mettre à jour leur finger-table. (S) doit avoir préalablement vidé sa reverse-finger-table, et attend que les processus de {R} qui l'ont toujours dans leur finger-table lui envoient un message spécial, permettant de les enregistrer de nouveau dans la reverse-finger-table de (S). Les processus de {R} envoient également un message à (I) s'ils y font référence, pour une (ou plus) clef de leur finger-table. Le processus à insérer (I) a donc sa reverse-finger-table à jour.

#### Mise à jour des finger-table

Du point du processus (I), son prédécesseur (P) est donc le pair de sa reverse-finger-table ayant la valeur la plus grande. Le processus (I) demande à son prédécesseur (P) et à son successeur (S) leur finger-table. À partir de ces deux tables, il calcule sa finger-table (approximative) en faisant comme si l'ensemble des pairs était ceux qu'il connaissait via ces deux finger-table. Maintenant, pour déterminer exactement sa finger-table, il va demander à chaque pair de sa finger-table appriximative qui est réellement en charge de chacune des clefs.

#### Complexité en messages




**Trucs pas mis :**
Le prédécesseur du processus est le plus grand (cycliquement) pair de ma reverse finger table. On lui demande sa finger table. Ensuite, on constitue notre finger table avec l'ensemble des valeurs constituées par le prédécesseur et le successeur. On vérifie alors auprès de tous ces processus que ce sont bien eux les responsables des clefs de notre finger table, et si ce n'est pas eux, ils nous renvoient le réel responsable de chaque clef, tout en mettant à jour leur reverse finger-table.

Lorsqu'on informe un pair de notre reverse finger table qu'il faut qu'il mette à jour sa finger table :

- Les pairs contactés (B) n'ont que besoin de la valeur du nouveau pair inséré pour mettre à jour leurs tables
- Le pair qui les a contactés (A) supprime toutes ses entrées de la reverse finger table
- Les pairs contactés (B) envoient soit un message au pair




 de contacter l'ensemble des noeuds qu'elle contient pour les informer de son entrée dans la DHT.

A l'aide de cette table il peut également contacter son plus proche prédécesseur pour récupérer sa finger-table.



