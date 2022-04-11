# Sleep-Breathing-Light

Il s'agit d'une lampe aidant à l'endormissement en donnant un rythme de 
respiration variant de 11 à 6 bat/min. L'éclairage et l'extinction progressive 
indique les périodes d'inspiration et d'expiration.

La durée de l'endormissement est programmable, soit en 5 min (une pression sur 
le bouton), soit en 10 min (2 pressions). A tout moment, il est possible de 
relancer une durée supplémentaire. Par contre c'est le débit initiale qui est 
réactivé (11 bat./min).

Par une pression longue, il est possible d'éteindre la lampe.  

## Composants
* https://www.google.fr/search?q=03962a ;
* LiPo 1S 400mAh ;
* Arduino Mini (obsolète) https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/arduino_mini_schematic05.pdf. 

## Notes
* Le Chargeur de Lipo surveille la charge mais aussi la décharge. Il bloque l'alimentation du circuit suivant en-dessous de 2.4v.
* Luminosité (rendre les variations "progressives") :
** https://ledshield.wordpress.com/2012/11/13/led-brightness-to-your-eye-gamma-correction-no/
** http://forum.arduino.cc/index.php/topic,147810.0.html