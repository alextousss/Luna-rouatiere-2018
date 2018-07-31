"""
Converti sensor_output.csv en sensor_output_celcius.csv
Enlève les valeurs aberrantes et change les unités :
- Kelvins vers celcius
- Pascals vers hPascals
- Secondes vers minutes
- Ajoute une dernière colonne, altitude avec l'équation de la pression  barométrique
"""
import csv

if __name__ == '__main__':
    with open('../data/sensor_output.csv', 'r') as input:
        with open('../data/sensor_output_celcius.csv', 'w') as output:
            csv_reader = csv.reader(input, delimiter=',')
            for row in csv_reader:
                for index, column in enumerate(row):
                    if column:
                        data = float(column)
                        if index == 0:
                            data /= 60
                        if 0 < index < 6:
                            data -= 273.75
                        elif index == 6:
                            data /=100

                        if 273.75 + -60 < float(row[1]) < 273.75 + 40:
                            output.write(str(data) + ", ")

                output.write("\n")
