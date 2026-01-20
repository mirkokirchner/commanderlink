# CommanderLink – SECURITY & JOIN POLICY

## Ziel
Paranoide, deterministische Join-Regeln ohne Heuristik.

---

## 1. Grundsatz

> **Kein Peer ist vertrauenswürdig, bevor er es bewiesen hat.**

---

## 2. Mesh-Identität

- `mesh_tag64`: Domain-Identität
- `node_tag64`: Hardware-Identität

Beide:
- deterministisch
- persistent
- nicht erratbar

---

## 3. Beacon (untrusted)

- Sichtbarkeit
- Caps-Anzeige
- Kein Vertrauen

---

## 4. Hello / HelloAck

- Authentifiziert
- Geometrie-Aushandlung
- Overlay-IP
- Gates

---

## 5. Grant

- Credits
- FEC/CRC-Parameter
- Accept-Mode
- TTL

Ohne gültigen Grant:
- kein Senden

---

## 6. Replay-Schutz

- Epoch pro Peer
- Strict Monotonicity

---

## 7. Fail-Safe

- Bei Fehler → PASSIVE
- Overlay-Routen entfernen
- OS übernimmt

---

## 8. Merksatz

> **Security ist ein Zustand, kein Feature.**

