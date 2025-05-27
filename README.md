# Digital Hourglass - Projet Arduino

Ce projet a été réalisé dans le cadre de ma formation à l’ESEO.  
L’idée était de concevoir un sablier électronique, fonctionnel, autonome et visuellement lisible, basé sur une carte Arduino Uno.

Le principe est simple : l’utilisateur peut régler la durée à l’aide d’un encodeur rotatif, puis observer le décompte du temps soit sous forme numérique, soit via une animation simulant un vrai sablier, affichée sur deux matrices LED. L’orientation du sablier est détectée par un accéléromètre, ce qui permet d’inverser l’affichage en fonction du sens dans lequel on le retourne.

Le montage électronique repose sur un Arduino Uno, deux matrices LED 8x8, un accéléromètre ADXL335, un encodeur, et une pile 9V pour l’alimentation. Un interrupteur permet d’allumer ou éteindre le système. Tous les composants sont intégrés dans un boîtier imprimé en 3D, modélisé sur mesure.

Côté programmation, j’ai utilisé la bibliothèque LedControl pour piloter les LED. Le code gère la lecture de l’accéléromètre, le réglage du temps via l’encodeur, le mode d’affichage, ainsi que le déroulement de la minuterie. L’objectif était d’avoir quelque chose de simple, intuitif et autonome.

Ce projet m’a permis de travailler à la fois sur l’électronique embarquée, la gestion des capteurs, la programmation Arduino, mais aussi sur la conception matérielle et l’assemblage physique du prototype. C’était un bon mélange entre pratique, technique et créativité.

Le système fonctionne bien et peut servir dans différents contextes : minuterie de jeu, outil de concentration, sablier de cuisine... Il reste encore des pistes d’amélioration, comme l’ajout d’un signal sonore ou d’un indicateur lumineux de fin de décompte, voire une version rechargeable.

Projet réalisé en équipe avec Axel Alabeatrix et Antoine Vadot.
