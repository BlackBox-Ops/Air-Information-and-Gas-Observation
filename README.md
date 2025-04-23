# AIGO - Air Information and Gas Observation

**AIGO** adalah sistem pemantauan kualitas udara dan gas berbasis sensor yang dirancang untuk memberikan informasi real-time mengenai kondisi lingkungan sekitar. Sistem ini dapat digunakan dalam berbagai skenario, mulai dari lingkungan industri, rumah tangga, hingga fasilitas publik, dengan fokus pada deteksi dini dan pengendalian kualitas udara secara cerdas.

## 🎯 Fitur Utama

- Monitoring real-time untuk parameter udara seperti suhu, kelembaban, dan konsentrasi gas (CO2, CO, NO2, dll).
- Tampilan dashboard interaktif berbasis desktop menggunakan Py.GUI.
- Penyimpanan data terstruktur dengan dukungan database lokal.
- Integrasi dengan sistem kontrol (fan, alarm, LED) berdasarkan logika sensor.
- Protokol komunikasi data menggunakan MQTT/HTTP.
- Dukungan sistem operasi real-time (RTOS) untuk manajemen multitasking.
- Potensi integrasi machine learning untuk prediksi tren kualitas udara.

## 💡 Arsitektur Sistem

AIGO terdiri dari beberapa komponen utama:

- **Perangkat Keras (Hardware):** Sensor gas dan lingkungan, mikrokontroler, aktuator.
- **Perangkat Lunak (Firmware):** RTOS, kontrol sensor dan aktuator, manajemen protokol komunikasi.
- **Dashboard Desktop:** Aplikasi GUI berbasis Python untuk visualisasi dan pengendalian.
- **Database:** Penyimpanan data historis untuk analisis dan visualisasi.
- **Machine Learning (Opsional):** Analisis prediktif dan klasifikasi kualitas udara.

## 📦 Teknologi yang Digunakan

- **Mikrokontroler:** Arduino MKR, ESP32, atau sejenisnya.
- **Bahasa Pemrograman:** C/C++ (Firmware), Python (Dashboard & ML).
- **GUI Library:** Py.GUI / tkinter / PyQt / custom PyCanvas.
- **Database:** SQLite / MySQL / TinyDB.
- **Protokol:** MQTT, HTTP REST.
- **RTOS:** FreeRTOS (opsional).
- **ML Framework:** Scikit-learn / TensorFlow Lite (opsional).

## ⚙️ Cara Kerja Singkat

1. Sensor membaca data lingkungan dan mengirimkannya ke mikrokontroler.
2. Mikrokontroler mengelola data dan mengontrol aktuator berdasarkan ambang batas.
3. Data dikirimkan ke dashboard desktop melalui protokol komunikasi.
4. Dashboard menampilkan data real-time dan menyimpan riwayat ke database lokal.
5. (Opsional) Algoritma ML menganalisis data untuk prediksi atau deteksi anomali.

## 🚀 Status Pengembangan

> Versi awal sedang dalam tahap integrasi sensor dan pengujian komunikasi perangkat.

## 📁 Lisensi

MIT License – bebas digunakan dan dimodifikasi untuk keperluan edukasi dan pengembangan.

## 🤝 Kontribusi

Proyek ini terbuka untuk kolaborasi! Jika kamu tertarik untuk berkontribusi dalam firmware, desain GUI, atau analisis data ML, silakan fork repositori ini dan buat pull request.

---

**EDGAR Lab – Embedded System & Instrumentation**
