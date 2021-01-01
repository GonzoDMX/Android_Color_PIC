package com.example.pic_bluetooth

/*
*       ANDREW O'SHEI
*       Projet - Realisation de Projets 1er Semestre
 */

import android.Manifest
import android.annotation.SuppressLint
import android.app.AlertDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Bundle
import android.text.method.ScrollingMovementMethod
import android.util.Log
import android.widget.ArrayAdapter
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.activity_main.*
import java.util.*
import kotlin.math.abs
import kotlin.math.log10

// Déclarer la baie contient l'ID de périphérique Bluetooth
var devices = ArrayList<BluetoothDevice>()
// Déclarer HashMap contient les adresses de l'appareil Bluetooth
var devicesMap = HashMap<String, BluetoothDevice>()
// Déclarer l'adaptateur Array, contient des chaînes pour Bluetooth AlertDialog
var mArrayAdapter: ArrayAdapter<String>? = null
// Déclarer l'UUID Bluetooth pour l'application
val uuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

// Déclarer le compteur de messages, compte le nombre de messages Bluetooth reçus
var msgCount = 0
// Déclarez le suivi des messages, garde la dernière ligne de l'enregistreur de messages au point
var msgTrack = 0
// Déclarer l'indicateur d'état de la connexion
var connexion = false
// Declare string, pour passer des messages de MainActivity à BluetoothClient Class
var message = ""
// Déclarer l'heure de début, conserve l'heure de début du processus de connexion
var startTime: Long = 0
// Indicateur de fin de connexion, indique quand l'utilisateur a fermé la connexion pour que le fil puisse se fermer
var terminateConn = false

// MainActivity Call, Fondamentalement, la boucle principale du programme
class MainActivity : AppCompatActivity() {

    // Fonction déclenchée lorsque l'activité est créée par le cycle Android Activity
    override fun onCreate(savedInstanceState: Bundle?) {
        // Hériter de la classe d'activité
        super.onCreate(savedInstanceState)
        // Définir le layout de l'activité
        setContentView(R.layout.activity_main)

        // Réglez le journal de texte en noir (pour qu'il soit visible en mode nuit)
        textLog.setTextColor(Color.BLACK)
        // Activer l'affichage du texte du journal de texte déroulant
        textLog.movementMethod = ScrollingMovementMethod()

        // Si les autorisations Bluetooth ne sont pas définies
        if (ContextCompat.checkSelfPermission(this,
                        Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            // Demandez l'autorisation d'accéder au Bluetooth lors du premier lancement de l'application
            ActivityCompat.requestPermissions(this,
                    arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                    1)
        }
        // Définir le layout de la liste des appareils AlertDialog
        mArrayAdapter = ArrayAdapter(this, R.layout.dialog_select_device)

        // Lier le bouton d'effacement du journal à l'action
        buttonClear.setOnClickListener {
            // Lorsque le bouton d'effacement du journal est enfoncé, efface le journal
            textLog.text = ""
            // Réinitialiser le suivi des messages
            msgTrack = 0
            // Réinitialiser le focus de l'enregistreur de messages
            textLog.scrollTo(0, 0)
        }

        // Lier le bouton de connexion à l'action
        buttonConn.setOnClickListener { view ->
            // Si le Bluetooth n'existe pas sur l'appareil
            if (BluetoothAdapter.getDefaultAdapter() == null) {
                // Afficher la boîte de dialogue d'erreur
                Snackbar.make(view, "Bluetooth is disabled", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show()
            }
            // Vérifiez que le Bluetooth est activé
            else if(!BluetoothAdapter.getDefaultAdapter().isEnabled){
                // Réinitialiser l'aperçu des couleurs
                clearPreview()
                // Effacer l'enregistreur de messages
                textLog.text = ""
                // Afficher un message d'erreur
                writeError("Bluetooth is not enabled\nEnable bluetooth and retry!")
            } else {
                // Réglez le journal de texte en noir (pour qu'il soit visible en mode nuit)
                textLog.setTextColor(Color.BLACK)
                // Initialiser la variable d'appareil, stocke temporairement l'appareil de BluetoothAdapter
                devices = ArrayList()
                // Réinitialiser la liste des appareils
                mArrayAdapter!!.clear()
                // Si la connexion est active
                if(connexion) {
                    // Définir le message de déconnexion
                    message = "<DISCONN>"
                    // Déclarer la connexion fermée
                    connexion = false
                    // Modifier le texte sur le bouton de connexion
                    buttonConn.text = "Connect"
                // Si la connexion n'est pas active
                } else {
                    // Définir le message de connexion
                    message = "<CONNECT>"
                    // Déclarer la connexion ouverte
                    connexion = true
                    // Réinitialiser l'indicateur de fin de connexion
                    terminateConn = false
                    // Modifier le texte sur le bouton de connexion
                    buttonConn.text = "Disconnect"
                    // Effacer l'enregistreur de messages
                    textLog.text = ""
                    // Réinitialiser le focus de l'enregistreur de messages
                    textLog.scrollTo(0, 0)
                    // Réinitialiser le compteur de messages
                    msgCount = 0
                    // Réinitialiser le suivi des messages
                    msgTrack = 0
                    // Verrouiller le bouton de connexion, évite les erreurs de faux état
                    buttonConn.isEnabled = false
                    // Réinitialiser l'aperçu des couleurs
                    clearPreview()
                }

                // Effacer le hashmap, évite les erreurs si l'appareil n'est pas couplé pendant que l'application est active
                devicesMap = HashMap<String, BluetoothDevice>()
                // Pour tous les appareils couplés dans l'adaptateur Bluetooth
                for (device in BluetoothAdapter.getDefaultAdapter().bondedDevices) {
                    // Charger l'appareil avec l'adresse associée dans la carte des appareils
                    devicesMap[device.address] = device
                    // Ajouter un appareil à la liste des appareils
                    devices.add(device)
                    // Ajoutez le nom et l'adresse à un adaptateur de baie à afficher dans un ListView
                    mArrayAdapter!!.add((if (device.name != null) device.name else "Unknown") + "\n" + device.address + "\nPaired")
                }
                Log.d("Device_ADAPT", mArrayAdapter.toString())
                // Si le connexion est ouvert
                if (connexion) {
                    // Vérifiez qu'il existe des appareils couplés avant la découverte, sinon l'application se bloque
                    if(devicesMap.isNotEmpty()) {
                        // Lancer le processus de découverte
                        if (BluetoothAdapter.getDefaultAdapter().startDiscovery()) {
                            // Configurer le générateur AlertDialog
                            // Cette boîte de dialogue permet à l'utilisateur de sélectionner parmi les appareils Bluetooth couplés
                            val dialog: AlertDialog.Builder = AlertDialog.Builder(this)
                            // Définir le titre AlertDialog
                            dialog.setTitle("Connect to: ")
                            // Ne pas autoriser l'annulation des dialogues, cela évite un bug de faux état
                            dialog.setCancelable(false)
                            // Créer la liste des appareils couplés à partir de l'adaptateur et de l'action de liaison
                            dialog.setAdapter(mArrayAdapter) { _, which: Int ->
                                // Terminer le processus de découverte Bluetooth
                                BluetoothAdapter.getDefaultAdapter().cancelDiscovery()
                                // Démarrer le thread client Bluetooth avec le périphérique sélectionné
                                BluetoothClient(this, devices[which]).start()
                            }
                            // Rendre le AlertDialog
                            dialog.show()
                        }
                    } else {
                        // Déclarer la connexion fermée
                        connexion = false
                        // Modifier le texte sur le bouton de connexion
                        buttonConn.text = "Connect"
                        // Mettre à jour le journal des messages
                        writeError("Error: No paired devices detected!")
                        // Déverrouille le bouton de connexion
                        unlockButton()
                    }
                // Si le connexion est fermee
                } else {
                    // Définir l'indicateur pour fermer le thread client
                    terminateConn = true
                }
            }
        }
    }



    // Internationalisation Seppress, nous voulons une sortie brute et non une traduction
    @SuppressLint("SetTextI18n")
    // La fonction reçoit un message Bluetooth et analyse le message
    fun parseMessage(text: String) {
        // Déclarer un tableau pour stocker les valeurs de couleur reçues
        var values = IntArray(4)
        // Déclarer l'en-tête du message
        val check = "<TCS>"
        // Extraire l'en-tête du message en tant que sous-chaîne
        var header: String = text.subSequence(0, 5) as String
        // Extraire les valeurs de couleur de la chaîne
        var extVals: String = text.subSequence(5, 13) as String
        // Si l'en-tête correspond à <TCS>
        if(header == check){
            // Itérer les valeurs de couleur dans le message reçu
            for(i in 0 until 7 step 2) {
                // Convertir la valeur hexadécimale reçue en Int
                values[i / 2] = hexToInt(extVals[i], extVals[i + 1])
            }
            // Incrémenter le compteur de messages
            msgCount += 1
            // Déclarer le compteur de chaînes
            var counter: String = ""
            // Si le compteur est trop élevé, passez à 1
            if(msgCount > 999999999) { msgCount = 1 }
            // Obtenez le nombre de chiffres dans msgCount
            val lenCount: Int = msgCount.length()
            // Itérer sur la longueur du compteur
            for(i in 0 until 9 - lenCount) {
                // Ajoutez des espaces pour garder un espacement uniforme dans le journal
                counter += " "
            }
            // Ajouter le nombre de messages à la chaîne de comptage
            counter += msgCount.toString()
            // Ajouter une chaîne de format
            counter += ": "
            // Évitez les bogues où le thread se ferme avant la fin de la fonction
            if(connexion) {
                when {
                    // Lorsque le nombre de messages est supérieur à 99
                    msgCount > 99 -> {
                        // Espace approprié
                        writeToLog(counter + text + "\t\t")
                    }
                    //Lorsque le nombre de messages est supérieur à 999
                    msgCount > 999 -> {
                        // Espace approprié
                        writeToLog(counter + text + "\t")
                    }
                    // Lorsque le nombre de messages est supérieur à 9999
                    msgCount > 9999 -> {
                        // Espace approprié
                        writeToLog(counter + text)
                    }
                    else -> {
                        // Espace approprié
                        writeToLog(counter + text + "\t\t\t")
                    }
                }

                textClear.text = "${values[0]}"     // Définir la valeur actuelle affichée pour clair
                textRed.text   = "${values[1]}"     // Définir la valeur actuelle affichée pour rouge
                textGreen.text = "${values[2]}"     // Définir la valeur actuelle affichée pour vert
                textBlue.text  = "${values[3]}"     // Définir la valeur actuelle affichée pour bleu

                // Définir la couleur de l'imageView
                imageView.setColorFilter(Color.argb(values[0], values[1], values[2], values[3]))
            }
        // Si l'en-tête ne correspond pas à <TCS>
        } else {
            // Si le message reçu correspond
            if(text == "<CON>GOOD_CON\r\n"){
                // Espace approprié
                writeToLog("Recv:   $text\t")
            } else {
                // Écrit le message entrant dans l'enregistreur
                writeToLog("Recv:   $text\t\t")
            }
        }
    }

    // Réinitialiser l'aperçu des couleurs
    private fun clearPreview(){
        // Définir la couleur de l'imageView
        imageView.setColorFilter(Color.argb(0, 0, 0, 0))
        textClear.text = "0"     // Définir la valeur actuelle affichée pour clair
        textRed.text   = "0"     // Définir la valeur actuelle affichée pour rouge
        textGreen.text = "0"     // Définir la valeur actuelle affichée pour vert
        textBlue.text  = "0"     // Définir la valeur actuelle affichée pour bleu
    }

    // Internationalisation Seppress
    @SuppressLint("SetTextI18n")
    // La fonction écrit dans le journal des messages
    fun writeToLog(msg: String){
        // Formater la chaîne de message
        val writeVal = msg.replace("\n", "").replace("\r", "")
        // Mettre à jour le journal des messages
        textLog.text = textLog.text.toString() + writeVal + "\t\t\t\tTime(ms):  " +
                (System.currentTimeMillis() - startTime).toString() + "\n"
        // Incrémenter le suivi des messages
        msgTrack += 1
        // Si plus de 23 messages sont affichés dans l'enregistreur de messages
        if(msgTrack > 23) {
            // Définir le focus de l'enregistreur de messages sur le message le plus récent
            textLog.scrollTo(0, ((msgTrack - 23) * 41))
        }
    }

    // Convertir une valeur hexadécimale en entier
    private fun hexToInt(val1: Char, val2: Char) : Int {
        // Convertir le caractère 1 en entier
        var int1 = parseAscii(val1.toInt())
        // Convertir le caractère 2 en entier
        var int2 = parseAscii(val2.toInt())
        // Renvoie une valeur entière
        return (int1 * 16 + int2)
    }

    // Convertir un caractère ascii en entier
    private fun parseAscii(value: Int): Int{
        // Si char est de A à F
        return if(value > 64){
                // Ajuster la valeur de manière appropriée
            (value - 55)
        // Si char est de 0 a 9
        } else {
            // Ajuster la valeur de manière appropriée
            (value - 48)
        }
    }

    // Fonction pour obtenir le nombre de chiffres dans un entier
    private fun Int.length() = when(this) {
        // Si l'entier est 0
        0 -> 1
        // Sinon, renvoie le nombre de chiffres
        else -> log10(abs(toDouble())).toInt() + 1
    }

    // Écrire un message d'erreur
    fun writeError(msg: String){
        // Écrire du texte dans l'enregistreur de messages
        textLog.text = textLog.text.toString() + msg
    }

    // réinitialiser l'etat de la connexion
    fun resetConnection(){
        // Déclarer la connexion fermée
        connexion = false
        // Déclarer la connexion fermée
        terminateConn = true
        // Modifier le texte sur le bouton de connexion
        buttonConn.text = "Connect"
    }

    // Déverrouille le bouton de connexion
    fun unlockButton(){
        // Définir le bouton sur actif
        buttonConn.isEnabled = true
    }

    // Lorsque l'activité principale est fermée
    override fun onDestroy() {
        // Hériter de la classe d'activité
        super.onDestroy()
        // Si la connexion client Bluetooth est ouverte
        if(connexion) {
            // Mettre fin à la connexion
            terminateConn = true
        }
    }
}


