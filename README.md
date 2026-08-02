# ESD Irrigation System

Firmware for an ESP32-based automated irrigation system. Waters based on weather API driven logic, and sends telemetry to an IoT server for user monitoring.

---

## 🛠️ Developer Setup & Security Guide

This project connects to a ThingsBoard IoT server and fetches weather data. For security and best practices, all local Wi-Fi credentials, API keys, and device access tokens are kept out of source control.

The `src/config.h` file is explicitly blocked by our `.gitignore`. Your project will not compile locally until you have this file set up.

### Choose Your Setup Path:

- **Option 1:** [First-Time Setup (New Clone / New Machine)](#option-1-first-time-setup-new-clone)
- **Option 2:** [Migration Guide (For Existing Devs with an Active `config.h`)](#option-2-migration-guide-for-existing-devs)

---

### Option 1: First-Time Setup (New Clone)

Follow these steps if you are setting up the project on a fresh machine or a clean clone:

#### 1. Sync Your Repository

Ensure you are on the development branch:

```bash
git checkout dev
git pull origin dev
```

#### 2. Create Your Personal Configuration File

Navigate to the `src/` directory, duplicate the template file, and rename it to `config.h`:

- **Mac / Linux:** `cp src/config.example.h src/config.h`
- **Windows (PowerShell):** `Copy-Item src/config.example.h src/config.h`
- **VS Code UI:** Right-click `config.example.h` ➡️ Copy ➡️ Right-click `src/` folder ➡️ Paste ➡️ Rename to `config.h`.

#### 3. Insert Your Local Credentials

Open `src/config.h` and replace the placeholder text with your personal credentials as shown in the [Credential Format Reference](#credential-format-reference).

---

### Option 2: Migration Guide (For Existing Devs)

If you already have a working `config.h` on your computer, running `git pull` right now will cause a Git conflict because the file structure has been modernized. Follow this to update safely without losing your local passwords:

#### 1. Backup Your Credentials

Temporarily rename your working configuration file so Git doesn't overwrite it:

- **Mac / Linux:** `mv src/config.h src/config.backup.h`
- **Windows (PowerShell):** `Rename-Item src/config.h src/config.backup.h`

#### 2. Pull the Repository Updates

```bash
git fetch origin
git checkout dev
git pull origin dev
```

#### 3. Restore Your Credentials

Restore your original settings. The new `.gitignore` rules will instantly protect your file:

- **Mac / Linux:** `mv src/config.backup.h src/config.h`
- **Windows (PowerShell):** `Rename-Item src/config.backup.h src/config.h`

---

### Credential Format Reference

Your personal `src/config.h` should match this structure:

```cpp
#pragma once

// WiFi credentials
constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";
constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Thingsboard device access token
const char* TB_ACCESS_TOKEN = "YOUR_ACTUAL_ACCESS_TOKEN";

// API url for weather data (Open-Meteo API free tier)
static const char* apiUrl = "API_URL";
```

### 🔍 Git Safety Verification

Before staging or committing code, run a quick status check:

```bash
git status
```

- **Safe Output:** `src/config.example.h` and your script modifications may appear, but **`src/config.h` must never be listed anywhere**.
- **Emergency Reset:** If `src/config.h` accidentally slips into your staging area, untrack it instantly by running:
  ```bash
  git rm --cached src/config.h
  ```

⚠️ **Important:** Never modify or commit your real credentials directly into `src/config.example.h`. That file is public and tracked globally!
