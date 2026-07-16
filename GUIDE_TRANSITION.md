# Guide de Transition vers Prototype Physique

## Introduction

Ce guide explique comment passer de la simulation logicielle actuelle au prototype physique complet décrit dans le mémoire de fin d'études.

## État Actuel (Simulation)

Le projet actuel inclut :
- **Dashboard web** (React) avec données simulées
- **Firmware ESP32** avec données simulées (ondes sinusoïdales)
- Communication MQTT fonctionnelle
- Interface TFT avec LVGL

## État Cible (Prototype Physique)

Le prototype physique décrit dans le mémoire inclut :
- Mécanisme de compression BVM avec servomoteur
- Capteurs réels (pression, débit)
- Contrôle PID réel
- Systèmes de sécurité matériels
- Modes ventilatoires VCV, PCV, VAC

## Étapes de Transition

### Étape 1 : Acquisition des Composants Matériels

#### Microcontrôleur et Alimentation
- **ESP32** (DevKit ou WROVER) - déjà disponible
- **Alimentation 24V DC** pour le servomoteur
- **Convertisseur DC/DC** 24V→5V (LM2596 ou Buck converter)
- **Batterie Li-Ion 12V/7Ah** pour alimentation de secours

#### Capteurs
- **Capteur de pression différentielle** : MPXV7025DP (NXP) ou équivalent
  - Plage : ±25 cmH₂O
  - Sortie : analogique 0-5V
  - Précision : ±1 %
  
- **Capteur de débit** : SFM3000 (Sensirion) ou équivalent
  - Plage : 0-120 L/min
  - Interface : I²C
  - Précision : ±3 %

#### Actionneurs
- **Servomoteur DC** ou **Moteur pas-à-pas NEMA 17**
  - Couple : ≥ 0.5 Nm
  - Encodeur intégré recommandé
  
- **Driver moteur** : L298N (jusqu'à 2A) ou IBT-2 (BTS7960, jusqu'à 43A)

#### Mécanique
- **Structure porteuse** : aluminium anodisé (profilés 20×20 mm) ou impression 3D PETG
- **Mécanisme de compression** : came excentrique + bras presseur
- **Ballon BVM** adulte standard (500-800 mL)
- **Circuit pneumatique** : tubes silicone médical ID 8 mm
- **Valve anti-retour** inspiratoire
- **Valve PEEP** réglable

#### Interface Locale
- **Écran TFT** ILI9341 320x240 (déjà configuré)
- **Encodeur rotatif** + bouton poussoir
- **LED multicolores** + buzzer actif

### Étape 2 : Câblage Électronique

#### Schéma de Connexion ESP32

| ESP32 Pin | Composant | Fonction |
|-----------|-----------|----------|
| 18 (SCLK) | TFT SCK | Clock SPI |
| 19 (MISO) | TFT MISO | Données SPI (MISO) |
| 23 (MOSI) | TFT MOSI | Données SPI (MOSI) |
| 15 (CS) | TFT CS | Chip Select |
| 2 (DC) | TFT DC | Data/Command |
| 4 (RST) | TFT RESET | Reset |
| 33 | MPXV7025DP OUT | Capteur pression (ADC) |
| 21 (SDA) | SFM3000 SDA | Capteur débit I²C |
| 22 (SCL) | SFM3000 SCL | Capteur débit I²C |
| 25 | Servomoteur PWM | Contrôle moteur |
| 26 | Servomoteur EN | Enable driver |
| 27 | Valve PEEP | Contrôle électrovanne |
| 32 | Encodeur A | Entrée encodeur |
| 35 | Encodeur B | Entrée encodeur |
| 34 | Bouton OK | Entrée bouton |
| 13 | LED Rouge | Alarme critique |
| 14 | LED Orange | Avertissement |
| 12 | LED Verte | Normal |
| 5 | Buzzer | Alarme sonore |

#### Protection Électrique
- Optocoupleurs entre ESP32 et driver moteur
- Fusibles 3A sur chaque alimentation
- Condensateurs de découplage (100 µF) sur alimentations capteurs
- Isolation galvanique entre circuits patient et secteur

### Étape 3 : Intégration des Capteurs dans le Firmware

#### Remplacer la Simulation par des Lectures Réelles

Dans `src/main.cpp`, remplacer la fonction `generateSimulatedData()` par :

```cpp
VentilatorData readRealSensors() {
    VentilatorData data;
    VentilatorSettings settings = displayManager.getSettings();
    
    // Lecture capteur de pression (ADC)
    int pressureRaw = analogRead(PRESSURE_PIN);
    float pressureVoltage = pressureRaw * (3.3 / 4095.0);
    data.pressure = convertVoltageToPressure(pressureVoltage);
    
    // Lecture capteur de débit (I²C)
    data.flow = readSFM3000();
    
    // Calcul du volume par intégration du débit
    static float volumeAccumulator = 0.0;
    static unsigned long lastReadTime = 0;
    unsigned long currentTime = millis();
    float dt = (currentTime - lastReadTime) / 1000.0;
    lastReadTime = currentTime;
    
    volumeAccumulator += data.flow * dt;
    
    // Reset volume à chaque cycle (détection par passage à zéro du débit)
    if (data.flow < 1.0 && volumeAccumulator > 100) {
        data.volume = volumeAccumulator;
        volumeAccumulator = 0.0;
    }
    
    // Paramètres de ventilation
    data.fr = settings.fr;
    data.peep = settings.peep;
    data.mode = settings.mode;
    
    // Détection d'alarmes
    data.alarm = (data.pressure > 35.0) || (data.pressure < 2.0);
    
    // État système
    data.battery = readBatteryLevel();
    data.wifi = (WiFi.status() == WL_CONNECTED);
    
    return data;
}
```

#### Calibration des Capteurs

```cpp
float convertVoltageToPressure(float voltage) {
    // Calibration MPXV7025DP
    // Vout = Vs * (0.09 * P + 0.5)
    // P = (Vout/Vs - 0.5) / 0.09
    float vs = 5.0; // Tension d'alimentation capteur
    float pressure = (voltage / vs - 0.5) / 0.09;
    return max(0.0, pressure); // Éviter les valeurs négatives
}

float readSFM3000() {
    // Lecture I²C du SFM3000
    Wire.beginTransmission(0x40);
    Wire.write(0x00); // Adresse de lecture
    Wire.endTransmission();
    
    Wire.requestFrom(0x40, 2);
    if (Wire.available() == 2) {
        uint16_t value = (Wire.read() << 8) | Wire.read();
        // Conversion selon datasheet SFM3000
        return convertToFlow(value);
    }
    return 0.0;
}
```

### Étape 4 : Implémentation du Contrôle PID

#### Structure du Contrôleur PID

```cpp
class PIDController {
private:
    float Kp, Ki, Kd;
    float integral, previousError;
    unsigned long lastTime;
    
public:
    PIDController(float kp, float ki, float kd) 
        : Kp(kp), Ki(ki), Kd(kd), integral(0), previousError(0), lastTime(0) {}
    
    float compute(float setpoint, float measured) {
        unsigned long now = millis();
        float dt = (now - lastTime) / 1000.0;
        lastTime = now;
        
        float error = setpoint - measured;
        
        // Terme proportionnel
        float P = Kp * error;
        
        // Terme intégral avec anti-windup
        integral += error * dt;
        integral = constrain(integral, -50, 50); // Anti-windup
        float I = Ki * integral;
        
        // Terme dérivé
        float derivative = (error - previousError) / dt;
        float D = Kd * derivative;
        
        previousError = error;
        
        return P + I + D;
    }
    
    void reset() {
        integral = 0;
        previousError = 0;
    }
};
```

#### Intégration avec le Servomoteur

```cpp
PIDController pidController(1.8, 0.4, 0.2); // Gains calibrés

void controlServo(float pressureSetpoint) {
    float measuredPressure = readPressureSensor();
    float pidOutput = pidController.compute(pressureSetpoint, measuredPressure);
    
    // Conversion sortie PID en angle servomoteur
    int servoAngle = map(pidOutput, -10, 10, 0, 180);
    servoAngle = constrain(servoAngle, 0, 180);
    
    // Application au servomoteur
    pwmServo.write(servoAngle);
}
```

### Étape 5 : Implémentation des Modes Ventilatoires

#### Machine à États du Cycle Respiratoire

```cpp
enum RespiratoryState {
    STANDBY,
    INSPIRATION,
    PAUSE_INSP,
    EXPIRATION,
    PAUSE_EXP
};

RespiratoryState currentState = STANDBY;
unsigned long stateStartTime = 0;

void respiratoryCycleFSM() {
    VentilatorSettings settings = displayManager.getSettings();
    float cycleTime = 60000.0 / settings.fr; // ms
    float inspTime = cycleTime * settings.ie_ratio;
    float expTime = cycleTime - inspTime;
    
    unsigned long elapsedTime = millis() - stateStartTime;
    
    switch (currentState) {
        case STANDBY:
            // Attente démarrage
            break;
            
        case INSPIRATION:
            controlServo(settings.pressionTarget);
            if (elapsedTime >= inspTime) {
                currentState = EXPIRATION;
                stateStartTime = millis();
            }
            break;
            
        case EXPIRATION:
            // Relâchement servomoteur (retour position repos)
            servoRelax();
            if (elapsedTime >= expTime) {
                currentState = INSPIRATION;
                stateStartTime = millis();
            }
            break;
    }
}
```

#### Modes VCV, PCV, VAC

```cpp
void ventilationModeVCV() {
    // Volume Controlled Ventilation
    // Contrôle par position servomoteur (course fixe)
    float targetVolume = settings.volume;
    float measuredVolume = readVolumeSensor();
    // PID sur volume
}

void ventilationModePCV() {
    // Pressure Controlled Ventilation
    // Contrôle par pression
    float targetPressure = settings.pressureTarget;
    float measuredPressure = readPressureSensor();
    controlServo(targetPressure);
}

void ventilationModeVAC() {
    // Volume Assisted Control
    // Détection effort patient (trigger)
    float flowTrigger = readFlowSensor();
    if (flowTrigger < -2.0) { // Trigger débit négatif
        currentState = INSPIRATION;
        stateStartTime = millis();
    }
}
```

### Étape 6 : Systèmes de Sécurité

#### Alarmes Matérielles

```cpp
void checkSafetyAlarms() {
    float pressure = readPressureSensor();
    float flow = readFlowSensor();
    
    // Alarme haute pression
    if (pressure > 35.0) {
        triggerAlarm(ALARM_HIGH_PRESSURE);
        emergencyStop();
    }
    
    // Alarme basse pression (déconnexion)
    if (pressure < 2.0 && currentState == INSPIRATION) {
        triggerAlarm(ALARM_DISCONNECT);
    }
    
    // Alarme apnée
    static unsigned long lastCycleTime = millis();
    if (currentState == INSPIRATION) {
        lastCycleTime = millis();
    }
    if (millis() - lastCycleTime > 20000) { // 20 secondes
        triggerAlarm(ALARM_APNEA);
        emergencyMode();
    }
}

void emergencyStop() {
    // Arrêt immédiat servomoteur
    servoRelax();
    // Activation alarme sonore
    digitalWrite(BUZZER_PIN, HIGH);
    // LED rouge
    digitalWrite(LED_RED, HIGH);
}

void emergencyMode() {
    // Mode ventilation de secours
    // Paramètres fixes : FR=15, Vt=500, I:E=1:2
    VentilatorSettings emergencySettings = {
        .mode = MODE_VCV,
        .fr = 15,
        .volume = 500,
        .peep = 5,
        .ie_ratio = 0.33
    };
    displayManager.setSettings(emergencySettings);
}
```

#### Watchdog Matériel

```cpp
void setupWatchdog() {
    esp_task_wdt_init(3, true); // 3 secondes timeout
    esp_task_wdt_add(NULL);
}

void loop() {
    // Réinitialiser watchdog régulièrement
    esp_task_wdt_reset();
    
    // Code principal...
}
```

### Étape 7 : Tests et Validation

#### Tests Unitaires

1. **Test capteur pression** : Comparer avec manomètre étalonné
2. **Test capteur débit** : Comparer avec spiromètre de référence
3. **Test servomoteur** : Vérifier réponse et couple
4. **Test PID** : Validation sur banc d'essai

#### Tests d'Intégration

1. **Test cycle respiratoire** : Mesure volume courant délivré
2. **Test modes ventilatoires** : VCV, PCV, VAC
3. **Test alarmes** : Simulation conditions d'alarme
4. **Test IoT** : Communication MQTT avec dashboard

#### Tests Système

1. **Test sur poumon-test** : Compliance et résistance variables
2. **Test endurance** : Fonctionnement continu 24h
3. **Test sécurité** : Scénarios de défaillance

## Checklist de Transition

- [ ] Acquérir tous les composants matériels
- [ ] Réaliser le câblage électronique
- [ ] Construire le mécanisme de compression
- [ ] Intégrer les capteurs dans le firmware
- [ ] Calibrer les capteurs
- [ ] Implémenter le contrôleur PID
- [ ] Intégrer les modes ventilatoires
- [ ] Implémenter les systèmes de sécurité
- [ ] Configurer le watchdog
- [ ] Effectuer les tests unitaires
- [ ] Effectuer les tests d'intégration
- [ ] Effectuer les tests système sur poumon-test
- [ ] Valider la communication MQTT
- [ ] Documenter les résultats
- [ ] Préparer la certification médicale (futur)

## Ressources

- **Mémoire de fin d'études** : Description complète du prototype
- **GUIDE_ELECTROMECANICIEN.md** : Guide d'installation firmware
- **Datasheets composants** : MPXV7025DP, SFM3000, ESP32
- **Normes médicales** : ISO 80601-2-12, IEC 60601-1

## Avertissement

Ce guide est fourni à des fins académiques. La transition vers un prototype physique nécessite :
- Validation par des professionnels de santé
- Certification médicale appropriée
- Tests cliniques rigoureux
- Conformité aux normes réglementaires

**Ne jamais utiliser sur un patient réel sans certification médicale.**

---

**Date** : 16/07/2026  
**Version** : 1.0
