MessageIdTypedef    = DWORD

SeverityNames=(
    Success         = 0x00:     STATUS_SEVERITY_SUCCESS
    Informational   = 0x01:     STATUS_SEVERITY_INFORMATIONAL
    Warning         = 0x02:     STATUS_SEVERITY_WARNING
    Error           = 0x03:     STATUS_SEVERITY_ERROR
    )

FacilityNames=(
    App             = 0x0FFF:   FACILITY_APP
    Question        = 0x0DFF:   FACILITY_QUESTION
    Request         = 0x0EFF:   FACILITY_REQUEST
    )

LanguageNames=(
    en = 0x09:en
    de = 0x07:de
    it = 0x10:it
    )

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_ACCESS_DENIED
Language        = en
The path %1 cannot be accessed.
.
Language        = de
Auf das Pfad %1 kann nicht zugegriffen werden.
.
Language        = it
Non è possibile accedere al percorso %1.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_BAD_ATTRIBUTE
Language        = en
The attribute specification "%1!c!" is not valid.
.
Language        = de
Die Attributangabe "%1!c!" ist nicht gültig.
.
Language        = it
La specifica di attributo "%1!c!" non è valida.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_BAD_OPTION
Language        = en
One of the parameters begins with "/", but is not a valid option.
.
Language        = de
Einer der Parameter beginnt mit "/", ist aber keine gültige Option.
.
Language        = it
Uno dei parametri inizia per "/" ma non è un'opzione valida.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_BAD_OPTION_OR_ATTRIBUTE
Language        = en
One of the parameters begins with "-", but is not a valid option or attribute specification.
.
Language        = de
Einer der Parameter beginnt mit "-", ist aber weder eine gültige Option noch eine Attributangabe.
.
Language        = it
Uno dei parametri inizia per "-" ma non è né un'opzione valida, né una specifica di attributo.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_BAD_PATH
Language        = en
The format of the path %1 is incorrect.
.
Language        = de
Das Format des Pfades %1 ist falsch.
.
Language        = it
Il formato del percorso %1 è errato.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_DOUBLE_ATTRIBUTE
Language        = en
The attribute specification "%1!c!" was repeated.
.
Language        = de
Die Attributangabe "%1!c!" wurde wiederholt.
.
Language        = it
La specifica di attributo "%1!c!" è stata ripetuta."
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_ERROR
Language        = en
ERROR
.
Language        = de
FEHLER
.
Language        = it
ERRORE
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILE_CHANGED
Language        = en
The file has been modified
.
Language        = de
Die Datei wurde geändert
.
Language        = it
Il file è stato modificato
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILE_NOT_CHANGED
Language        = en
No changes needed
.
Language        = de
Keine Änderungen notwendig
.
Language        = it
Nessuna modifica necessaria
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_CHANGED_0
Language        = en
no changes needed
.
Language        = de
keine Änderungen notwendig
.
Language        = it
nessuna modifica necessaria
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_CHANGED_1
Language        = en
1 modified
.
Language        = de
1 geändert
.
Language        = it
1 modificato
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_CHANGED_MANY
Language        = en
%1!i! modified
.
Language        = de
%1!i! geändert
.
Language        = it
%1!i! modificati
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_PROCESSED_0
Language        = en
No files processed
.
Language        = de
Keine Dateien bearbeitet
.
Language        = it
Nessun file elaborato
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_PROCESSED_1
Language        = en
1 file processed
.
Language        = de
1 Datei bearbeitet
.
Language        = it
1 file elaborato
.

MessageId       = +1
Severity        = Success
Facility        = App
SymbolicName    = MSG_FILES_PROCESSED_MANY
Language        = en
%1!i! files processed
.
Language        = de
%1!i! Dateien bearbeitet
.
Language        = it
%1!i! file elaborati
.

MessageId       = +1
Severity        = Informational
Facility        = App
SymbolicName    = MSG_HELP
Language        = en

Shows or modifies the attributes of MP3 files.

Usage:
  MP3EPOC [/L|/S] [/W] [/F] [+P|-P] [+C|-C] [+O|-O] [/E0|/E1|/E2|/Ex] files

  /L        Show attributes in extended format.
  /S        Show attributes in compact format.
  /W        Read whole files, not only the key frames.
  /F        First frame is key frame (older Winamp versions).
  +         Set an attribute or show files with an attribute set.
  -         Clear an attribute or show files with an attribute not set.
  P         Private attribute.
  C         Copyright attribute.
  O         Original attribute.
  /E0       Remove emphasis or show files with no emphasis.
  /E1       Set or show files with emphasis at 50/15 microsec.
  /E2       Set or show files with emphasis at CCITT j. 17.
  /Ex       Show files with invalid emphasis.
  files     One or more files to process (wildcards are allowed).

Attribute specs are introduced by the sign + or - and may specify different attributes at the same time like in +PO or -OCP; the /E0, /E1, /E2 and /Ex options are also considered attribute specs, but they cannot be combined.

If the command line contains any attribute specs without any of the options /L, /S, /W or /F, MP3epoc writes the new attribute settings in every MP3 file specified.
If the command line contains any attribute specs along with any of the options /L, /S, /W or /F, MP3epoc shows the MP3 files matching the specified attributes without modifying them.
If the command line doesn't contain any attribute specs, MP3epoc shows the attributes of the MP3 files without modifying them: in this case, attributes are always displayed in extended format, unless the /S option is explicitly specified.
Some examples:

  > MP3EPOC *.MP3
  Shows in extended format the attributes of all MP3 files in the current directory.

  > MP3EPOC -C +O "Madonna - Material Girl.mp3"
  Clears the copyright attribute and sets the original attribute in all frames of the file "Madonna - Material Girl.mp3".

  > MP3EPOC /S /W /E1 "My Music\*Mozart*.MP3"
  Shows in compact format the attributes of all MP3 files in the folder "My Music" containing the word "Mozart" in the name and having the emphasis set at 50/15 microsec in all frames.
  
  > MP3EPOC -PCO /E0 "\\My-Host\Shared MP3s\*"
  Clears the private, copyright and original attributes and removes the emphasis from all frames of all files in the network folder "Shared MP3s" on "My-Host".

Attributes representation in extended format follows the general scheme:

  ±P* ±C* ±O* E?*

The letters P, C, O and E stand for the private, copyright, original and emphasis attributes respectively. The sign + before a letter indicates that the corresponding attribute is set. The sign - before a letter indicates that the attribute is not set.
The emphasis setting is indicated by the character after the E: 0 = no emphasis, 1 = 50/15 microsec, 2 = CCITT j. 17, x = invalid emphasis.
If using the /W option, an asterisk after an attribute means that the settings of the attribute are the same in all frames of the file.

The compact format shows the same information as the extended format in less space, it is just somewhat more complicated to read. The compact format uses one letter for each attribute in the sequence: private, copyright, original, emphasis. The different letters have the following meaning:

  j = emphasis at CCITT j. 17
  m = emphasis at 50/15 microsec
  n = attribute not set or no emphasis
  x = invalid emphasis
  y = attribute set

A capital letter means that the settings of the attribute are the same in all frames of the file.

Altough attribute settings are generally identical for all frames of an MP3 file, this is not always true.
The settings Winamp shows in the file info dialog for MP3 files are those found in the second frame of the file: this is the default key frame, the one from which MP3epoc normally extracts its informations to show the main settings of attributes or to exclude specific files from the search when the /W option is omitted. Some older Winamp versions and other applications consider rather the first frame as the file's key frame. The same behavior can be achieved with MP3epoc by specifying the /F option.
.
Language        = de

Zeigt die Attribute von MP3‐Dateien oder ändert sie.

Gebrauch:
  MP3EPOC [/L|/S] [/W] [/F] [+P|-P] [+C|-C] [+O|-O] [/E0|/E1|/E2|/Ex] Dateien

  /L        Zeigt die Attribute im erweiterten Format.
  /S        Zeigt die Attribute im kompakten Format.
  /W        Liest gesamte Dateien, nicht nur die Schlüssel‐Frames.
  /F        Erster Frame ist Schlüssel‐Frame (ältere Winamp‐Versionen).
  +         Setzt ein Attribut oder zeigt die Dateien wo es gesetzt ist.
  -         Löscht ein Attribut oder zeigt die Dateien wo es nicht gesetzt ist.
  P         Attribut "Privat".
  C         Attribut "Copyright".
  O         Attribut "Original".
  /E0       Entfernt die Emphase oder zeigt die Dateien ohne Emphase.
  /E1       Setzt oder zeigt die Dateien mit Emphase auf 50/15 microsec.
  /E2       Setzt oder zeigt die Dateien mit Emphase auf CCITT j. 17.
  /Ex       Zeigt die Dateien mit ungültiger Emphase.
  Dateien   Die zu verarbeitende(n) Datei(en) (Platzhalter sind erlaubt).

Attributangaben werden vom Zeichen + oder - eingeleitet und können verschiedene Attribute gleichzeitig angeben wie in +PO oder -OCP; die Optionen /E0, /E1, /E2 und /Ex gelten auch als Attributangaben, sie können jedoch nicht kombiniert werden.

Enthält die Befehlszeile Attributangaben ohne die Optionen /L, /S, /W oder /F, so schreibt MP3epoc die neuen Attributeinstellungen in jede angegebene MP3‐Datei.
Enthält die Befehlszeile Attributangaben zusammen mit den Optionen /L, /S, /W oder /F, so zeigt MP3epoc die den angegebenen Attributen entsprechenden MP3‐Dateien ohne sie zu ändern.
Enthält die Befehlszeile keine Attributangaben, so zeigt MP3epoc die Attribute der MP3‐Dateien ohne sie zu ändern: in diesem Fall werden Attribute stets im erweiterten Format angezeigt, wenn die Option /S nicht explizit angegeben ist.
Einige Beispiele:

  > MP3EPOC *.MP3
  Zeigt im erweiterten Format die Attribute aller MP3‐Dateien im aktuellen Verzeichnis.

  > MP3EPOC -C +O "Madonna - Material Girl.mp3"
  Löscht das Attribut "Copyright" und setzt das Attribut "Original" in allen Frames der Datei "Madonna - Material Girl.mp3".

  > MP3EPOC /S /W /E1 "My Music\*Mozart*.MP3"
  Zeigt im kompaktem Format die Attribute aller MP3‐Dateien im Verzeichnis "My Music" die das Wort "Mozart" im Namen enthalten und die Emphase auf 50/15 microsec in allen Frames gestzt haben.
  
  > MP3EPOC -PCO /E0 "\\My-Host\Shared MP3s\*"
  Löscht die Attribute "Privat", "Copyright" und "Original" und entfernt die Emphase von allen Frames aller Dateien im Netzwerkverzeichnis "Shared MP3s" auf "My-Host".

Die Darstellung der Attribute im erweiterten Format folgt das generelle Schema:

  ±P* ±C* ±O* E?*

Die Buchstaben P, C, O und E stehen jeweils für die Attribute "Privat", "Copyright", "Original" und "Emphase". Das Zeichen + vor einem Buchstabe zeigt, dass das entsprechende Attribut gesetzt ist; das Zeichen - zeigt, dass das Attribut nicht gesetzt ist.
Die Einstellung der Emphase wird von dem Zeichen nach dem E deutlich gemacht: 0 = keine Emphase, 1 = 50/15 microsec, 2 = CCITT j. 17, x = ungültige Emphase.
Wird die Option /W verwendet, so bedeutet ein Asteriskus nach einem Attribut, dass die Einstellungen des Attributes gleich in allen Frames der Datei sind.

Das kompakte Format zeigt die selben Informationen wie das erweiterte Format in weniger Platz, es ist nur etwas komplizierter zum Lesen. Das kompakte Format benutzt einen Buchstaben für jedes Attribut in der Reihefolge: "Privat", "Copyright", "Original", "Emphase". Die verschiedenen Buchstaben haben folgende Bedeutung:

  j = Emphase auf CCITT j. 17
  m = Emphase auf 50/15 microsec
  n = Attribut nicht gesetzt oder keine Emphase
  x = ungültige Emphase
  y = Attribut gesetzt

Ein Großbuchstabe bedeutet, dass die Einstellungen des Attributes gleich in allen Frames der Datei sind.

Obwohl Attributeinstellungen üblicherweise gleich für allen Frames einer MP3‐ Datei sind, ist dies nicht immer der Fall.
Die von Winamp im Dialog Titelinformationen für MP3‐Dateien gezeigten Einstellungen sind die, die im zweiten Frame der Datei liegen: dies ist der Default‐Schlüssel‐Frame, derjenige, aus dem MP3epoc normalerweise seine Angaben bezieht um die Haupteinstellungen der Attribute zu zeigen oder um bestimmte Dateien aus der Suche auszuschließen wenn die Option /W weggelassen wird. Einige ältere Winamp‐Versionen sowie andere Anwendungen betrachten statt dessen den ersten Frame als Schlüssel‐Frame der Datei. Das selbe Verhalten kann mit MP3epoc durch Angabe der Option /F erreicht werden.
.
Language        = it

Mostra o modifica gli attributi dei file MP3.

Uso:
  MP3EPOC [/L|/S] [/W] [/F] [+P|-P] [+C|-C] [+O|-O] [/E0|/E1|/E2|/Ex] file

  /L        Mostra attributi in formato esteso.
  /S        Mostra attributi in formato compatto.
  /W        Legge file interi, non solo i frame chiave.
  /F        Il primo frame è il frame chiave (vecchie versioni di Winamp).
  +         Imposta un attributo o mostra i file dov'è impostato.
  -         Cancella un attributo o mostra i file dove non è impostato.
  P         Attributo privato.
  C         Attributo copyright.
  O         Attributo originale.
  /E0       Rimuove l'enfasi o mostra i file senza enfasi.
  /E1       Imposta o mostra i file con enfasi a 50/15 microsec.
  /E2       Imposta o mostra i file con enfasi a CCITT j. 17.
  /Ex       Mostra i file con enfasi non valida.
  file      Uno o più file da elaborare (sono ammessi caratteri jolly).

Le specifiche di attributi sono introdotte dal segno + o - e possono indicare diversi attributi contemporaneamente come in +PO o -OCP; anche le opzioni /E0, /E1, /E2 e /Ex sono considerate specifiche di attributi, esse tuttavia non possono essere combinate.

Se la linea di comando contiene specifiche di attributi senza le opzioni /L, /S, /W o /F, MP3epoc scrive le nuove impostazioni di attributi in ogni file MP3 indicato.
Se la linea di comando contiene specifiche di attributi insieme alle opzioni /L, /S, /W o /F, MP3epoc mostra i file MP3 rispondenti agli attributi indicati senza modificarli.
Se la linea di comando non contiene specifiche di attributi, MP3epoc mostra gli attributi dei file MP3 senza modificarli: in questo caso gli attributi vengono visualizzati sempre in formato esteso, a meno che l'opzione /S non sia indicata esplicitamente.
Ecco alcuni esempi:

  > MP3EPOC *.MP3
  Mostra in formato esteso gli attributi di tutti i file MP3 nella directory corrente.

  > MP3EPOC -C +O "Madonna - Material Girl.mp3"
  Cancella l'attributo copyright e imposta l'attributo originale in tutti i frame del file "Madonna - Material Girl.mp3".

  > MP3EPOC /S /W /E1 "My Music\*Mozart*.MP3"
  Mostra in formato compatto gli attributi di tutti i file MP3 nella cartella "My Music" che contengono nel nome la parola "Mozart" ed hanno l'enfasi impostata a 50/15 microsec in tutti i frame.
  
  > MP3EPOC -PCO /E0 "\\My-Host\Shared MP3s\*"
  Cancella gli attributi privato, copyright e originale e rimuove l'enfasi da tutti i frame di tutti i file nella cartella di rete "Shared MP3s" su "My-Host".

La rappresentazione degli attributi in formato esteso segue lo schema generale:

  ±P* ±C* ±O* E?*

Le lettere P, C, O ed E stanno ad indicare rispettivamente gli attributi privato, copyright, originale ed enfasi. Il segno + davanti a una lettera indica che l'attributo corrispondente è impostato; il segno - indica che l'attributo non è impostato.
L'impostazione dell'enfasi è indicata dal carattere dopo la E: 0 = nessuna enfasi, 1 = 50/15 microsec, 2 = CCITT j. 17, x = enfasi non valida.
Se si usa l'opzione /W, un asterisco dopo un attributo indica che le impostazioni dell'attributo sono le stesse in tutti i frame del file.

Il formato compatto mostra le stesse informazioni del formato esteso in meno spazio, è solo un po' più complicato da leggere. Il formato compatto utilizza una lettera per ciascun attributo nell'ordine: privato, copyright, originale, enfasi. Le diverse lettere hanno il seguente significato:

  j = enfasi a CCITT j. 17
  m = enfasi a 50/15 microsec
  n = attributo non impostato o nessuna enfasi
  x = enfasi non valida
  y = attributo impostato

Una lettera maiuscola indica che le impostazioni dell'attributo sono le stesse in tutti i frame del file.

Sebbene le impostazioni di attributi siano generalmente uguali per tutti i frame di un file MP3, questo non è sempre il caso.
Le impostazioni mostrate da Winamp nel dialogo informazioni sul file per i file MP3 sono quelle che si trovano nel secondo frame del file: questo è il frame chiave di default, quello da cui MP3epoc estrae normalmente le proprie informazioni per mostrare le impostazioni principali degli attributi o per escludere determinati file dalla ricerca quando l'opzione /W viene omessa. Alcune versioni più vecchie di Winamp e altre applicazioni considerano invece il primo frame come frame chiave del file. Lo stesso comportamento può essere ottenuto con MP3epoc indicando l'opzione /F.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_DATA_UNKNOWN_EXCEPTION
Language        = en
The file %2 contains unknown data at offset %1.
.
Language        = de
Die Datei %2 enthält unbekannte Daten an der Stelle %1.
.
Language        = it
Il file %2 contiene dati sconosciuti alla posizione %1.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FILE_INVALID_EXCEPTION
Language        = en
%1 is not an MP3 file.
.
Language        = de
%1 ist keine MP3‐Datei.
.
Language        = it
%1 non è un file MP3.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FIRST_FRAME_NOT_FOUND_EXCEPTION
Language        = en
Either the size of the file %1 or the information in the ID3v2 tag is wrong.
.
Language        = de
Entweder die Größe der Datei %1 oder die Informationen im ID3v2‐Tag sind falsch.
.
Language        = it
Le dimensioni del file %1 sono errate, oppure lo sono le informazioni nel tag ID3v2.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FORMAT_EXCEPTION
Language        = en
An error occurred while processing file %2 at offset %1.
.
Language        = de
Beim Bearbeiten der Datei file %2 an der Stelle %1 ist ein Fehler aufgetreten.
.
Language        = it
Si è verificato un errore durante l'eleborazione del file %2 alla posizione %1.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FRAME_CRC_TEST_EXCEPTION
Language        = en
Frame %1!I64i! in file %3 at offset %2 is corrupt and did not pass the CRC test.
.
Language        = de
Der Frame %1!I64i! in der Datei %3 an der Stelle %2 ist beschädigt und hat die CRC‐Prüfung nicht bestanden.
.
Language        = it
Il frame %1!I64i! del file %3 alla posizione %2 è danneggiato e non ha superato la prova del CRC.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FRAME_CRC_UNKNOWN_EXCEPTION
Language        = en
The CRC of frame %1!I64i! in file %3 at offset %2 cannot be recalculated.
.
Language        = de
Der CRC‐Wert des Frames %1!I64i! in der Datei %3 an der Stelle %2 kann nicht neu gerechnet werden.
.
Language        = it
Non è possibile ricalcolare il codice CRC del frame %1!I64i! del file %3 alla posizione %2.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FRAME_EXCEPTION
Language        = en
An error occurred while processing frame %1!I64i! in file %3 at offset %2.
.
Language        = de
Beim Bearbeiten des Frames %1!I64i! in der Datei %3 an der Stelle %2 ist ein Fehler aufgetreten.
.
Language        = it
Si è verificato un errore durante l'eleborazione del frame %1!I64i! del file %3 alla posizione %2.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_FRAME_SIZE_UNKNOWN_EXCEPTION
Language        = en
The size of frame %1!I64i! in file %3 at offset %2 cannot be determined.
.
Language        = de
Die Größe des Frames %1!I64i! in der Datei %3 an der Stelle %2 kann nicht ermittelt werden.
.
Language        = it
Non è possibile stabilire le dimensioni del frame %1!I64i! del file %3 alla posizione %2.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_GENERIC_EXCEPTION
Language        = en
An error occurred while processing file %1.
.
Language        = de
Beim Bearbeiten der Datei %1 ist ein Fehler aufgetreten.
.
Language        = it
Si è verificato un errore durante l'eleborazione del file %1.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_MP3_KEY_FRAME_NOT_FOUND_EXCEPTION
Language        = en
The file %1 has no key frame.
.
Language        = de
Die Datei %1 hat keinen Schlüssel‐Frame.
.
Language        = it
Il file %1 non ha il frame chiave.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_NO_FILE
Language        = en
No file was specified.
.
Language        = de
Es wurde keine Datei angegeben.
.
Language        = it
Non è stato indicato alcun file.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_OPT_EX_IN_WRITING_OP
Language        = en
The /Ex option can only be used together with any of the options /L, /S, /W or /F.
.
Language        = de
Die Option /Ex kann nur zusammen mit den Optionen /L, /S, /W oder /F benutzt werden.
.
Language        = it
L'opzione /Ex può essere usata solo insieme alle opzioni /L, /S, /W o /F.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_PATH_IS_DIR
Language        = en
The path %1 denotes a directory.
.
Language        = de
Der Pfad %1 bezeichnet ein Verzeichnis.
.
Language        = it
Il percorso %1 denota una directory.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_PATH_NOT_FOUND
Language        = en
The path %1 was not found.
.
Language        = de
Der Pfad %1 wurde nicht gefunden.
.
Language        = it
Il percorso %1 non è stato trovato.
.

MessageId       = +1
Severity        = Error
Facility        = App
SymbolicName    = MSG_SYNTAX_ERROR
Language        = en
The syntax of the command is incorrect.
.
Language        = de
Die Syntax des Befehls ist falsch.
.
Language        = it
La sintassi del comando è errata.
.
