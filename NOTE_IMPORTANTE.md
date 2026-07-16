# Note Importante - Nature du Projet

## ⚠️ Avertissement

Ce projet est une **simulation logicielle** du système de respirateur artificiel décrit dans le mémoire de fin d'études. Il ne constitue pas un prototype physique fonctionnel et ne doit en aucun cas être utilisé sur un patient réel.

## Différence entre Simulation et Prototype Physique

### Ce qui est actuellement implémenté (Simulation)

**Dashboard Web (React + TypeScript)**
- Interface utilisateur complète en français
- Graphiques temps réel avec Recharts
- Communication MQTT fonctionnelle
- Affichage des paramètres ventilatoires simulés
- Interface de configuration des paramètres

**Firmware ESP32 (Simulation)**
- Interface TFT avec LVGL (320x240)
- Simulation de données respiratoires (ondes sinusoïdales)
- Communication MQTT avec le dashboard
- Affichage local des métriques
- Graphiques temps réel (pression, débit, volume)

### Ce qui est décrit dans le mémoire (Prototype Physique)

Le mémoire décrit un prototype physique complet incluant :

**Matériel**
- Mécanisme de compression BVM avec servomoteur
- Capteurs réels : MPXV7025DP (pression), SFM3000 (débit)
- Régulation PID réelle (Kp=1.8, Ki=0.4, Kd=0.2)
- Système d'alarmes matérielles et logicielles
- Alimentation de secours (batterie Li-Ion)
- Interface locale LCD 16x2 + boutons

**Logiciel**
- FreeRTOS avec tâches temps réel
- Contrôle PID sur la pression des voies aériennes
- Modes ventilatoires : VCV, PCV, VAC
- Système de sécurité avec watchdog
- Mode de ventilation de secours

**IoT**
- Broker MQTT Mosquitto local
- Node-RED pour traitement des données
- InfluxDB pour stockage temporel
- Grafana pour visualisation

## Correspondance entre les Deux Approches

| Aspect | Simulation Actuelle | Mémoire (Prototype Physique) |
|--------|---------------------|------------------------------|
| Dashboard | React + Vite | Node-RED + Grafana |
| MQTT Broker | HiveMQ public | Mosquitto local |
| Données | Simulées (sinusoïdes) | Capteurs réels |
| Contrôle | Aucun | PID réel |
| Actionneur | Aucun | Servomoteur BVM |
| Sécurité | Simulation | Matérielle + logicielle |

## Pourquoi cette Simulation ?

Cette simulation a été développée pour :

1. **Démontrer l'interface utilisateur** du dashboard web moderne
2. **Valider l'architecture MQTT** entre ESP32 et dashboard
3. **Faciliter le développement** sans matériel physique
4. **Permettre des tests** de l'interface à distance
5. **Servir de base** pour le développement du prototype physique

## Transition vers Prototype Physique

Pour passer de cette simulation au prototype physique décrit dans le mémoire, il faut :

1. **Acquérir les composants matériels** (voir GUIDE_TRANSITION.md)
2. **Intégrer les capteurs réels** dans le firmware ESP32
3. **Implémenter le contrôle PID** avec servomoteur
4. **Remplacer la simulation** par les lectures capteurs
5. **Ajouter les systèmes de sécurité** matériels
6. **Tester sur banc d'essai** avec poumon-test

## Sécurité et Réglementation

- Ce projet est **uniquement destiné à des fins académiques**
- Il ne respecte pas les normes médicales (ISO 80601-2-12, IEC 60601-1)
- Aucune certification médicale n'a été obtenue
- **Ne jamais utiliser sur un patient réel**
- Le prototype physique décrit dans le mémoire nécessite également une certification avant tout usage clinique

## Documentation Complémentaire

- `GUIDE_TRANSITION.md` : Guide pour passer à un prototype physique
- `firmware/GUIDE_ELECTROMECANICIEN.md` : Guide d'installation du firmware
- `dashboard/README.md` : Documentation du dashboard web
- Mémoire de fin d'études : Description complète du prototype physique

---

**Date** : 16/07/2026  
**Version** : 1.0
