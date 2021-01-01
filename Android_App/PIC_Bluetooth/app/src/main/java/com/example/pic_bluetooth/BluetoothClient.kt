package com.example.pic_bluetooth

/*
*       ANDREW O'SHEI
*       Projet - Realisation de Projets 1er Semestre
 */

import android.bluetooth.BluetoothDevice
import kotlinx.android.synthetic.main.activity_main.*
import java.io.IOException


// Déclarer la classe client Bluetooth et hériter du thread
class BluetoothClient(private val activity: MainActivity, device: BluetoothDevice): Thread() {
    // Déclarez la prise Bluetooth et définissez l'UUID
    private val socket = device.createRfcommSocketToServiceRecord(uuid)
    // Fonction appelée au démarrage du thread
    override fun run() {
        // Si la socket ne renvoie pas null
        if(this.socket != null) {
            // Définir l'heure de début du processus
            startTime = System.currentTimeMillis()
            // Mettre à jour le journal des messages
            writeLog("Connecting...\t\t\t\t\t\t\t\t\t\t\t")
            // Retard de 250 millisecondes
            sleep(250)
            try {
                // Connectez le client socket
                this.socket.connect()
                // Déclarer le flux de sortie pour les messages sortants
                val outputStream = this.socket.outputStream
                // Déclarer le flux d'entrée pour les messages entrants
                val inputStream = this.socket.inputStream
                // Essayez, pour éviter de planter le programme en cas d'erreur
                try {
                    // Écrire un message dans le flux de sortie
                    outputStream.write(message.toByteArray())
                    // Assurez-vous que tout le message est envoyé
                    outputStream.flush()
                    // Pendant que la connexion est en boucle ouverte
                    while (!terminateConn) {
                        // Déverrouille le bouton de connexion
                        unlock()
                        // Recevoir des messages du flux d'entrée
                        recMsg()
                    }
                    // Écrire un message dans le flux de sortie
                    outputStream.write(message.toByteArray())
                    // Assurez-vous que tout le message est envoyé
                    outputStream.flush()
                    // Mettre à jour le journal des messages
                    writeLog("Closing Connection...\t\t\t\t\t")
                    // Retard de 500 millisecondes
                    sleep(500)
                    // Recevoir un message de déconnexion avant de fermer
                    recMsg()
                // S'il y a une exception
                } catch (e: Exception) {
                    // Ecrire le message d'erreur dans le journal des messages
                    writeLog("Connection Failed!\t\t\t\t\t\t\t")
                    // Ecrire le message d'erreur dans le journal des messages
                    writeError("Error: $e")
                    // réinitialiser la connexion
                    resetConnex()
                // finalement
                } finally {
                    // Fermer le flux de sortie
                    outputStream.close()
                    // Fermer le flux d'entrée
                    inputStream.close()
                    // Fermer le socket
                    this.socket.close()
                    // Mettre à jour le journal des messages
                    writeLog("Connection Closed.\t\t\t\t\t\t")
                }
            // S'il y a une exception
            } catch (e: IOException) {
                // Ecrire le message d'erreur dans le journal des messages
                writeLog("Connection Failed!\t\t\t\t\t\t\t")
                // Ecrire le message d'erreur dans le journal des messages
                writeError("Bluetooth Device not connected!")
                // réinitialiser la connexion
                resetConnex()
            }
        // Si la socket renvoie null
        } else {
            // Ecrire le message d'erreur dans le journal des messages
            writeError("Connection Failed!\n")
            // Ecrire le message d'erreur dans le journal des messages
            writeError("Error: Failed to connect socket")
            // réinitialiser la connexion
            resetConnex()
        }
    }

    // Ecrire un message dans le Log
    private fun writeLog(msg: String){
        // Pour éviter les avertissements, exécutez sur le thread d'interface utilisateur
        activity.runOnUiThread(Runnable {
            // Mettre à jour le journal des messages
            activity.writeToLog(msg)
        })
    }

    // Ecrire un message d'erreur dans le Log
    private fun writeError(msg: String){
        // Pour éviter les avertissements, exécutez sur le thread d'interface utilisateur
        activity.runOnUiThread(Runnable {
            // Mettre à jour le journal des messages
            activity.writeError(msg)
        })
    }

    private fun resetConnex(){
        // Pour éviter les avertissements, exécutez sur le thread d'interface utilisateur
        activity.runOnUiThread(Runnable {
            // réinitialiser la connexion
            activity.resetConnection()
            // Déverrouille le bouton de connexion
            activity.unlockButton()
        })
    }

    private fun unlock(){
        // Pour éviter les avertissements, exécutez sur le thread d'interface utilisateur
        activity.runOnUiThread(Runnable {
            // Déverrouille le bouton de connexion
            activity.unlockButton()
        })
    }

    // Envoyer le message reçu à la fonction parseMessage dans MainActivity
    private fun parse(msg: String){
        // Pour éviter les avertissements, exécutez sur le thread d'interface utilisateur
        activity.runOnUiThread(Runnable {
            // Envoyer un message à l'analyseur de messages pour traitement
            activity.parseMessage(msg)
        })
    }

    // La fonction reçoit des messages du flux d'entrée
    private fun recMsg(){
        // Obtenir le nombre d'octets dans le flux d'entrée
        val available = this.socket.inputStream.available()
        // Si plus de 13 octets sont disponibles
        if(available > 13) {
            // Déclarer un tableau d'octets vide de la taille du message entrant
            val bytes = ByteArray(available)
            // Lire le message entrant et écrire dans le tableau d'octets
            this.socket.inputStream.read(bytes, 0, available)
            // Convertir un tableau d'octets en chaîne
            val text = String(bytes)
            // Envoyer un message à l'analyseur de messages pour traitement
            parse(text)
        }
    }
}