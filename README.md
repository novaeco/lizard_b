# LizardB - Système de Gestion d'Élevage de Reptiles

## Description

LizardB est un système complet de gestion d'élevage de reptiles développé pour ESP32-S3 avec ESP-IDF. Il offre une solution professionnelle pour la gestion des animaux, la surveillance des terrariums, la gestion des stocks et la conformité réglementaire.

## Fonctionnalités

### 🦎 Gestion des Animaux
- Fiches individuelles détaillées (espèce, âge, sexe, origine)
- Suivi médical et historique des mues
- Suivi de la reproduction
- Historique complet par individu

### 🏠 Surveillance des Terrariums
- Monitoring en temps réel (température, humidité, éclairage)
- Alarmes personnalisables
- Contrôle automatique des équipements
- Historique des données environnementales

### 📦 Gestion des Stocks
- Inventaire automatique des consommables
- Alertes de réapprovisionnement
- Suivi des dates d'expiration
- Gestion des fournisseurs

### 💰 Gestion des Transactions
- Suivi des ventes, achats et échanges
- Génération automatique de certificats
- Conformité réglementaire (CITES)
- Statistiques financières

### ⚖️ Conformité Réglementaire
- Vérification automatique CITES
- Génération de documents obligatoires
- Registre d'élevage automatique
- Conformité française et européenne

### 📊 Interface Graphique (LVGL)
- Tableau de bord moderne et intuitif
- Interface tactile responsive
- Statistiques en temps réel
- Navigation fluide

### 🔒 Sécurité
- Authentification utilisateur
- Gestion des rôles et permissions
- Chiffrement des données sensibles
- Logs d'audit complets

### 🌐 Connectivité
- Interface web sécurisée
- API REST complète
- WebSocket pour les mises à jour temps réel
- Export de données (CSV, JSON, PDF)

## Configuration Matérielle

### ESP32-S3
- **Processeur**: ESP32-S3 (240 MHz, dual-core)
- **Mémoire**: PSRAM 8MB activée
- **Flash**: 16MB minimum
- **Écran**: 800x480 pixels, 16-bit couleur
- **Tactile**: GT911 (I2C)

### Connexions

#### Écran LCD (SPI)
- MOSI: GPIO 11
- CLK: GPIO 12
- CS: GPIO 10
- DC: GPIO 13
- RST: GPIO 14
- BL: GPIO 15

#### Tactile GT911 (I2C)
- SDA: GPIO 19
- SCL: GPIO 20
- INT: GPIO 21
- RST: GPIO 22

## Installation

### Prérequis
- ESP-IDF v5.4.1 ou supérieur
- Python 3.8+
- Git

### Configuration ESP-IDF
```bash
# Installation ESP-IDF
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
. ./export.sh
```

### Compilation du Projet
```bash
# Cloner le projet
git clone <repository-url>
cd lizard_b

# Configuration
idf.py set-target esp32s3
idf.py menuconfig

# Compilation
idf.py build

# Flash
idf.py -p /dev/ttyUSB0 flash monitor
```

### Configuration Menuconfig

#### Composants Requis
- **LVGL**: Activé avec support tactile
- **Wi-Fi**: Configuration selon votre réseau
- **PSRAM**: Mode Octal, 80MHz
- **Partition Table**: Custom (partitions.csv)

#### Paramètres Recommandés
- **CPU Frequency**: 240 MHz
- **Flash Size**: 16MB
- **PSRAM**: 8MB
- **Log Level**: Info (production) / Debug (développement)

## Structure du Projet

```
lizard_b/
├── main/                          # Application principale
│   ├── main.c                     # Point d'entrée
│   ├── app_main.c                 # Logique principale
│   ├── system_init.c              # Initialisation système
│   └── include/                   # Headers principaux
├── components/                    # Composants modulaires
│   ├── lvgl_component/            # Interface graphique
│   ├── animals_manager/           # Gestion des animaux
│   ├── terrarium_monitor/         # Surveillance terrariums
│   ├── stock_manager/             # Gestion des stocks
│   ├── transaction_manager/       # Gestion des transactions
│   ├── regulatory_compliance/     # Conformité réglementaire
│   ├── data_export/              # Export et sauvegarde
│   ├── web_interface/            # Interface web
│   └── security_manager/         # Sécurité et authentification
├── partitions.csv                # Table des partitions
├── sdkconfig.defaults           # Configuration par défaut
└── CMakeLists.txt              # Configuration CMake
```

## Utilisation

### Premier Démarrage
1. **Configuration Wi-Fi**: Accédez aux paramètres pour configurer votre réseau
2. **Création Utilisateur**: Créez le premier compte administrateur
3. **Configuration Terrariums**: Ajoutez vos terrariums et capteurs
4. **Ajout d'Animaux**: Enregistrez vos animaux avec leurs informations

### Interface Tactile
- **Tableau de Bord**: Vue d'ensemble des statistiques
- **Animaux**: Gestion complète des fiches animaux
- **Terrariums**: Surveillance et contrôle environnemental
- **Stocks**: Gestion de l'inventaire
- **Transactions**: Suivi des ventes/achats
- **Paramètres**: Configuration système

### Interface Web
Accédez à `http://[IP_ESP32]` pour l'interface web complète avec toutes les fonctionnalités disponibles.

### API REST
Documentation complète disponible à `http://[IP_ESP32]/api/docs`

## Conformité Réglementaire

### France
- **Arrêté du 8 octobre 2018**: Animaux de compagnie
- **Code de l'environnement**: Espèces protégées
- **Registre d'élevage**: Génération automatique

### Europe
- **Règlement CITES**: Vérification automatique
- **Directive Habitats**: Espèces protégées
- **Règlement sanitaire**: Traçabilité

### International
- **CITES**: Appendices I, II, III
- **IUCN**: Statut de conservation
- **Certificats**: Génération automatique

## Sauvegarde et Export

### Sauvegarde Automatique
- **Intervalle**: Configurable (défaut: 30 minutes)
- **Stockage**: Carte SD ou mémoire interne
- **Chiffrement**: AES-256 optionnel
- **Compression**: Réduction de 60-80% de la taille

### Formats d'Export
- **CSV**: Compatible Excel/LibreOffice
- **JSON**: Intégration avec d'autres systèmes
- **PDF**: Documents officiels et rapports

## Maintenance

### Logs Système
- **Niveau**: Configurable (Error, Warning, Info, Debug)
- **Rotation**: Automatique avec limite de taille
- **Export**: Via interface web ou API

### Mise à Jour
- **OTA**: Mise à jour Over-The-Air via Wi-Fi
- **USB**: Mise à jour par câble USB
- **Sauvegarde**: Automatique avant mise à jour

### Monitoring
- **Mémoire**: Surveillance continue du heap
- **Performance**: Métriques LVGL intégrées
- **Réseau**: Statistiques Wi-Fi et serveur web

## Dépannage

### Problèmes Courants

#### Écran Noir
1. Vérifier les connexions SPI
2. Contrôler l'alimentation du rétroéclairage
3. Vérifier la configuration GPIO

#### Tactile Non Fonctionnel
1. Vérifier les connexions I2C
2. Contrôler l'adresse I2C du GT911
3. Vérifier les GPIO d'interruption

#### Wi-Fi Non Connecté
1. Vérifier SSID et mot de passe
2. Contrôler la portée du signal
3. Vérifier la configuration réseau

#### Mémoire Insuffisante
1. Activer la PSRAM
2. Optimiser la configuration LVGL
3. Réduire les buffers si nécessaire

### Codes d'Erreur

| Code | Description | Solution |
|------|-------------|----------|
| -1   | Erreur générale | Vérifier les logs détaillés |
| -2   | Mémoire insuffisante | Activer PSRAM, réduire buffers |
| -3   | Paramètre invalide | Vérifier les valeurs passées |
| -4   | Élément non trouvé | Vérifier l'existence de l'ID |
| -5   | Timeout | Augmenter les délais d'attente |
| -6   | Erreur stockage | Vérifier la carte SD/NVS |
| -7   | Erreur réseau | Vérifier la connexion Wi-Fi |

## Support et Contribution

### Documentation
- **Wiki**: Documentation détaillée en ligne
- **API**: Documentation Swagger intégrée
- **Exemples**: Code d'exemple pour chaque composant

### Développement
- **Tests**: Suite de tests unitaires
- **CI/CD**: Intégration continue
- **Code Style**: Respect des conventions ESP-IDF

### Licence
Ce projet est sous licence MIT. Voir le fichier LICENSE pour plus de détails.

## Roadmap

### Version 1.1
- [ ] Support Bluetooth pour capteurs externes
- [ ] Application mobile compagnon
- [ ] Intégration cloud optionnelle
- [ ] Reconnaissance vocale

### Version 1.2
- [ ] Intelligence artificielle pour prédictions
- [ ] Intégration caméras de surveillance
- [ ] Système d'alertes SMS/Email
- [ ] Multi-langues (EN, ES, DE)

### Version 2.0
- [ ] Support multi-sites
- [ ] Synchronisation cloud
- [ ] Marketplace d'extensions
- [ ] API publique complète

---

**LizardB** - Votre partenaire pour un élevage de reptiles professionnel et conforme.