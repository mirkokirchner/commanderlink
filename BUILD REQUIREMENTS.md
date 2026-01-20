BUILD_REQUIREMENTS.md
Zweck

Diese Datei definiert die Build- und Laufzeit-Voraussetzungen für CommanderLink (CL) auf den Zielplattformen:

Linux (openSUSE Tumbleweed / Arch)

GhostBSD (FreeBSD-Basis)

OmniOS (illumos)

Grundsatz:
CL wird als Binary ausgeliefert. Externe Libraries werden nur verlangt, wenn sie Host-Requirement sind (z. B. OpenSSL). Kompressionsbibliotheken wie LZ4 werden mitgeliefert/vendored, sofern wir sie verwenden.

1) Gemeinsame Anforderungen (alle Plattformen)
1.1 Toolchain

C-Compiler: GCC oder Clang (C11)

Make: GNU make empfohlen (auf illumos zwingend als gmake)

Standard-Header und libc

1.2 Curses UI (Monitor)

ncurses/curses Development Header + Library

Wegen OS-Unterschieden (Headerpfad / Libraryname) wird ein Wrapper genutzt (z. B. monitor_curses.h), der auf die richtige Include-Variante mappt. ncurses/curses Header-Varianten sind historisch unterschiedlich. 
FreeBSD Handbuchseiten
+1

1.3 Security (Join/Trust paranoid)

OpenSSL ist Host-Requirement für OOB-Auth/Join/Trust.

CL verweigert Join (Default-deny), wenn OpenSSL fehlt oder zu alt ist.

OpenSSL wird nicht vendored (Security-Patch-Pflege bleibt Host-Aufgabe).

2) Linux (openSUSE Tumbleweed / Arch)
2.1 Pflicht (Build)

gcc oder clang

make

pkg-config (optional, aber praktisch)

ncurses dev headers + lib (für Monitor)

libpthread/librt sind Teil der glibc Umgebung (typisch)

Beispiel: openSUSE Tumbleweed
sudo zypper install -t pattern devel_basis
sudo zypper install ncurses-devel

Beispiel: Arch
sudo pacman -S --needed base-devel ncurses

2.2 Optional (Performance Backends)
AF_XDP (BYPASS)

Kernel muss AF_XDP unterstützen; CL nutzt den Kernel-Mechanismus UMEM + XSK rings (SPSC rings) wie in Kernel-Doku beschrieben. 
Kernel-Dokumentation
+2
kernel.org
+2

Optional: libbpf/bpftool nur, wenn ihr XDP-Programme laden wollt (v0.1 kann BYPASS capability-gated sein).

Wichtig: UMEM „chunk size“ beeinflusst Kapazität/Max-Packet-Size; Kernel-Doku beschreibt Chunk-Beziehung. 
Kernel-Dokumentation
+2
kernel.org
+2

3) GhostBSD / FreeBSD

GhostBSD ist FreeBSD-basiert, daher gelten FreeBSD-Manpages und Base-System-Konventionen.

3.1 Pflicht (Build)

Compiler: cc (Clang in Base) oder GCC

make / gmake (je nach Ports)

ncurses/curses: FreeBSD bringt ncursesw in der Base; Header liegt typischerweise unter /usr/include/ncurses.h. 
The FreeBSD Forums
+1

Optionaler Build-Komfort
pkg install gmake

3.2 Optional (Performance Backends)
netmap (BYPASS)

netmap ist ein Framework für sehr schnelles Packet I/O; FreeBSD manpage ist Primärreferenz. 
FreeBSD Handbuchseiten
+1

Wenn netmap genutzt wird, müssen Kernel/Driver dies unterstützen (capability-gated).

4) OmniOS / illumos
4.1 Pflicht (Build)

GCC (OmniOS liefert verschiedene GCC-Packages; Beispiel: gcc14)

GNU make: Paketname ist developer/build/gnu-make (liefert gmake). 
OmniOS
+2
GitHub
+2

System-Header: system/header (für Build-Header; OmniOS nutzt pkg)

Beispiel (OmniOS)
pfexec pkg refresh
pfexec pkg install developer/build/gnu-make
pfexec pkg install library/ncurses
pfexec pkg install system/header


Hinweis: OmniOS Katalog führt developer/build/gnu-make explizit. 
OmniOS
+2
GitHub
+2

4.2 curses/ncurses Besonderheiten

OmniOS kann curses.h statt ncurses.h bereitstellen bzw. symlink-Strukturen nutzen. Deshalb wird weiterhin ein Wrapper-Header verwendet, der die richtige Include-Variante wählt und die korrekte Library (-lcurses oder -lncurses) linkt. 
FreeBSD Handbuchseiten
+1

4.3 Optional (Performance Backends)

OmniOS ist bei BYPASS-Mechanismen konservativer. Für v0.1 ist die Baseline:

UDP (Tier-0)

TCP fallback

netmap ist primär BSD-Ökosystem; OmniOS benötigt eigene Treiberpfade → wird capability-gated und später konkretisiert, wenn Treiber/Kernel-Support belegt ist.

5) Laufzeit-Anforderungen (alle Plattformen)
5.1 Privilegien

CL Daemon läuft als Systemdienst, i. d. R. mit Root/CAP-Äquivalent, weil:

Overlay-Interface (tun) anlegen

Routes setzen (/32 host routes)

ggf. Bypass-Queues initialisieren (capability-gated)

5.2 Ports / Firewall

OOB: UDP/4343

Baseline Data Plane: UDP (intern) + optional TCP fallback

In paranoid join: ohne gültige Auth-OOB wird kein Datenpfad aktiviert.

5.3 Optional: ZFS Visibility

Auf OmniOS und ggf. FreeBSD (OpenZFS) werden ZFS-Daten über OS-Interfaces/Tools gesammelt (z. B. arcstat/zpool). CL benötigt keine Kernel-Hacks; es zeigt den Zustand an.

6) CI / Verifikation (Empfehlung)

Linux + FreeBSD + OmniOS Builds als Pflichtmatrix (Monitor + Dummy-SHM).

ABI-Assertions müssen in jedem Build grün sein (sizeof/offset).

Sobald stabil: -Werror für Monitor-Build aktivieren (Phase-Gate).
