Überblick
=========

Das Ziel dieses Projektes ist die Simulation von Verkehrsflüssen. Es werden zum einen (mehrspurige) Autobahnen mit den Regeln des Nagel-Schreckenberg Modells bzw. STCA-Regeln für die Spurwechsel simuliert, wofür das Programm Multilane zuständig ist. Straßenverläufe beliebiger Form können mit dem Programm StreetMap simuliert werden, wobei ein Kreisverkehr und ein Autobahndreieck vorimplementiert sind.

Als Output produzieren beide Programme Bilder, die im Ordner output/ abgelegt werden, wofür die Library CImg genutzt wird. Zusätzlich wird eine Datei erzeugt, die die Verkehrsdichte in den einzelnen Iterationen enthält. Für beide Programme liegen Bash-Skripte bereit, die sowohl das Programm ausführen, als auch aus den Verkehrsdichten einen Plot erstellt (hierfür wird das Programm gnuplot benötigt). Die Skripte reichen alle übergebenen Parameter an das jeweilige Programm weiter.

Multilane
=========
Programm: bin/multilane
Skript: multilane.sh

Eine detailierte Übersicht über die Programmoptionen erhalt man mit "bin/multilane -h". Hier ein paar Anwendungsbeispiele:

* Einspurige Autobahn der Länge 300 Zellen mit zyklischen Randbedingungen (wrap-around), 150 Iterationen, Verkehrsdichte 0.1

  bin/multilane -x 300 -y 1 -w -i 150 -t 0.1

* Zweispurige Autobahn der Länge 300 Zellen mit gleichmäßig verteilten Autos bei der Initialisierung

  bin/multilane -x 300 -y 2 -p -e

* Vierspurige Autobahn der Länge 100 Zellen mit Trödelfaktor zwischen 0.01 und 0.1, lambda für l2r-Verteilung 10, für r2l 30

  bin/multilane -x 100 -y 4 -d 0.01 - D 0.1 -r 10 -R 30

StreetMap
=========
Programm: bin/streetmap
Skript: streetmap.sh

Eine detailierte Übersicht über die Programmoptionen erhalt man mit "bin/streetmap -h". Hier ein paar Anwendungsbeispiele:

* Kreisverkehr der Größe 10x6, 150 Iterationen, Fahrzeugerzeugungsrate 0.15

  bin/streetmap -r -w 10 -h 6 -i 150 -c 0.15

* Autobahnkreuz mit 14 Zellen Zufahrtsstraßen, Trödelfaktor zwischen 0.01 und 0.1

  bin/streetmap -x -l 14 -d 0.01 -D 0.1
