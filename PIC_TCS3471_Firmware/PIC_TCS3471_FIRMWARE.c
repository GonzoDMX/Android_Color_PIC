/*
*	ANDREW O'SHEI
*	PIC TCS3471 FINAL PROJECT
*	Realisation de Projets 1er Semestre
*	Program creer pour MikroC pour Pic
*/

// Libraries sélectionnées dans MikroC: C_String, I2C, UART


#include "stdint.h"				// Library utilisée pour le type de variable uint8_t

// TCS3471 I2C Slave Address //
#define TCS3471_ADDRESS_1   0x29		// Address pour TCS34715FN et TCS34717FN
#define TCS3471_ADDRESS_2   0x39		// Address pour TCS34711FN et TCS34713FN

// TCS3471 Registres de commandes //
#define TCS3471_ENABLE       0x00		// Enable Register:	    Active les états et les interrupts
#define TCS3471_ATIME        0x01		// ADC Time Register:	    Contrôle le temps d'intégration interne des ADC RGBC
#define TCS3471_WTIME        0x03		// Wait Time Register:	    Définit le temps de cycle du processus
#define TCS3471_AILTL        0x04		// Threshold Low LSB:	    Définit l'octet le moins significatif pour le seuil bas RGBC
#define TCS3471_AILTH        0x05		// Threshold Low MSB:	    Définit l'octet le plus significatif pour le seuil bas RGBC
#define TCS3471_AIHTL        0x06		// Threshold High LSB:	    Définit l'octet le moins significatif pour le seuil haut RGBC
#define TCS3471_AIHTH        0x07		// Threshold High MSB:	    Définit l'octet le pluss significatif pour le seuil haut RGBC
#define TCS3471_PERS         0x0C		// Interrupt Persistance:  Définir le nombre d'occurrences de seuil hors plage avant les déclenchements d'interruption
#define TCS3471_CONFIG       0x0D		// Configure Register:	    Définit une augmentation de 12 fois du temps d'attente pour le 'Wait Time Register'
#define TCS3471_CONTROL      0x0F		// Control Register:	    Règle le gain pour les ADC
#define TCS3471_ID           0x12		// ID Register:	    (Lecture seulement) Renvoie l'adresse I2C du composant
#define TCS3471_STATUS       0x13		// Status Register:	    (Lecture seulement) Renvoie l'état du composant
#define TCS3471_CLEAR        0x14		// Clear LSB Register:	    (Lecture seulement) Renvoie l'octet le moins significatif du canal clair
#define TCS3471_CDATAH       0x15		// Clear MSB Register:	    (Lecture seulement) Renvoie l'octet le plus significatif du canal clair
#define TCS3471_RED          0x16		// Red LSB Register:	    (Lecture seulement) Renvoie l'octet le moins significatif du canal rouge
#define TCS3471_RDATAH       0x17		// Red MSB Register:	    (Lecture seulement) Renvoie l'octet le plus significatif du canal rouge
#define TCS3471_GREEN        0x18		// Green LSB Register:	    (Lecture seulement) Renvoie l'octet le moins significatif du canal vert
#define TCS3471_GDATAH       0x19		// Green MSB Register:	    (Lecture seulement) Renvoie l'octet le plus significatif du canal vert
#define TCS3471_BLUE         0x1A		// Blue LSB Register:	    (Lecture seulement) Renvoie l'octet le moins significatif du canal bleu
#define TCS3471_BDATAH       0x1B		// Blue MSB Register:	    (Lecture seulement) Renvoie l'octet le plus significatif du canal bleu

// TCS3471 Bits de contrôle - Pour le registre de Command //
#define TCS3471_COMMAND_BIT  0x80		// Écrire dans le registre de commande
#define TCS3471_AUTOINCR_BIT 0x20		// Envoie automatiquement le deuxième octet lors de la lecture du premier d'une valeur de 2 octets
#define TCS3471_SPECIAL_BIT  0x60		// Soulevez pour sélectionner des fonctions spéciales
#define TCS3471_INTCLEAR_BIT 0x06		// Réinitialiser l'interruption RGBC si elle est à l'état déclenché

// TCS3471 Bits de contrôle - Pour le registre Enable //
#define TCS3471_AIEN_BIT     0x10		// Interrupt enable
#define TCS3471_WEN_BIT      0x08		// Wait Time enable
#define TCS3471_AEN_BIT      0x02		// ADC Enable
#define TCS3471_PON_BIT      0x01		// Activer ou désactiver l'oscillateur interne, nécessaire au fonctionnement du composant

// TCS3471 Bits de contrôle - Pour régler le gain ADC //
#define GAIN_1X              0x00		// Réglez le gain à l'unité, très bien pour la plupart des cas d'utilisation
#define GAIN_4X              0x01		// Régler le gain sur 4 fois l'intensite
#define GAIN_16X             0x02		// Régler le gain sur 16 fois l'intensite
#define GAIN_60X             0x03		// Régler le gain sur 60 fois l'intensite


// Définissez le nom et le mot de passe Bluetooth HC-06 sur le firmware le plus ancien
// (le délimiteur '\ 0' est ajouté juste pour assurer un formatage correct)
// Définissez le nom du module Bluetooth sur "ColorClick"
char blueName1[] = {'A', 'T', '+', 'N', 'A', 'M', 'E', 'C', 'o', 'l', 'o', 'r', 'T', 'o', 'o', 't', 'h', '\r', '\n', '\0'};
// Définissez le mot de passe sur 0000
char bluePin1[] = {'A', 'T', '+', 'P', 'I', 'N', '0', '0', '0', '0', '\r', '\n', '\0'};

// Définissez le nom et le mot de passe Bluetooth HC-06 sur le firmware actuel
// Définissez le nom du module Bluetooth sur "ColorClick"
char blueName2[] = {'A', 'T', '+', 'N', 'A', 'M', 'E', ':', 'C', 'o', 'l', 'o', 'r', 'T', 'o', 'o', 't', 'h', '\r', '\n', '\0'};
// Définissez le mot de passe sur 0000
char bluePin2[] = {'A', 'T', '+', 'P', 'S', 'W', 'D', ':', '\"', '0', '0', '0', '0', '\"', '\r', '\n', '\0'};

// Définir la longueur standard du msg sortant
const int MSG_LEN = 13;

// Précharger les messages sortants standard
// Message pour confirmer la connexion du Bluetooth
char conn[MSG_LEN]     = { '<', 'C', 'O', 'N', '>', 'G', 'O', 'O', 'D', '_', 'C', 'O', 'N' };
// Message pour confirmer la déconnexion du Bluetooth
char disc[MSG_LEN]     = { '<', 'C', 'O', 'N', '>', 'G', 'O', 'O', 'D', '_', 'D', 'I', 'S' };
// Message pour signaler une erreur
char erro[MSG_LEN]     = { '<', 'M', 'S', 'G', '>', 'E', 'R', 'R', 'O', 'R', '(', '1', ')' };

// Précharger les messages entrants standard
// Utilisé pour valider une demande de connexion
static const char connectBT[9] = { '<', 'C', 'O', 'N', 'N', 'E', 'C', 'T', '>' };
// Utilisé pour valider une demande de déconnexion
static const char disconnBT[9] = { '<', 'D', 'I', 'S', 'C', 'O', 'N', 'N', '>' };

// Déclarer une chaine des caracteres formaté pour envoyer les valeurs RGBC via Bluetooth
char msg[13] = { '<', 'T', 'C', 'S', '>', '0', '0', '0', '0', '0', '0', '0', '0' };

// Stocke les messages Bluetooth entrants
char buff[9];

// Stocke l'adresse I2C du TCS3471
uint8_t address;

//TCS341 Valeurs de couleur entrantes
unsigned short clear;		// Stocke la valeur du canal blanc
unsigned short red;		// Stocke la valeur du canal rouge
unsigned short green;		// Stocke la valeur du canal vert
unsigned short blue;		// Stocke la valeur du canal bleu

//Déclarez les valeurs de caractères pour l'affichage à 7 segments
//                         '0'   '1'   '2'   '3'   '4'   '5'   '6'   '7'   '8'   '9'
uint8_t dispNums[]    = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

//Indiquer l'état de la connexion Bluetooth, 1 = ouvert, 0 = fermé
int connexion = 0;

//Declaring functions
uint8_t Init_TCS3471();
uint8_t Config_TCS3471();
void writeToReg(uint8_t reg, uint8_t dat);
void writeTwoReg(uint8_t reg, unsigned short dat);
unsigned short readTwoReg(uint8_t reg);
void clearInterrupt();
int scaleToDisp(unsigned short color);

void Config_Bluetooth();
void FlushRx();
int ErrorCheck();
char *prepMsg(unsigned short clear, unsigned short red, unsigned short green, unsigned short blue);
int scaleToBT(unsigned short color);
void writeBT(char* msg);
int checkMsgBT(char* msg);

char getMSChar(int val);
char getLSChar(int val);

void updateDisplay(uint8_t dis3, uint8_t dis2, uint8_t dis1, uint8_t dis0);
void initDisplay();

//------------------------------------------------------------------------------------------//


//-------------------------------------PROGRAM SETUP----------------------------------------//

//  Configurer le PIC16F877 et les composants attachés  //
void setup() {

//-----CONFIGURE PINS-----//

     ADCON1 = 0x07;		// Configurer les broches AN sur les E / S numériques
     CMCON = 0x07;		// Désactiver le comparateur car il n'est pas utilisé

//-----CONFIGURE 7Seg Display-----//

     TRISD = 0x00;		// Réglez tous les ports D sur la sortie

     TRISA.B0 = 0;		// Réglez RA0 sur la sortie pour 7Seg Disp0
     TRISA.B1 = 0;		// Réglez RA1 sur la sortie pour 7Seg Disp1
     TRISA.B2 = 0;		// Réglez RA2 sur la sortie pour 7Seg Disp2
     TRISA.B3 = 0;		// Réglez RA3 sur la sortie pour 7Seg Disp3

     PORTA.B0 = 0;		// Clear DISP0
     PORTA.B1 = 0;		// Clear DISP1
     PORTA.B2 = 0;		// Clear DISP2
     PORTA.B3 = 0;		// Clear DISP3

     initDisplay();		// Exécuter une séquence de test d'affichage à 7 segments

//-----CONFIGURE BLUETOOTH-----//

     TRISC.B6 = 0;		// Définir UART TX comme sortie
     TRISC.B7 = 1;		// Définir UART RX comme entrée

     UART1_Init(9600);		// Configurer l'interface UART pour la communication avec le module Bluetooth HC-06

     Config_Bluetooth();	// Configurer le module Bluetooth HC-06

//-----CONFIGURE TCS3471-----//

     TRISB.B1 = 1;		// Définissez RB1 comme entrée pour intercepter les interruptions TCS3471

     I2C1_Init(100000);	// Configuration de l'interface I2C pour la communication avec TCS3471

     address = Init_TCS3471(); // Obtenez l'adresse du TCS3471 et validez qu'elle est opérationnelle

     if(!address) {		// Si aucune adresse n'a été récupérée, affiche une erreur et bloque le processus
          while(1) { }		// Erreur Aucun TCS3471 détecté sur le bus I2C
     }

     Config_TCS3471();		// Configurer et activer TCS3471
}
//------------------------------------------------------------------------------------------//


//-------------------------------------MAIN LOOP OF PROGRAM---------------------------------//

int main() {		// Boucle principale du programme

    setup();		// Configurer le PIC16F877 et les composants attachés

    while(1){		// Boucle infinie après la configuration du programme, "setup" ne doit s'exécuter qu'une seule fois

         if(UART1_Data_Ready()){			// Si des données disponibles

              int rxCount = 0;			// Déclarer le compteur, suivre la longueur du message

              while(UART1_Data_Ready()){		// Si les données sont disponibles dans Rx
                   buff[rxCount] = UART1_Read();	// Charger les données dans le buffer
                   rxCount += 1;			// Compter l'octet de données lues

                   if(rxCount == 9){			// Lorsque le message de longueur 9 est reçu
                        connexion = checkMsgBT(buff); // Testez le message reçu
                        break;			// Quitter la boucle
                   }
              }
              FlushRx();				// Effacer le buffer d'entrée Rx
         }

    	 // Si RB1 est bas, l'interruption TCS3471 a été déclenchée, alors lisez la couleur
         if(!PORTB.B1){

              clearInterrupt();			// Réinitialiser l'interruption

              clear = readTwoReg(TCS3471_CLEAR);	// Lire les données de couleur clair
              red = readTwoReg(TCS3471_RED);		// Lire les données de couleur rouge
              green = readTwoReg(TCS3471_GREEN);	// Lire les données de couleur vert
              blue = readTwoReg(TCS3471_BLUE);	// Lire les données de couleur bleu
         }


         // Afficher les valeurs TCS3471 sur l'affichage à 7 segments
         // Les valeurs sont simplifiées normalement elles peuvent aller de 0 à 65535 (deux octets non signés)
         // Mais il n'y a que 4 écrans 7Seg, donc les valeurs sont mises à l'échelle de 0 à 9
         // Disp3 (le plus à gauche) = valeur clair; Disp2 (centre gauche) = valeur rouge;
         // Disp1 (centre droit) = valeur verte; Disp0 (le plus à droite) = valeur bleue
         updateDisplay(dispNums[scaleToDisp(clear)],
                       dispNums[scaleToDisp(red)],
                       dispNums[scaleToDisp(green)],
                       dispNums[scaleToDisp(blue)]);


         // Si la connexion Bluetooth est ouverte
         if(connexion){
              if(UART1_Tx_Idle()){				// Si UART Tx est ouvert pour transmettre
                   writeBT(prepMsg(clear, red, green, blue));// Transmettez les données
              }
         }
         // Ajouter un léger retard pour gérer le débit de données
         Delay_ms(50);
    }
}
//------------------------------------------------------------------------------------------//


//-------------------------------BLUETOOTH FUNCTIONS----------------------------------------//

// Configurer le nom et le mot de passe du module Bluetooth
void Config_Bluetooth() {
     UART1_Write_Text(blueName1);	// Essayez de définir le nom Bluetooth avec la syntaxe du Firmware 1

     if(ErrorCheck()) {		// Vérifiez si le module utilise Firmware 1, sinon il y aura un erreur
          UART1_Write_Text(bluePin1);	// Définir le mot de passe Bluetooth avec la syntaxe du Firmware 1
          FlushRx();			// Vider la réponse du buffer Rx

     } else {				// Else, si l'erreur est renvoyée, utilisez la syntaxe du Firmware 1
          UART1_Write_Text(blueName2);// Définir le nom Bluetooth avec la syntaxe du Firmware 2
          FlushRx();			// Vider la réponse du buffer Rx
          UART1_Write_Text(bluePin2);	// Définir le mot de passe Bluetooth avec la syntaxe du Firmware 2
          FlushRx();			// Vider la réponse du buffer Rx
     }
}


// Vérifiez si le message reçu via Bluetooth est valide
int checkMsgBT(char* msg){
     int i, c = 0, d = 0;		// Déclarer les variables, i = index, c = connecter, d = déconnecter
     for(i = 0; i < 9; i++){		// Pour tous les caractères du message entrant

     	  if(msg[i] != connectBT[i]){	// Si le caractère ne correspond pas à connect msg lever un drapeau
     	       c = 1;			// Indiquer que le message n'est pas un message de connexion valide
     	  }
     	  if(msg[i] != disconnBT[i]){	// Si le caractère ne correspond pas au message de déconnexion, lever un drapeau
     	       d = 1;			// Indiquer que le message n'est pas un message de déconnexion valide
      	  }
     }
     if(c == 0){			// S'il s'agit d'un message de connexion valide
          writeBT(conn);		// Confirmer la connexion
          return 1;			// Définir l'indicateur de connexion sur 1
     }
     if(d == 0){			// S'il s'agit d'un message de déconnexion valide
          writeBT(disc);		// Confirmer la déconnexion
          return 0;			// Définir l'indicateur de connexion sur 0
     }
     writeBT(erro);			// Sinon, le message est une erreur, avertissez l'expéditeur
     return connexion;			// Ne pas modifier l'indicateur de connexion
}


// Ecrire un message au module Bluetooth via UART Serial
void writeBT(char* msg){
    int i;                              // Déclarer la variable d'index
    for(i = 0; i < MSG_LEN; i++){	// Pour les caractères dans la chaîne de caractères
      UART1_Write(msg[i]);		// Envoyez les caractères au module Bluetooth
    }
    //Terminer le message avec un délimiteur
    UART1_Write('\r');			// envoyer d'abord un carriage return
    UART1_Write('\n');			// Puis envoyez un line break
}


// Vider le buffer Rx
void FlushRx() {
     while(UART1_Data_Ready()){	// Tant que les données sont disponibles
          UART1_Read();		// Supprimer l'octet du buffer
     }
}


// Formate les données RGBC afin de les transmettre via Bluetooth
char *prepMsg(unsigned short clear, unsigned short red, unsigned short green, unsigned short blue) {
     // Mettre la valeur clair à hexadecimal (mis à l'échelle à un octet)
     msg[5]  = getMSChar(scaleToBT(clear));	// Clair, 4 bits les plus significatifs
     msg[6]  = getLSChar(scaleToBT(clear));	// Clair, 4 bits les moins significatifs

     // Mettre la valeur rouge à hexadecimal (mis à l'échelle à un octet)
     msg[7]  = getMSChar(scaleToBT(red));	// Rouge, 4 bits les plus significatifs
     msg[8]  = getLSChar(scaleToBT(red));	// Rouge, 4 bits les moins significatifs

     // Mettre la valeur vert à hexadecimal (mis à l'échelle à un octet)
     msg[9]  = getMSChar(scaleToBT(green));	// Vert, 4 bits les plus significatifs
     msg[10] = getLSChar(scaleToBT(green));	// Vert, 4 bits les moins significatifs

     // Mettre la valeur bleu à hexadecimal (mis à l'échelle à un octet)
     msg[11] = getMSChar(scaleToBT(blue));	// Bleu, 4 bits les plus significatifs
     msg[12] = getLSChar(scaleToBT(blue));	// Bleu, 4 bits les moins significatifs

     return msg;				// Retourne le msg du tableau de caractères
}


// Échelle 0-65535 unsigned short à 0-255 Int pour l'envoyer via Bluetooth
int scaleToBT(unsigned short color){
     return (int)((long)(color * 255) / 65535);// Renvoie un entier de 0 à 255
}


// Déclarez toutes les valeurs hexadécimales possibles sous forme de caractères
char hexChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

char getMSChar(int val){	// Renvoie le caractère hexadécimal le plus significatif a partir d'un entier
     return hexChars[val / 16];// Divisez la valeur par 16 et utilisez le résultat pour rechercher le caractère hexadécimal du tableau
}


char getLSChar(int val){	// Renvoie le caractère hexadécimal le moins significatif a partir d'un entier
     return hexChars[val % 16];// Trouvez le modulo de valeur et utilisez le résultat pour rechercher le caractère hexadécimal du tableau
}


// Message d'erreur de la commande AT du module Bluetooth
char error[8] = {'E', 'R', 'R', 'O', 'R', '(', '0', ')'};

// Vérifie si la réponse au message AT du module Bluetooth est une erreur
int ErrorCheck() {
     int i = 0;			// Déclarer une variable d'index
     while(UART1_Data_Ready()){	// Pendant que les données sont disponibles dans le buffer Rx
          if( i < 8 ){			// et alors que les octets entrants sont inférieurs à la longueur d'un message d'erreur
               buff[i] = UART1_Read(); // Lire l'octet dans le tableau buffer
               i += 1;			// Incrémenter l'index
          } else {			// Else, Après 8 octets ont été reçus
          	break;			// Quitter la boucle
          }
     }
     if(strncmp(error, buff, 8)) {	// Si le message reçu est identique à l'erreur
          return 0;			// Retour 0
     }
     FlushRx();			// Vider la réponse du buffer Rx
     return 1;				// Retour 1
}
//------------------------------------------------------------------------------------------//


//-------------------------------7 SEGMENT DISPLAY FUNCTIONS--------------------------------//

// Échelle 0-65535 unsigned short à 0-9 Int pour l'affichage sur l'affichage à 7 segments
int scaleToDisp(unsigned short color){
     return (int)((long)(color * 9) / 65535);	// Renvoie un entier de 0 à 9
}


//Display characters                  "C"   "r"   "g"   "b"   "o"   "L"   " "
static const uint8_t dispChars[] = { 0x39, 0x50, 0x6F, 0x7C, 0x5C, 0x38, 0x00 };

// Init 7 Segment affichage et affiche un message de test
void initDisplay() {
     int count = 100;		// Déclarer l'index du décompte
     // Afficher "CoLr" sur l'affichage 7 segments pendant environ 800 ms
     while(count > 0){
          //Affiche "CoLr" sur l'affichage 7 segments
          updateDisplay(dispChars[0], dispChars[4], dispChars[5], dispChars[1]);
          count -= 1;		// Décrémenter count
     }

     PORTD = dispChars[0];	// Réglez PORTD pour afficher "C"
     PORTA.B3 = 1;		// Activer Disp3
     Delay_ms(300);		// Delay 300ms
     PORTA.B3 = 0;		// Désactiver Disp3

     PORTD = dispChars[1];	// Réglez PORTD pour afficher "r"
     PORTA.B2 = 1;		// Activer Disp2
     Delay_ms(300);		// Delay 300ms
     PORTA.B2 = 0;		// Désactiver Disp2

     PORTD = dispChars[2];	// Réglez PORTD pour afficher "g"
     PORTA.B1 = 1;		// Activer Disp1
     Delay_ms(300);		// Delay 300ms
     PORTA.B1 = 0;		// Désactiver Disp1

     PORTD = dispChars[0];	// Réglez PORTD pour afficher "b"
     PORTA.B0 = 1;		// Activer Disp0
     Delay_ms(300);		// Delay 300ms
     PORTA.B0 = 0;		// Désactiver Disp0
}


// Un cycle de l'affichage à 7 segments
void updateDisplay(uint8_t dis3, uint8_t dis2, uint8_t dis1, uint8_t dis0) {
    PORTD = dis3;	// Définir la valeur pour DISP3
    PORTA.B3 = 1;	// Allumez DISP3 (A3)
    Delay_ms(2);	// Delay 2ms
    PORTA.B3 = 0;	// Éteignez DISP3 (A3)

    PORTD = dis2;	// Définir la valeur pour DISP2
    PORTA.B2 = 1;	// Allumez DISP2 (A2)
    Delay_ms(2);	// Delay 2ms
    PORTA.B2 = 0;	// Éteignez DISP2 (A2)

    PORTD = dis1;	// Définir la valeur pour DISP1
    PORTA.B1 = 1;	// Allumez DISP1 (A1)
    Delay_ms(2);	// Delay 2ms
    PORTA.B1 = 0;	// Éteignez DISP1 (A1)

    PORTD = dis0;	// Définir la valeur pour DISP0
    PORTA.B0 = 1;	// Allumez DISP0 (A0)
    Delay_ms(2);	// Delay 2ms
    PORTA.B0 = 0;	// Éteignez DISP0 (A0)
}
//------------------------------------------------------------------------------------------//


//---------------------------------TCS3471 COLOR CLICK FUNCTIONS----------------------------//

// TCS3471 Renvoie l'adresse I2C, valide la puce est opérationnelle dans le circuit
uint8_t Init_TCS3471() {
        uint8_t checkByte;		// Déclarer l'octet pour stocker la valeur renvoyée par I2C Read
        I2C1_Start();			// Démarrez I2C aka vérifiez si le bus I2C est libre
        I2C1_Wr(TCS3471_ADDRESS_1);	// Essayez l'adresse 1 0x29 aka écrire sur le bus I2C
        I2C1_Wr(TCS3471_ID);		// Écrire dans le registre d'identification pour en lire
        checkByte = I2C1_Rd(1u);	// Lire la réponse du TCS3471 (avec ACK)
        I2C1_Stop();			// Passer le contrôle du bus I2C

        // Si une réponse valide reçue du TCS3471, l'adresse est correcte
        if(checkByte == 0x14 || checkByte == 0x1D){
               return 0x29;		// Renvoyer l'adresse valide

        } else {				// Si la première adresse n'est pas valide
               I2C1_Start();			// Démarrez I2C aka vérifiez si le bus I2C est libre
               I2C1_Wr(TCS3471_ADDRESS_2);	// Essayez la deuxième adresse, envoyez-la via I2c
               I2C1_Wr(TCS3471_ID);		// Écrire dans le registre d'identification pour en lire
               checkByte = I2C1_Rd(1u);	// Lire la réponse du TCS3471 (avec ACK)
               I2C1_Stop();			// Passer le contrôle du bus I2C

               // Si une réponse valide reçue du TCS3471, l'adresse est correcte
               if(checkByte == 0x14 || checkByte == 0x1D){
                            return 0x39;	// Renvoyer l'adresse valide
               }
        }
        return 0;	// Erreur TCS3471 non détectée sur le bus I2C
}


// Configurer et activer TCS3471
uint8_t Config_TCS3471() {
        writeToReg(TCS3471_ENABLE, TCS3471_AIEN_BIT);  // Activer TCS3471 Interrupt

        // RGBC Timing Register, 0x00 est le temps maximum de 700 ms et 0xFF est
        // le temps le plus rapide de 2,4 ms. 0xD5 est le temps d'intégration de 101 ms
        writeToReg(TCS3471_ATIME, 0xD5);

        // Définissez le registre de temps d'attente 0x00 614 ms sur 0xFF 2,4 ms
        // si WLONG, l'heure définie est 12 fois la valeur. ex. 0x00 avec WLONG est 7.4sec
        // 0xAB est 204ms avec WLONG = 0
        writeToReg(TCS3471_WTIME, 0xAB);

        // Désactiver WLONG, réglez CONFIG sur 0x02 pour activer WLONG
        writeToReg(TCS3471_CONFIG, 0x00);

        // Activer la minuterie d'attente, si elle est désactivée, le temps d'attente par défaut est de 2,4 ms
        writeToReg(TCS3471_ENABLE, TCS3471_WEN_BIT);

        // Réglez le gain pour les ADC, le gain de 1X convient à la plupart des cas
        writeToReg(TCS3471_CONFIG, GAIN_1X);

        writeTwoReg(TCS3471_AIHTL, 32768);	// Définir le seuil supérieur pour le canal clair

        writeTwoReg(TCS3471_AILTL, 32767);	// Définir le seuil inférieur pour le canal clair

        // Définir la persistance d'interruption, le nombre de fois que le seuil doit être franchi pour définir l'interruption
        // Accepte une valeur de 4 bits 0 signifie que chaque cycle définit l'interruption
        // 0x01 signifie qu'il faut franchir le seuil une fois pour définir l'interruption
        writeToReg(TCS3471_PERS, 0x01);

        clearInterrupt();	// Réinitialiser l'interruption pour éviter un raccrochage potentiel

        writeToReg(TCS3471_ENABLE, TCS3471_AEN_BIT);	// Activer ADC à deux canaux et activer RGBC

        writeToReg(TCS3471_ENABLE, TCS3471_PON_BIT);	// Activer l'oscillateur
}


// Réinitialise l'interruption de seuil TCS3471
void clearInterrupt(){
     I2C1_Start();		// Démarrer la transmission I2C
     I2C1_Wr(address);		// Ecrire à l'adresse
     				// Définir le bit d'interruption de réinitialisation
     I2C1_Wr(TCS3471_SPECIAL_BIT | TCS3471_INTCLEAR_BIT);
     I2C1_Stop();		// Terminer la transmission I2C
}


// Ecrire une valeur d'un octet dans un registre
void writeToReg(uint8_t reg, uint8_t dat) {
     I2C1_Start();		// Démarrer la transmission I2C
     I2C1_Wr(address);		// Ecrire à l'adresse
     I2C1_Wr(reg);		// Ecrire pour vous inscrire
     I2C1_Wr(dat);		// Écrire l'octet de données
     I2C1_Stop();		// Terminer la transmission I2C
}


// Ecrire une valeur de deux octets dans un registre
void writeTwoReg(uint8_t reg, unsigned short dat) {
     I2C1_Start();				// Démarrer la transmission I2C
     I2C1_Wr(address);				// Ecrire à l'adresse
     I2C1_Wr(TCS3471_AUTOINCR_BIT | reg);	// Écrire pour s'inscrire avec un jeu d'auto-incrémentation
     I2C1_Wr(dat & 0xFF);			// Ecrire l'octet le plus significatif de dat
     I2C1_Wr((dat >> 8) & 0xFF);		// Ecrire l'octet le moins significatif de dat
     I2C1_Stop();				// Terminer la transmission I2C
}


// Lire deux octets de TCS3471 via I2C
unsigned short readTwoReg(uint8_t reg) {
         uint8_t dat1;		// Déclarer la variable pour le 1er octet reçu
         uint8_t dat2;		// Déclarer la variable pour le 2e octet reçu
         			// Configurer le registre de lecture
         uint8_t readReg = TCS3471_COMMAND_BIT | TCS3471_AUTOINCR_BIT | reg;
         I2C1_Start();		// Démarrer la transmission I2C
         I2C1_Wr(address);	// Ecrire à l'adresse
         I2C1_Wr(readReg);	// Envoyer le registre
         dat1 = I2C1_Rd(1u);	// Lire le premier octet
         dat2 = I2C1_Rd(1u);	// Lire le deuxième octet
         I2C1_Stop();		// Terminer la transmission I2C

         			// Joindre deux octets et renvoyer en tant que unsigned short
         return (((uint16_t)dat2 << 8) | dat1);
}
//------------------------------------------------------------------------------------------//

