// Masukkan Library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Definisi Pin
#define merah 11
#define kuning 12
#define hijau 13
#define buzzer 7
#define button 2

// Definisi Durasi 
const int durasimerah = 10000;
const int durasikuning = 1000;
const int durasihijau = 10000;
const int durasiPeringatan = 1500;
const int durasiPejalanKaki = 5000;

// Inisialisasi Hardware
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definisi semua kemungkinan status
#define STATUS_MERAH 0
#define STATUS_KUNING_1 1
#define STATUS_HIJAU 2
#define STATUS_KUNING_2 3
#define STATUS_PERINGATAN 4
#define STATUS_PEJALAN_KAKI 5

int statusNow;
unsigned long statusLast;

// Variabel Global
bool permintaan = false;
unsigned long waktuButtonDitekan = 0;
unsigned long waktuBuzzerLast;
bool statusBuzzer = false;
int detikTerakhir = -1;

// Variabel penyimpan sisa waktu lampu hijau saat dijeda
unsigned long sisaWaktuHijau = 0;

void setup(){

  // Atur semua pin output
  pinMode(merah, OUTPUT);
  pinMode(kuning, OUTPUT);
  pinMode(hijau, OUTPUT);
  pinMode(buzzer, OUTPUT);
  // Atur pin button sebagai HIGH default, LOW saat ditekan
  pinMode(button, INPUT_PULLUP);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // Opening Message 
  lcd.print("Maap bg");
  lcd.setCursor(0, 1);
  lcd.print("Masi pemulakk");

  delay(1500);
  lcd.clear();

  // Mulai program dengan masuk ke STATUS_MERAH
  ubahStatus(STATUS_MERAH);
}

void loop() {
  
  // Ambil waktu saat ini
  unsigned long now = millis();
  // Variabel untuk countdown
  long durasiNow = 0;
  
  // Jika status sekarang MERAH
  if (statusNow == STATUS_MERAH) {
    durasiNow = durasimerah;
    
    // Cek apakah timer merah sudah habis
    if (now - statusLast >= durasiNow) {
      ubahStatus(STATUS_KUNING_1); // Lanjut ke kuning 1
    }
  }

  // Jika status sekarang KUNING 1
  else if (statusNow == STATUS_KUNING_1) {
    durasiNow = durasikuning;

    // Cek apakah timer kuning 1 sudah habis
    if (now - statusLast >= durasiNow) {
      ubahStatus(STATUS_HIJAU); // Lanjut ke hijau
    }
  }

  // Jika status sekarang HIJAU
  else if(statusNow == STATUS_HIJAU) {
    durasiNow = durasihijau;

    // Cek apakah tombil ditekan (LOW)
    if (digitalRead(button) == LOW) {
      // Cek debounce
      if (permintaan == false && (now - waktuButtonDitekan > 1000)) {
        permintaan = true; // Catat permintaan
        waktuButtonDitekan = now;
        
        // Hitung sisa waktu hijau
        sisaWaktuHijau = (statusLast + durasihijau) - now;
        // Kurangi sisa waktu dengan durasi interupsi
        sisaWaktuHijau = sisaWaktuHijau - durasiPeringatan - durasiPejalanKaki;
        
        // Pastikan tidak negatif
        if (sisaWaktuHijau < 0) sisaWaktuHijau = 0;
        
        // Langsung ganti status ke PERINGATAN (interupsi)
        ubahStatus(STATUS_PERINGATAN); 
      }
    }
    
    // Cek timer normal (HANYA jika tidak ada permintaan interupsi)
    else if (permintaan == false && (now - statusLast >= durasiNow)) {
      ubahStatus(STATUS_KUNING_2); // Lanjut ke kuning 2
    }
  }

  // Jika status sekarang KUNING 2
  else if (statusNow == STATUS_KUNING_2) {
    durasiNow = durasikuning;

    // Cek apakah timer 2 sudah habis
    if (now - statusLast >= durasiNow) {
      ubahStatus(STATUS_MERAH); // Kembali ke merah
    }
  }
  
  // Jika status sekarang PERINGATAN (kuning interupsi)
  else if (statusNow == STATUS_PERINGATAN) {
    durasiNow = durasiPeringatan;
    
    jalankanBuzzer(now); // Buzzer mulai berbunyi disini
    
    // Cek apakah timer peringatan sudah habis
    if (now - statusLast >= durasiNow) {
      ubahStatus(STATUS_PEJALAN_KAKI); // Lanjut ke pejalan kaki
    }
  }
  
  // Jika status sekarang PEJALAN KAKI (merah interupsi)
  else if (statusNow == STATUS_PEJALAN_KAKI) {
    durasiNow = durasiPejalanKaki;
    
    jalankanBuzzer(now); // Buzzer terus berbunyi
    
    // Cek apakah timer pejalan kaki sudah habis
    if (now - statusLast >= durasiNow) {
      ubahStatus(STATUS_HIJAU); // Kembali ke HIJAU
    }
  }

  // Panggil fungsi countdown di setiap loop
  updateLCD(now, durasiNow);
}

// Fungsi untuk mengubah status
void ubahStatus(int newStatus) {
  statusNow = newStatus; // Simpan status baru
  detikTerakhir = -1; // Reset countdown LCD
  lcd.clear();

  digitalWrite(buzzer, LOW); // Selalu matikan buzzer diawal

  // Jika status baru adalah MERAH
  if (newStatus == STATUS_MERAH) {
    statusLast = millis(); // Reset timer ke "sekarang"
    digitalWrite(merah, HIGH);
    digitalWrite(kuning, LOW);
    digitalWrite(hijau, LOW);
    lcd.setCursor(0, 0);
    lcd.print("LAMPU MERAH");
  }

  // Semua status yang menyalakan lampu kuning
  else if (newStatus ==  STATUS_KUNING_1 || newStatus == STATUS_KUNING_2 || newStatus == STATUS_PERINGATAN) {
    statusLast = millis(); // Reset timer ke "sekarang"
    digitalWrite(merah, LOW);
    digitalWrite(kuning, HIGH);
    digitalWrite(hijau, LOW);
    lcd.setCursor(0, 0);
    lcd.print("LAMPU KUNING");
    
    // Jika ini status PERINGATAN, tambahkan teks & reset buzzer
    if (newStatus == STATUS_PERINGATAN) {
       lcd.setCursor(0, 1);
       lcd.print("PERINGATAN!");
       waktuBuzzerLast = millis(); // Siapkan buzzer untuk mulai "bip"
       statusBuzzer = false;
    }
  }

  // Jika status baru adalah HIJAU
  else if (newStatus == STATUS_HIJAU) {
    // Cek apakah kita melanjutkan dari sisa waktu?
    if (sisaWaktuHijau > 0) {
      // Ya: Atur timer seolah-olah sudah berjalan
      statusLast = millis() - (durasihijau - sisaWaktuHijau);
      sisaWaktuHijau = 0; // Hapus susa waktu yang sudah dipakai
    } else {
      // Tidak: Siklus baru, reset timer ke "sekarang"
      statusLast = millis();
    }
    permintaan = false; // Selalu reset "permintaan" saat masuk hijau
    
    digitalWrite(merah, LOW);
    digitalWrite(kuning, LOW);
    digitalWrite(hijau, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("LAMPU HIJAU");
  }
  
  // Jika status baru adalah PEJALAN KAKI
  else if (newStatus == STATUS_PEJALAN_KAKI) {
    statusLast = millis(); // Reset timer ke "sekarang"
    digitalWrite(merah, HIGH); // Lampu lalu lintas MERAH
    digitalWrite(kuning, LOW);
    digitalWrite(hijau, LOW);
        
    lcd.setCursor(0, 0);
    lcd.print("PENYEBERANGAN");
    lcd.setCursor(0, 1);
    lcd.print("Silakan Jalan");
    //(Timer buzzer tidak direset di sini, agar lanjut berbunyi)
  }
}

// Fungsi membuat buzzer berbunyi tanpa DELAY
void jalankanBuzzer(unsigned long now) {
  // Cek apakah sudah 250ms sejak aksi buzzer terakhir
  if (now - waktuBuzzerLast >= 250) {
    waktuBuzzerLast = now; // Catat waktu "bip"

    // Balik status buzzer
    if (statusBuzzer == false) {
      digitalWrite(buzzer, HIGH);
      statusBuzzer = true;
    } else {
      digitalWrite(buzzer, LOW);
      statusBuzzer = false;
    }
  }
}

// Fungsi menampilkan countdown di LCD
void updateLCD(unsigned long now, long totalDurasi) {
  // Jangan tampilkan countdown jika durasi 0
  if (totalDurasi == 0) return;

  // Hitung sisa waktu
  long waktuBerjalan = now - statusLast;
  long sisaWaktu = totalDurasi - waktuBerjalan;
  // Ubah ke detik (pembulatan atas)
  int detik = (sisaWaktu + 999) / 1000;

  // Cek agar tidak "flicker" (hanya update jika detik berubah)
  if (detik != detikTerakhir) {
    detikTerakhir = detik;

    lcd.setCursor(14, 0); // Pojok kanan atas
    // Koreksi tampilan jika "sisaWaktuHijau" membuat detik < 0
    if (detik < 0) detik = 0;
    // Tambahkan 0 di depan angka satuan
    if (detik < 10) {
      lcd.print("0");
    }
    lcd.print(detik);
  }
}