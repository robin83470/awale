# Projet Serveur de Jeu Awalé

## 1. Présentation

Ce projet implémente un serveur multijoueur en C permettant à plusieurs clients de jouer à l’Awalé, se défier, chatter et observer des parties. Le serveur gère automatiquement les connexions, les états des joueurs et la logique complète du jeu.

---

## 2. Fonctionnalités principales

* Connexion par pseudo
* Recherche automatique de partie
* Défi d’un joueur spécifique
* Observation en direct de parties en cours
* Jeu d’Awalé complet (graines, captures, famine, score, fin de partie)
* Chat intégré entre joueurs
* Gestion de multiples états côté serveur

---

## 3. Organisation des fichiers

```
server2.c
client2.c 
server2.h      
client2.h 
client22.h      
Makefile
README.txt
```

---

## 4. Compilation

```
make (ou make re si le projet est déjà compilé)
./server
./client [adresseIp du serveur] [pseudo]
```

---

## 5. Déroulement d’une partie

1. Le joueur se connecte avec un pseudo.
2. Il choisit : jouer, observer ou défier un joueur.
3. Le serveur crée/associe la partie.
4. Les joueurs jouent chacun leur tour.
5. Le serveur vérifie les coups, met à jour les scores et annonce le gagnant.
6. Il est possible d'obsrever une partie en cours avec l'option 2 du menu, de plus il est possible de chatter en direct avec les autres specateurs/joueurs
7. Il est aussi possible de défier une personne spécififquement avec l'option 3 du menu, il faut ensuite saisir le nom d'un joueurs disponibles, puis attendre qu'il accepte votre demande de défie

---

## 6. Utilisation de l'IA

Nous avons utiliser l'IA pour la méthode d'affichage de notre jeu awalé.
