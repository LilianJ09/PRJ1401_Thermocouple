import socket
import matplotlib.pyplot as plt
import time

# Adresse IP et port du serveur
HOST = "192.168.154.156"
PORT = 1234

# Initialisation des listes de données
temps = []
Tension = []
Tsc = []

# Définition de la fonction pour mettre à jour le graphique
def update_graph():
    # Effacement du graphique précédent
    plt.clf()
    # Configuration du titre et des axes
    plt.title("TempératureSC et TensionmV en fonction du temps")
    plt.xlabel("Temps (s)")
    plt.ylabel("Valeurs")
    # Tracé des données de température
    plt.plot(temps, Tension, label="TensionmV", color="red")
    # Tracé des données d'humidité
    plt.plot(temps, Tsc, label="TempératureSC", color="blue")
    # Ajout de la légende
    plt.legend()
    # Affichage du graphique
    plt.draw()
    plt.pause(0.001)

plt.suptitle("Graphique de température et tension")

# Création du socket serveur
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # IPv4 et TCP
server_socket.bind((HOST, PORT))
server_socket.listen(1) # le serveur ecoute sur le port

print("Serveur en écoute sur {}:{}".format(HOST, PORT))

# Attente de la connexion d'un client
conn, addr = server_socket.accept()
print("Connexion établie avec {}:{}".format(addr[0], addr[1]))

i=0
while True:
    t = time.time()
    # Réception des données envoyées par le client
    data = conn.recv(1024)
    if not data:
        break
    #print(data)
    #strip() permet de supprimer les caractères d'espacement en début et fin de chaîne de caractères
    #split() permet de découper une chaîne de caractères en une liste de sous-chaînes en fonction d'un séparateur donné
    TensionmV,TempSC = data.decode('utf-8').strip().split(',')
    #affiche une f-string (chaine de caracteres formatee)
    print(f'TensionmV: {TensionmV} mV, TempSC: {TempSC} °C')

    # Ajout des données à la liste
    temps.append(t)
    Tension.append(TensionmV)
    Tsc.append(TempSC)
    # Mise à jour du graphique
    update_graph()
    
    # Affichage du graphique dans une fenêtre séparée pour permettre l'interaction avec l'utilisateur
    #if(i%10==0):
    #    plt.show() 

# Fermeture des sockets
conn.close()
conn.close()