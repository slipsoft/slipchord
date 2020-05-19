# Projet CHORD - Nicolas Peugnet - Sylvain Joube

Dans tout les exercices de notre projet, le simulateur a pour rang MPI N-1, c'est à dire que c'est le processus ayant l’identifiant MPI le plus élevé. Le processus de rang 0 est un pair comme un autre.  

## Exercice 1 - Recherche d’une clé (6 points)

### Question 1.1. Initialisation de le DHT

Le processus simulateur s'occupe de créer la DHT (distributed hash-table) et les finger tables des processus, puis envoie les infos utiles aux pairs (processus créés). Il choisit aussi le nombre de clefs du système. (c'est une puissance de deux)

Le nombre de pairs est fonction du nombre de processus créés via MPI, pour changer leur nombre il suffit de changer la valeur de `NP` exemple `make run NP=11`.

IL est possible d'initialiser la DHT aléatoirement (fonction `init_pairs_aleatoire_classe`) ou de l'initialiser selon ce que nous avons vu en TD (fonction `init_pairs_TD`).  

### Question 1.2. Recherche de clef

Notre implémentation reprend exactement ce qui a été vu en TD. Le code est abondamment commenté.

### Question 1.3. Affichage via MPI

Lorsque les variables suivantes sont mises à 1 (true), toutes les informations de débug sont affichées.

```c
int show_full_debug_info = 1;
int show_peers_messages = 1;
```

Modifier la variable suivante pour initialiser les pairs soit selon le TD, soit aléatoirement :

```c
// 0 si initialiser en fonction du TD, 1 si initialiser aléatoirement
int dht_aleatoire = 1;
```

Il est très facilement possible de vérifier que notre algorithme s'est bien déroulé car il affiche les valeurs des pairs de la DHT ainsi que leur finger table, puis la totalité des messages échangés. Voir ci-après un exemple (tronqué) de trace d'exécution de notre programme :

```
VALEUR DE LA SEED ACTUELLE : 774062185
nombre_clefs_exposant(6)  nombre_clefs(64)  nombre_pairs(10) 

------- Pairs de la DHT -------
 - 10 - 15 - 16 - 17 - 30 - 35 - 40 - 53 - 58 - 62 - 

------- Finger tables des pairs -------

--- Finger table du pair 10 (rang mpi 0) ---
   [0]  clef( 11)  pair( 15)   rang_MPI( 1)
   [1]  clef( 12)  pair( 15)   rang_MPI( 1)
   [2]  clef( 14)  pair( 15)   rang_MPI( 1)
   [3]  clef( 18)  pair( 30)   rang_MPI( 4)
   [4]  clef( 26)  pair( 30)   rang_MPI( 4)
   [5]  clef( 42)  pair( 53)   rang_MPI( 7)

--- Finger table du pair 15 (rang mpi 1) ---
   [0]  clef( 16)  pair( 16)   rang_MPI( 2)
   [1]  clef( 17)  pair( 17)   rang_MPI( 3)
   [2]  clef( 19)  pair( 30)   rang_MPI( 4)
   [3]  clef( 23)  pair( 30)   rang_MPI( 4)
   [4]  clef( 31)  pair( 35)   rang_MPI( 5)
   [5]  clef( 47)  pair( 53)   rang_MPI( 7)

--- Finger table du pair 16 (rang mpi 2) ---
   [0]  clef( 17)  pair( 17)   rang_MPI( 3)
   [1]  clef( 18)  pair( 30)   rang_MPI( 4)
   [2]  clef( 20)  pair( 30)   rang_MPI( 4)
   [3]  clef( 24)  pair( 30)   rang_MPI( 4)
   [4]  clef( 32)  pair( 35)   rang_MPI( 5)
   [5]  clef( 48)  pair( 53)   rang_MPI( 7)

--- Finger table du pair 17 (rang mpi 3) ---
   [0]  clef( 18)  pair( 30)   rang_MPI( 4)
   [1]  clef( 19)  pair( 30)   rang_MPI( 4)
   [2]  clef( 21)  pair( 30)   rang_MPI( 4)
   [3]  clef( 25)  pair( 30)   rang_MPI( 4)
   [4]  clef( 33)  pair( 35)   rang_MPI( 5)
   [5]  clef( 49)  pair( 53)   rang_MPI( 7)

--- Finger table du pair 30 (rang mpi 4) ---
   [0]  clef( 31)  pair( 35)   rang_MPI( 5)
   [1]  clef( 32)  pair( 35)   rang_MPI( 5)
   [2]  clef( 34)  pair( 35)   rang_MPI( 5)
   [3]  clef( 38)  pair( 40)   rang_MPI( 6)
   [4]  clef( 46)  pair( 53)   rang_MPI( 7)
   [5]  clef( 62)  pair( 62)   rang_MPI( 9)

--- Finger table du pair 35 (rang mpi 5) ---
   [0]  clef( 36)  pair( 40)   rang_MPI( 6)
   [1]  clef( 37)  pair( 40)   rang_MPI( 6)
   [2]  clef( 39)  pair( 40)   rang_MPI( 6)
   [3]  clef( 43)  pair( 53)   rang_MPI( 7)
   [4]  clef( 51)  pair( 53)   rang_MPI( 7)
   [5]  clef(  3)  pair( 10)   rang_MPI( 0)

--- Finger table du pair 40 (rang mpi 6) ---
   [0]  clef( 41)  pair( 53)   rang_MPI( 7)
   [1]  clef( 42)  pair( 53)   rang_MPI( 7)
   [2]  clef( 44)  pair( 53)   rang_MPI( 7)
   [3]  clef( 48)  pair( 53)   rang_MPI( 7)
   [4]  clef( 56)  pair( 58)   rang_MPI( 8)
   [5]  clef(  8)  pair( 10)   rang_MPI( 0)

--- Finger table du pair 53 (rang mpi 7) ---
   [0]  clef( 54)  pair( 58)   rang_MPI( 8)
   [1]  clef( 55)  pair( 58)   rang_MPI( 8)
   [2]  clef( 57)  pair( 58)   rang_MPI( 8)
   [3]  clef( 61)  pair( 62)   rang_MPI( 9)
   [4]  clef(  5)  pair( 10)   rang_MPI( 0)
   [5]  clef( 21)  pair( 30)   rang_MPI( 4)

--- Finger table du pair 58 (rang mpi 8) ---
   [0]  clef( 59)  pair( 62)   rang_MPI( 9)
   [1]  clef( 60)  pair( 62)   rang_MPI( 9)
   [2]  clef( 62)  pair( 62)   rang_MPI( 9)
   [3]  clef(  2)  pair( 10)   rang_MPI( 0)
   [4]  clef( 10)  pair( 10)   rang_MPI( 0)
   [5]  clef( 26)  pair( 30)   rang_MPI( 4)

--- Finger table du pair 62 (rang mpi 9) ---
   [0]  clef( 63)  pair( 10)   rang_MPI( 0)
   [1]  clef(  0)  pair( 10)   rang_MPI( 0)
   [2]  clef(  2)  pair( 10)   rang_MPI( 0)
   [3]  clef(  6)  pair( 10)   rang_MPI( 0)
   [4]  clef( 14)  pair( 15)   rang_MPI( 1)
   [5]  clef( 30)  pair( 30)   rang_MPI( 4)


(pair 53 rg 7) : Simulateur me demande de rechercher la clef 26

(pair 53 rg 7) : j'envoie la demande au pair 10 (rg 0) (recherche de la clef 26).

(pair 10 rg 0) : j'envoie la demande au pair 15 (rg 1) (recherche de la clef 26).

(pair 15 rg 1) : j'envoie la demande au pair 17 (rg 3) (recherche de la clef 26).

(pair 17 rg 3) : mon successeur(30) est responsable de la clef(26).

(pair 17 rg 3) : je renvoie le message au demandeur : rang_MPI(7)  responsable : val(30) rg(4).

(pair 53 rg 7) : réception (de rg 3) du pair responsable de la clef : valeur(30) rang_MPI(4).


-----> Simulateur : le pair en charge de la clef 26 est le pair 30.


```

