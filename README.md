# TP_2_OS

Pour compiler  :

    sudo make docker-qemu

Pour rajouter l'auto-testeur du TP2 :

    sudo docker pull pwilke/oscorrector

Evaluation :
    make eval ACT="tp2-act-4-1"


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

## Question 4.3












