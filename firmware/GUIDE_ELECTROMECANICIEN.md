# Guide d'Installation et Configuration - Firmware ESP32
**Pour Électromécanicien**

## Table des Matières
1. [Vue d'ensemble](#vue-densemble)
2. [Matériel Requis](#matériel-requis)
3. [Installation de l'Environnement](#installation-de-lenvironnement)
4. [Configuration du Firmware](#configuration-du-firmware)
5. [Câblage de l'Écran TFT](#câblage-de-lécran-tft)
6. [Compilation et Téléversement](#compilation-et-téléversement)
7. [Tests et Vérification](#tests-et-vérification)
8. [Dépannage](#dépannage)
9. [Intégration avec le Dashboard](#intégration-avec-le-dashboard)

---

## Vue d'ensemble

Ce firmware contrôle l'ESP32 avec un écran TFT 320x240 pour afficher les données du respirateur en temps réel. Il communique avec le dashboard web via MQTT.

**Fonctionnalités :**
- Affichage des métriques respiratoires sur écran TFT
- Graphiques temps réel (pression, débit, volume)
- Connexion WiFi automatique
- Communication MQTT bidirectionnelle
- Simulation de données pour tests

---

## Matériel Requis

**Composants :**
- ESP32 (DevKit ou WROVER)
- Écran TFT ILI9341 320x240
- Câble USB pour ESP32
- Ordinateur Windows/Mac/Linux

**Optionnel :**
- Broker MQTT local (Mosquitto, EMQX, etc.)
- Routeur WiFi

---

## Installation de l'Environnement

### Option 1 : VS Code (Recommandé)

1. **Installer VS Code**
   - Télécharger : https://code.visualstudio.com/

2. **Installer l'extension PlatformIO**
   - Ouvrir VS Code
   - Aller dans Extensions (Ctrl+Shift+X)
   - Rechercher "PlatformIO IDE"
   - Installer

3. **Ouvrir le projet**
   - Fichier → Ouvrir le dossier
   - Sélectionner le dossier `firmware`

### Option 2 : PlatformIO Standalone

1. **Installer Python 3.8+**
   - https://www.python.org/downloads/

2. **Installer PlatformIO**
   ```bash
   pip install platformio
   ```

3. **Vérifier l'installation**
   ```bash
   pio --version
   ```

---

## Configuration du Firmware

### Étape 1 : Configuration WiFi

Ouvrir le fichier `src/main.cpp` et modifier les lignes 23-24 :

```cpp
const char* WIFI_SSID = "VOTRE_NOM_WIFI";
const char* WIFI_PASSWORD = "VOTRE_MOT_DE_PASSE";
```

**Important :** Remplacez "VOTRE_NOM_WIFI" et "VOTRE_MOT_DE_PASSE" par vos identifiants WiFi réels.

### Étape 2 : Configuration MQTT

Modifier la ligne 25 dans `src/main.cpp` :

**Option A - Broker MQTT Local (Recommandé pour production)**
```cpp
const char* MQTT_HOST = "192.168.1.10";  // Adresse IP de votre broker
const uint16_t MQTT_PORT = 1883;
```

**Option B - Broker Public HiveMQ (Pour tests)**
```cpp
const char* MQTT_HOST = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
```

**Note :** Le dashboard Vercel utilise `wss://broker.hivemq.com:8884/mqtt` par défaut. L'ESP32 utilise le port 1883 (MQTT standard) et non WebSocket.

### Étape 3 : Configuration des Pins TFT

Les pins sont déjà configurés dans `platformio.ini` (lignes 26-32) :

```ini
-DTFT_MISO=19
-DTFT_MOSI=23
-DTFT_SCLK=18
-DTFT_CS=15
-DTFT_DC=2
-DTFT_RST=4
```

**Si votre écran utilise des pins différents :**
1. Modifier les valeurs dans `platformio.ini`
2. Adapter le câblage en conséquence

---

## Câblage de l'Écran TFT

### Schéma de Connexion (ESP32 → TFT ILI9341)

| ESP32 Pin | TFT Pin | Fonction |
|-----------|---------|----------|
| 3.3V | VCC | Alimentation |
| GND | GND | Masse |
| 18 (SCLK) | SCK | Clock SPI |
| 19 (MISO) | MISO | Données SPI (MISO) |
| 23 (MOSI) | MOSI | Données SPI (MOSI) |
| 15 (CS) | CS | Chip Select |
| 2 (DC) | DC | Data/Command |
| 4 (RST) | RESET | Reset |
| - | LED | Backlight (connecter à 3.3V) |

**Notes importantes :**
- Vérifier que l'écran est compatible 3.3V
- Le backlight peut être connecté directement à 3.3V
- Utiliser des fils courts pour éviter les interférences

---

## Compilation et Téléversement

### Avec VS Code + PlatformIO IDE

1. **Connecter l'ESP32** via USB
2. **Sélectionner le port** :
   - Cliquer sur l'icône PlatformIO (alien en bas)
   - Devices → Sélectionner le port COM
3. **Compiler** :
   - Cliquer sur la flèche (Build)
   - Ou Ctrl+Alt+B
4. **Téléverser** :
   - Cliquer sur la flèche vers le bas (Upload)
   - Ou Ctrl+Alt+U

### Avec PlatformIO en ligne de commande

```bash
# Aller dans le dossier firmware
cd firmware

# Compiler
pio run

# Téléverser
pio run --target upload

# Voir les logs série
pio device monitor

# Compiler et téléverser en une commande
pio run --target upload --target monitor
```

### Vérification du Téléversement

**Signes de succès :**
- LED bleue de l'ESP32 clignote pendant le téléversement
- Message "Writing at 0x0000..." dans la console
- Message "Done in X.XX seconds"

---

## Tests et Vérification

### 1. Vérification de la Connexion Série

Ouvrir le moniteur série (115200 bauds) :

```bash
pio device monitor -b 115200
```

**Messages attendus au démarrage :**
```
Initializing Display Manager...
Connecting to WiFi: VOTRE_NOM_WIFI
WiFi connected
Connecting to MQTT broker: broker.hivemq.com
MQTT connected and subscribed to ventilator/config
```

### 2. Vérification de l'Écran TFT

L'écran doit afficher :
- Barre de statut (WiFi, batterie, alarme)
- Cartes de métriques (Mode, Pression, Volume, etc.)
- Graphiques temps réel
- Boutons de navigation

### 3. Vérification MQTT

**Ouvrir le dashboard web** et vérifier :
- Les données s'affichent en temps réel
- Les graphiques se mettent à jour
- Le statut de connexion est "Connecté"

### 4. Test de Configuration

**Depuis le dashboard :**
1. Aller dans "Paramètres"
2. Modifier un paramètre (ex: Fréquence respiratoire)
3. Cliquer sur "Enregistrer les modifications"
4. Vérifier le message de succès

**Sur l'ESP32 (moniteur série) :**
```
Received ventilator/config from dashboard
```

---

## Dépannage

### Problème : "WiFi not connected"

**Solutions :**
1. Vérifier les identifiants WiFi dans `main.cpp`
2. Vérifier que le WiFi est accessible
3. Vérifier la distance ESP32 - routeur
4. Redémarrer l'ESP32

### Problème : "MQTT connection failed"

**Solutions :**
1. Vérifier l'adresse IP du broker MQTT
2. Vérifier que le broker est accessible
3. Tester avec un client MQTT (ex: MQTT Explorer)
4. Vérifier le pare-feu

### Problème : Écran blanc

**Solutions :**
1. Vérifier le câblage TFT
2. Vérifier l'alimentation (3.3V)
3. Vérifier les pins dans `platformio.ini`
4. Tester avec un autre écran si possible

### Problème : Graphiques ne s'affichent pas

**Solutions :**
1. Vérifier le moniteur série pour les erreurs
2. Vérifier que LVGL est initialisé correctement
3. Vérifier la mémoire disponible (ESP32 WROVER recommandé)

### Problème : Compilation échoue

**Solutions :**
1. Nettoyer le projet : `pio run --target clean`
2. Mettre à jour PlatformIO : `pio upgrade`
3. Vérifier les dépendances dans `platformio.ini`
4. Réinstaller l'extension PlatformIO

---

## Intégration avec le Dashboard

### Topics MQTT

**ESP32 publie sur :**
```
respirateur-medvent/data
```

**ESP32 s'abonne à :**
```
respirateur-medvent/config
```

### Format des Données

**Télémesure (ESP32 → Dashboard) :**
```json
{
  "pressure": 22.5,
  "flow": 30.2,
  "volume": 500,
  "fr": 15,
  "peep": 5,
  "mode": "VCV",
  "alarm": false,
  "battery": 94,
  "wifi": true
}
```

**Configuration (Dashboard → ESP32) :**
```json
{
  "mode": "VCV",
  "fr": 15,
  "volume": 500,
  "peep": 5,
  "ie_ratio": 0.5
}
```

### Fréquence de Publication

- **Télémesure** : 100ms (configurable dans `main.cpp`)
- **Configuration** : À la demande depuis le dashboard

---

## Sécurité et Maintenance

### Sécurité

- **Ne pas exposer le broker MQTT publiquement sans authentification**
- Utiliser des mots de passe WiFi forts
- Valider les configurations reçues avant application
- Implémenter des limites de sécurité pour les paramètres

### Maintenance

- **Logs série** : Surveiller régulièrement pour les erreurs
- **Mises à jour** : Mettre à jour PlatformIO et les bibliothèques
- **Sauvegardes** : Garder une copie de la configuration
- **Documentation** : Noter toute modification du câblage

---

## Contact et Support

**Pour toute question technique :**
- Vérifier d'abord la section [Dépannage](#dépannage)
- Consulter les logs série
- Vérifier la documentation PlatformIO : https://docs.platformio.org/

**Dashboard web :**
- URL : [Votre URL Vercel]
- Topics MQTT : `respirateur-medvent/*`

---

## Checklist de Déploiement

- [ ] Installer VS Code + PlatformIO
- [ ] Configurer le WiFi dans `main.cpp`
- [ ] Configurer le broker MQTT dans `main.cpp`
- [ ] Vérifier le câblage de l'écran TFT
- [ ] Connecter l'ESP32 via USB
- [ ] Compiler le firmware
- [ ] Téléverser sur l'ESP32
- [ ] Vérifier les logs série
- [ ] Vérifier l'affichage TFT
- [ ] Tester la connexion WiFi
- [ ] Tester la connexion MQTT
- [ ] Vérifier l'intégration avec le dashboard
- [ ] Tester l'envoi de configuration depuis le dashboard

---

**Document version 1.0 - Dernière mise à jour : 16/07/2026**
