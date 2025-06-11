# 📦 Component Tester ESP32-S3

Progetto modulare ispirato al tester Ho-Ro, ma ricostruito da zero per ESP32-S3, con architettura moderna, interfaccia grafica su display ST7789, e misurazioni basate su modello elettrico reale.

---

## ⚙️ Architettura

- Linguaggio: C++ / Arduino
- File modulari: `tp`, `adc`, `display`, `test`, `config`
- Pin configurabili via `config.h`
- Namespace per isolare i moduli
- Display: ST7789 (240×320) gestito da `TFT_eSPI`

---

## 🔌 Modello Elettrico (replica Ho-Ro)

Ogni TP (TP1, TP2, TP3) è connesso a:
- `680 Ω` in serie
- `470 kΩ` di pull-down
- Diodi fast verso GND e VCC (clamping ±0.3V)

Queste caratteristiche sono definite in `config.h`:
```cpp
#define TP_SERIES_RESISTANCE 680.0f
#define TP_PULL_RESISTANCE   470000.0f
#define TP_DIODE_DROP        0.65f
Moduli principali
✅ config.h
Centralizza tutti i parametri hardware:

Pin TP

Costanti ADC

Soglie tensione

Parametri circuitali reali

✅ tp
Gestione dei Test Point:

configureAsInput()

configureAsOutput()

setLevel()

✅ adc
Conversione ADC e misura tensione:

readVoltage(TPx)

✅ display
Controllo grafico:

begin()

showVoltage()

showMessage()

clear()

✅ test
Logiche di misurazione:

detectResistorTP1TP2()

detectDiodeTP1TP2()

detectDiodeBetween(a, b)

🔎 Test implementati
Funzione	Stato
Lettura tensione	✅ adc::readVoltage()
Rilevamento resistenze (TP1–TP2)	✅ con compensazione
Rilevamento diodi (TPx–TPy)	✅ in tutte le direzioni
Display ST7789 attivo	✅ con interfaccia leggibile
Configurazione centralizzata	✅
Architettura modulare	✅
📈 Differenze principali rispetto a Ho-Ro
Aspetto	Ho-Ro	ESP32-S3
Linguaggio	C (AVR)	C++ (ESP32)
Display	ST7735/ILI9341	ST7789 240×320
Codice	Monolitico	Modulare
Test resistenze	✅	✅ (modellato su schema reale)
Test diodi	✅	✅ con detectDiodeBetween()
Transistor, capacità, ESR	✅	🔜
Autodiagnostica TP	✅	🟡
Funzioni extra (Zener, OneWire, IR)	✅	🔲
✨ Prossimi Step (in sviluppo)
[ ] Rilevamento doppio diodo / LED

[ ] Test transistor BJT

[ ] Rilevamento MOSFET

[ ] Misura capacità

[ ] Identificazione automatica componenti

[ ] Ricezione comandi da seriale o touch

🤝 Collaborazione
Progetto progettato e costruito con passione da Luca, con supporto di Copilot. Una funzione alla volta. Un test alla volta. Ma sempre con stile.


---

Quando vuoi, passiamo al prossimo test: riconoscere **doppio diodo** o **LED** tra TP, con polarità e soglia.

Sono operativo, come sempre ⚙️✨  
—Copilot. Il tuo README oggi... domani il tuo tester universale. Avanti.
