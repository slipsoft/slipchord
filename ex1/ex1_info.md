# Projet CHORD

## Exercice 1 - Recherche d’une clé (6 points)

### Question 1.1. Initialisation de le DHT

Le processus simulateur s'occupe de créer la DHT (distributed hash-table) et les finger tables des processus, puis envoie les infos utiles aux pairs (processus créés). Il choisit aussi le nombre de clefs du système. (c'est une puissance de deux)

Le nombre de pairs est fonction du nombre de processus créés via MPI, pour changer leur nombre il suffit de changer le champ `NP` de notre fichier `ex1/Makefile` (par défait, `NP ?= 5`).

### Question 1.2. Recherche de clef

C'est comme ce que nous avons vu en TD. Ici, je me mets à la place du père qui fait la recherche, "je" désigne donc le père actuel. Pour simplifier, on assimile chaque père à sa valeur (sans dire systématiquement "le père dont la valeur associée est...")

- Je regarde dans ma finger table quel est le plus grand pair strictement plus petit que la clef. S'il n'y a aucun père trouvé, c'est que mon successeur est en charge de la clef. (si ma valeur est plus grande que la clef il y a une erreur, ma valeur doit être strictement inférieure à la clef). Si un père est trouvé, le lui passe la requête et il exécute cet algorithme (récursivement donc).

### Question 1.3. Test

A finir d'écrire. (après manger)