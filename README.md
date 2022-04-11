# Sleep-Breathing-Light

Il s'agit d'une lampe aidant à l'endormissement en donnant un rythme de respiration variant de 11 à 6 bat/min. L'éclairage et l'extinction progressive indique les périodes d'inspiration et d'expiration.

La durée de l'endormissement est configurée à 15 minutes. À tout moment, il est possible d'arrêter un cycle en cours par une pression longue. On peut alors relancer un cycle, mais c'est le débit initial qui est réactivé (11 bat/min).

En dehors d'un cycle et par une pression longue, il est possible de choisir la teinte de la lampe.

## Composants
1. Prototype sur Arduino Uno.
2. Cible ATtiny85 :
	* Kit support LiIon + 5v ;
	* Led WS2812 ;
	* Bouton SPST (utilisation du pullup interne);

## Notes
* Travail en cours, le code n'est pas testé pas sur ATtiny85.