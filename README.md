Überblick
=========

Das Ziel dieses Projektes ist die Simulation von Verkehrsflüssen. Es werden zum einen (mehrspurige) Autobahnen mit den Regeln des Nagel-Schreckenberg Modells bzw. STCA-Regeln für die Spurwechsel simuliert, wofür das Programm Multilane zuständig ist. Straßenverläufe beliebiger Form können mit dem Programm StreetMap simuliert werden, wobei ein Kreisverkehr und ein Autobahndreieck vorimplementiert sind.

Als Output produzieren beide Programme Bilder, die im Ordner output/ abgelegt werden, wofür die Library CImg genutzt wird. Zusätzlich wird eine Datei erzeugt, die die Verkehrsdichte in den einzelnen Iterationen enthält. Für beide Programme liegen Bash-Skripte bereit, die sowohl das Programm ausführen, als auch aus den Verkehrsdichten einen Plot erstellt (hierfür wird das Programm gnuplot benötigt).

Multilane
=========


StreetMap
=========
Per default arbeitet StreetMap auf einem Kreisverkehr der 8 Zellen breit und 6 Zellen hoch ist.
