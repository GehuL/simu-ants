import matplotlib.pyplot as plt
import csv

# Lire les données du fichier CSV
generations = []
fitness_moyenne = []

with open('C:/Users/LENOVO/Desktop/simu-ants/fitness_moyenne_lab.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        generations.append(int(row[0]))  # Génération
        fitness_moyenne.append(float(row[1]))  # Fitness moyenne

# Tracer la courbe de fitness moyenne par génération
plt.plot(generations, fitness_moyenne, marker='o', color='b', label='Fitness moyenne')
plt.title("Évolution de la Fitness Moyenne")
plt.xlabel("Génération")
plt.ylabel("Fitness Moyenne")
plt.legend()

# Afficher le graphique
plt.show()
