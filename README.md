# OpenGL Car Racing Game 
In diesem Projekt haben wir ein kleines Autorennspiel gebaut.

## Features
- Steuerung des Autos
- Straßenrundkurs
- Checkpoints und Ziellinie
- Rundenzeiten und Gesamtzeit
	- Inkl. Zeitdifferenz zu Rekord
- Geist (zweites Auto), den Rekord repräsentiert
- Skybox

## Steuerung
- `↑` forwärts fahren (beschleunigen)
- `↓` rückwärts fahren (bremsen und negativ beschleunigen)
- `←` nach links lenken 
- `→` nach rechts lenken 
- `leertaste` bremsen
Wird weder positiv noch negativ beschleunigt, so wird leicht gebremst.


## Straßenrundkurs
Die Straße, Checkpoint, Ziellinie und Landschaft wurden mit Maya erstellt und als .fbx 3D-Modell exportiert und dann mit ASSIMP in das Programm eingebunden.

## Checkpoints, Ziellinie und Zeiten
Auf der Rennstrecke gibt es eine Ziellinie und einen Checkpoint, diese werden genutzt um die Rundenzeiten und Zwischenzeiten zu ermitteln. Die Zeiten werden auf der Konsole ausgegeben. Nach einem kompletten Rennen, was standardmäßig aus 3 Runden besteht wird die Gesamtzeit mit der aktuellen Bestzeit verglichen, falls noch keine Bestzeit existiert oder die aktuelle Zeit schneller ist wird der Rekord in einer Datei gespeichert/überschrieben. In der Datei werden die Rundenzeiten und die Model-Matrizen der Fahrt mit den zugehörigen Zeitstempeln gespeichert.

![Zeitenausgabe 1](zeitenausgabe_1.png)
Durchfahren von Checkpoints und der Ziellinie (sofern alle Checkpoints der Runde durchfahren sind) führt zu Konsolenausgaben mit Zeitangaben. Da alle Runden vollendet wurden und noch kein Rekord Vorlag wird beim Beenden des Programms die Fahrt in dem unter dem angegebenen Pfad gespeichert und wird bis zur aktualisierung von dem Geist wiederholt werden.

![Zeitenausgabe 2](zeitenausgabe_2.png)
Zeitdifferenzen beim vollenden von Runden werden in der Konsole ebenfalls ausgegeben. Da die Gesamtzeit geringer als die bisherige Bestzeit war wird die Rekord-Datei aktualisiert. 

## Geist
Existiert ein Rekord, so wird dieser durch ein zweites Auto (den "Geist") repräsentiert, welches anhand der in der Datei gespeicherten und Zeitstempeln zugeorndeten Model-Matrizen bewegt wird. 

## Resourcen und Quellen
- Benutzte Libraries
	- glfw
	- glew
	- SOIL
	- ASSIMP
- Resourcen
	- [open.gl](https://open.gl/)
	- [learnopengl.com](https://learnopengl.com/)
	- [SonarSystems/Modern-OpenGL-Tutorials](https://github.com/SonarSystems/Modern-OpenGL-Tutorials)
		- Verwendet beim Modelloading ([Code Modelloading](https://github.com/SonarSystems/Modern-OpenGL-Tutorials/tree/master/%5BMODEL%20LOADING%5D))
		- Verwendet für die Skybox ([Code Skybox](https://github.com/SonarSystems/Modern-OpenGL-Tutorials/tree/master/%5BADVANCED%20OPENGL%5D/%5B17%5D%20Cubemap:Skybox))
    	- [Car Model](https://free3d.com/3d-model/chevrolet-camaro-ss-coupe-373476.html) 
		- Model des Autos, aus der kostenlos verfügbaren Blender Datei exportiert. 
    	- Code aus den Übungen
	- [Stackoverflow Read floats from a .txt file](https://stackoverflow.com/questions/8421170/read-floats-from-a-txt-file) 
		- Verwendet um istringstream zu verstehen für das Einlesen der Datei, die den Geist steuert (Methode: CarGhost::readRecordFile(...))
