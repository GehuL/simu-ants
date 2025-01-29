import matplotlib.pyplot as plt
import csv

# Lire les données du fichier CSV
generations = []
fitness_moyenne = []
fitness_max = []
fitness_min = []

with open('C:/Users/LENOVO/Desktop/simu-ants/fitness_moyenne_lab.csv', 'r') as file:
    reader = csv.reader(file)
    next(reader)  # Ignorer l'en-tête
    for row in reader:
        generations.append(int(row[0]))          # Génération
        fitness_moyenne.append(float(row[1]))    # Fitness moyenne
        fitness_max.append(float(row[2]))        # Fitness maximale
        fitness_min.append(float(row[3]))        # Fitness minimale

# Tracer les courbes
plt.figure(figsize=(10, 6))
plt.plot(generations, fitness_moyenne, marker='o', color='b', label='Fitness moyenne')
plt.plot(generations, fitness_max, marker='^', color='g', label='Fitness maximale')
plt.plot(generations, fitness_min, marker='v', color='r', label='Fitness minimale')

# Ajouter des titres et légendes
plt.title("Évolution des Fitness par Génération")
plt.xlabel("Génération")
plt.ylabel("Fitness")
plt.legend()
plt.grid(True)

# Afficher le graphique
plt.show()
