# Projet CHORD - Nicolas Peugnet - Sylvain Joube

## Exercice 3 - Insertion d’un pair (5 points + bonus)

### Question 3.1. Proposez un algorithme permettant une insertion

Pour identifier de manière claire les processus et notions dont nous allons parler, nous associons des lettres aux notions et processus importants pour nous :
- (I) le processus à insérer dans le système
- (S) le processus successeur de ce processus (trouvé)
- {R} l'ensemble des processus de la reverse-table du processus (S)
- (P) le prédécesseur du processus (I) à insérer (trouvé)
- n ou N le nombre de pairs du système, (I) compris.

#### Prérequis

On suppose l'unicité de la valeur de chaque noeud, deux noeuds ne peuvent pas avoir la même valeur. On suppose également que la DHT est correcte et cohérente. On suppose donc aussi que le processus à insérer a une valeur qui n'est pas déjà prise.

#### Première requête

Dans un premier temps, le nouveau noeud contacte un pair quelconque de la DHT, et lui demande quel est le noeud en charge de sa valeur. De cette manière, le noeud à insérer (I) connaît son successeur (S).

#### Mise à jour des reverse-finger-table

(I) enregistre que (S) est son successeur, et lui demande ensuite sa reverse-finger-table. (I) contacte tous les processus {R} de la reverse-finger-table pour les informer qu'il est désormais là, dans l'intervalle entre le prédécesseur de (S) et le nouveau processus (I). Tous ces pairs {R} contactés doivent mettre à jour leur finger-table. (S) doit avoir préalablement vidé sa reverse-finger-table, et attend que les processus de {R} qui l'ont toujours dans leur finger-table lui envoient un message spécial, permettant de les enregistrer de nouveau dans la reverse-finger-table de (S). Les processus de {R} envoient également un message à (I) s'ils y font référence, pour une (ou plus) clef de leur finger-table. Le processus à insérer (I) a donc sa reverse-finger-table à jour.

#### Mise à jour des finger-table

Du point du processus (I), son prédécesseur (P) est donc le pair de sa reverse-finger-table ayant la valeur la plus grande. Le processus (I) demande à son prédécesseur (P) et à son successeur (S) leur finger-table. À partir de ces deux tables, il calcule sa finger-table (approximative) en faisant comme si l'ensemble des pairs était ceux qu'il connaissait via ces deux finger-table. Maintenant, pour déterminer exactement sa finger-table, il va demander à chaque pair de sa finger-table appriximative qui est réellement en charge de chacune des clefs.

#### Complexité en messages

Complexité de chaque opération, approximative (à une constante près)

- Demande initiale : log(N)
- Envoi aux pairs de la reverse-finger-table (et, au passage, mise à jour des finger-table): en moyenne log(N), dans le pire cas N
- Réponse de ces pairs : en moyenne log(N), dans le pire cas N
- Envoi de messages aux pairs de la finger-table approximative du processus (I) à ajouter : (log(N))^2   (si l'écriture passe chez vous : log²(N))

La complexité en message est donc en O((log(N)^2) (log carré de N) en moyenne, O(N) dans le pire cas.

### Question 3.2. Un début d'implémentation

Nous n'avons que eu le temps d'implémenter les reverse-finger-table d'une manière distribuée, et n'avons pas eu le temps de plus en faire, comme nous avons une importante charge de travail dans toutes les UE, compte tenu de la situation actuelle et de l'orientation générale prise par les responsables d'UE en master SAR qui nous demandent de rendre tous les TME dans toutes les matières.