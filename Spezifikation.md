# Jump and Run Game

## Spezifikationen

Das Ziel unseres Projektes ist ein Jump and Run Spiel mit verschiedenen Leveln zu gestalten.

Die Kernfunktionalitäten des Spieles sollen hierbei sein:

* Spiel: *(hauptsächlich Daniel)*
	* Level aus Dateien lesen (Text-Dateien, Art der Blöcke als Zahlen) *(Stefan)*
	* Laden der benötigten Blöcke aus Grafikdateien und richtige Darstellung auf dem Bildschirm
	* animieren des Bildes, bewegte Karte und Spieler, Gravitation, Beschleunigung, Kollision
		* Animation mit konstanter Framerate
	* Steuerung
		* Tastatur-Events
	* Zusatzitems in den Levels: Schatzkisten, Münzen etc.
* Highscores bestimmen (benötigte Zeit für ein Level, Anzahl gesammelter Items) *(Murat)*
* Menü: *(Stefan)*
	* Spiel starten, Level-Auswahl
	* Einstellungsmöglichkeiten
		* Auflösung
		* Fullscreen
	* Highscores anzeigen
* Persistenz: *(Murat)*
	* speichern von Einstellungen
	* speichern von Highscores
* Hintergrundmusik

## CodeStil

* Einrückung
    * Allman / BSD (https://de.wikipedia.org/wiki/Einr%C3%BCckungsstil#Allman_.2F_BSD_.2F_.E2.80.9EEast_Coast.E2.80.9C)
    * Tabs
* Klammern verwenden
* Variablen und Funktionsnamen:
	* grundsätzlich klein geschrieben
	* wenn diese aus mehreren Wörtern bestehen: erster Buchstabe der Wörter groß (z. B. drawMenu)
	* keine Worttrennung mit Unterstrichen
* Abfragen
	* zwischen den Variablen / Rechnungen und dem Vergleichsoperator ein Leerzeichen (z. B. (x*y) > (x+y))
* defines
	* alle Zeichen groß geschrieben
	* wenn aus mehreren Wörtern bestehend Trennung per Unterstrich (z. B. SCREEN_BPP)