Dies ist der Einstiegspunkt für die ADTF-Filterentwicklung. Die Ordnerstruktur baut sich wie folgt auf.

Folders:
* _build_user: wird bei Verwendung des Buildsktipts (build_user.sh) angelegt und enthält die temporären Buildartefakte.
* _build_base: wird bei Verwendung des Buildsktipts (build_base.sh) angelegt und enthält die temporären Buildartefakte.
* _build_demo: wird bei Verwendung des Buildsktipts (build_demo.sh) angelegt und enthält die temporären Buildartefakte.

* aadcUser: Enthält einen Template-Filter. In diesem Ordner werden die eigenen Filter der Studenten abgelegt.
* aadcBase: Enthält die Sourcen der vorkompilierten Basismodule zur Ansteuerung des AADC-Fahrzeugs. Änderungen sind lokal möglich, werden aber zum Wettbewerb auf einen einheitlichen Stand gebracht. Siehe hierzu auch die Wettbewerbsregeln.
* aadcDemo: Enthält die Sourcen der erweiterten Beispielmodule und können von den Teilnehmer übernommen oder angepasst werden.

Files:
* build_user.sh: baut die im Verzeichnis aadcUser befindlichen Sourcen mit Hilfe von CMake.
* build_base.sh: baut die im Verzeichnis aadcBase befindlichen Sourcen mit Hilfe von CMake.
* build_demo.sh: baut die im Verzeichnis aadcDemo befindlichen Sourcen mit Hilfe von CMake.