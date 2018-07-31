"""
Convertis notre fichier d'entrée séparé par des ; et en sort un .csv

Vous pouvez spécifier une limite de ligne à lire en argument principal
Ex : Si on veut couper à la 10ème ligne : python3 converter.py 10
"""
import sys

if __name__ == '__main__':
    with open('../data/SENSOR41', 'r') as input:
        with open('../data/sensor_output.csv', 'w') as output:
            for counter, line in enumerate(input):
                if len(sys.argv) > 1 :
                    if int(sys.argv[1]) > counter:
                        for char in line:
                            if char == ';':
                                output.write(",")
                            else:
                                output.write(char)
