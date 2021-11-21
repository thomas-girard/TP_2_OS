# TP_2_OS

Pour compiler  :

    sudo make docker-qemu

Pour rajouter l'auto-testeur du TP2 :

    sudo docker pull pwilke/oscorrector

Evaluation :
    sudo make eval ACT="tp2-act-4-1"


## Question 2.1
sbrk(0) permet d'obtenir l'adresse du "programm break" (sommet du tas), on en déduit que la fonction permet de désalouer toute la mémoire liée au processus. Cela provoque l'interruption du processus, l'instruction suivante "printf" n'est pas exécutée.

## Question 3.1
Il y a 5 segments

Voici la taille des segments numéroté de 1 à 5 :
1.  280 octets
2.  6494 octets
3.  0 octet
4.  58 octets
5.  0 octet

Voici la taille une fois que ces segments seront chargés en mémoire :
1.  280 octets
2.  6494 octets
3.  0 octet
4.  58 octets
5.  1224 octets

## Question 4.2

Il y a 5 VMAs : les 3 premières correspondent aux 5 segments du fichier ELF, et les 2 dernières aux pile et tas

## Question 4.4
On essaye d'accéder à des pages qui ne sont pas alloués d'où l'erreur

## Question 4.7
Il y a encore des erreurs page_fault. scause nous donne le type d'erreur (en l'occurence page_fault), stval nous donne les adresses auxquelles on essaie d'écrire et sepc nous donne les adresses des instructions qui ont provoqués l'erreur

## Question 4.8
Le tas est définis entre [0x0000000000006000; 0x0000000000016000[. La taille du tas est donc de 10 000 en hexa, donc 65536 octet, soit 64 ko.

Il y a 3 pages associés au tas et qui sont dans l'intervalle [0x0000000000006000; 0x0000000000016000[ du tas.

## Question 4.9
C'est l'appel système write() qui échoue dans le test

# Question 4.12
Pas réussi pour l'instant (KO booted) => à compléter plus tard

# Question 5.1
On constate que naivefib  ne marche plus à partir de 17. Ce programme sert à calculer le n-ième terme de la suite de Fibonacci de manière récursive. A partir du 17ème terme, la pile n'est plus assez grande pour stocker les appels récursifs

# Question 5.2















