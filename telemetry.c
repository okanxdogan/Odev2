#include <stdio.h>
#include <stdlib.h>
#include "telemetry.h"


// KAPSÜLLENEN MODÜL DEĞİŞKENLERİ - dışarıdan değiştirilemez static ile tutulur.
static float guncel_hiz = 0.0f;
static float motor_sicakligi;
static float batarya_sicakligi;
static int batarya_yuzdesi;

// DİZİLER VE SAYACLAR
static float hizlanma_kayitlari[MAX_KAYIT];
static float yavaslama_kayitlari[MAX_KAYIT];
static float rejen_kayitlari[MAX_KAYIT];

static int hizlanma_sayaci = 0;
static int yavaslama_sayaci = 0;
static int rejen_sayaci = 0;


static void sinir_kontrolleri_yap() {
    // arabanın güvenli sınırlar içinde çalışmasını sağlamak için sıcaklık ve hız sınırları kontrol edilir.
    // aşırı ısınma durumunda sistem kapatılır.
    if (motor_sicakligi > 90.0f || batarya_sicakligi > 70.0f) {
        printf("\nKRİTİK HATA: Aşırı Isınma! Sistem kapatılıyor.\n");
        exit(1);
    }

    if (motor_sicakligi < 20.0f) { motor_sicakligi = 20.0f; }
    if (batarya_sicakligi < 20.0f) { batarya_sicakligi = 20.0f; }
    if (guncel_hiz > 70.0f) { guncel_hiz = 70.0f; }
    if (batarya_yuzdesi > 100) { batarya_yuzdesi = 100; }

}

void sistemi_kur(float baslangic_sicaklik, int baslangic_sarj) {

    guncel_hiz = 0.0f;
    motor_sicakligi = baslangic_sicaklik;
    batarya_sicakligi = baslangic_sicaklik;
    batarya_yuzdesi = baslangic_sarj;
    
}

void gaza_bas() { // gaza basıldığında hız artar, şarj düşer batarya ısınır.

    if (batarya_yuzdesi <= 1) { // 1 ve altına düşerse sürüş engellenir.
        printf("\nUYARI: Batarya Tükendi! Araç hareket ettirilemez.\n");
        return;
    }

    float artis = (rand() % 81 + 20) / 10.0f; // gaza basıldığında artış oranıdır.
    
    // gaza basıldığından etkilenen parametreler burada ve güncellenirler.
    guncel_hiz += artis;
    batarya_yuzdesi -= 2;
    motor_sicakligi += 5.0f;
    batarya_sicakligi += 2.0f;   
    // hızlanma kaydı tutulur, MAX_KAYIT sınırına kadar kaydedilir. 100 olarak belirlenmiştik, daha fazla kaydedilmez.
    if (hizlanma_sayaci < MAX_KAYIT) {
        hizlanma_kayitlari[hizlanma_sayaci] = artis;
        hizlanma_sayaci++;
    }

    sinir_kontrolleri_yap();
    // kontrolden sonra bilgi basıyoruz. eğer kontrolü geçerse bilgi basılır, geçemezse kritik hata mesajı basılır ve sistem kapatılır.
    printf("[BİLGİ] Gaza basıldı. Araç %.1f km/s hızlandı. Motor: %.1f°C, Batarya: %.1f°C\n", artis, motor_sicakligi, batarya_sicakligi);

} 

void frene_bas() {
    // aşadağı frene basıldığında hız düşer, şarj artar, sıcaklık düşer. Eğer araç zaten duruyorsa uyarı verilir.
    if (guncel_hiz <= 0.0f) {
        printf("\nUYARI: Araç zaten duruyor!\n");
        return;
    }

    float dusus = (rand() % 101 + 50) / 10.0f;

    if (dusus > guncel_hiz) {
        dusus = guncel_hiz;
        guncel_hiz = 0.0f;
    } else {
        guncel_hiz -= dusus;
    }

    motor_sicakligi -= 3.0f;
    batarya_sicakligi -= 1.0f;

    if (yavaslama_sayaci < MAX_KAYIT) {
        yavaslama_kayitlari[yavaslama_sayaci] = dusus;
        yavaslama_sayaci++;
    }

    sinir_kontrolleri_yap();
    printf("[BİLGİ] Frene basıldı. Araç %.1f km/s yavaşladı. Güncel Hız: %.1f km/s\n", dusus, guncel_hiz);
}

void rejen_fren() {
    // aşağıda rejeneratif frenleme yapıldığında hız düşer, şarj artar, sıcaklık düşer. Eğer araç duruyorsa uyarı verilir.
    if (guncel_hiz <= 0.0f) {
        printf("\nUYARI: Araç dururken rejeneratif fren yapılamaz!\n");
        return;
    }

    // 2.0 ile 10.0 arası rastgele yavaşlama
    float dusus = (rand() % 81 + 20) / 10.0f; 
    
    if (dusus > guncel_hiz) {
        dusus = guncel_hiz;
        guncel_hiz = 0.0f;
    } else {
        guncel_hiz -= dusus;
    }

    batarya_yuzdesi += 1;
    motor_sicakligi -= 2.0f;
    batarya_sicakligi += 1.0f;

    if (rejen_sayaci < MAX_KAYIT) {
        rejen_kayitlari[rejen_sayaci] = dusus;
        rejen_sayaci++;
    }

    sinir_kontrolleri_yap();
    printf("[BİLGİ] Rejeneratif fren devrede. Araç %.1f km/s yavaşladı. Batarya şarj oluyor.\n", dusus);
}

void telemetri_ve_istatistik_yazdir() {
    // istatistiklerimizi görmek için kullanılan fonksiyon. Anlık telemetri verileri ve sürüş istatistikleri burada yazdırılır.
    // Anlık Telemetri Verileri //
    printf("\n--- ANLIK TELEMETRİ ---\n");
    printf("Güncel Hız: %.1f km/s\n", guncel_hiz);
    printf("Batarya: %%%d\n", batarya_yuzdesi);
    printf("Motor Sıcaklığı: %.1f °C\n", motor_sicakligi);
    printf("Batarya Sıcaklığı: %.1f °C\n", batarya_sicakligi);

    printf("\n--- SÜRÜŞ İSTATİSTİKLERİ ---\n");
    
    // Hızlanma Ortalaması
    float toplam_hizlanma = 0;
    for (int i = 0; i < hizlanma_sayaci; i++) toplam_hizlanma += hizlanma_kayitlari[i];
    float ort_hizlanma = (hizlanma_sayaci > 0) ? (toplam_hizlanma / hizlanma_sayaci) : 0.0f;
    printf("Gaza Basma Sayısı: %d (Ortalama Artış: %.2f km/s)\n", hizlanma_sayaci, ort_hizlanma);

    // Yavaşlama Ortalaması
    float toplam_yavaslama = 0;
    for (int i = 0; i < yavaslama_sayaci; i++) toplam_yavaslama += yavaslama_kayitlari[i];
    float ort_yavaslama = (yavaslama_sayaci > 0) ? (toplam_yavaslama / yavaslama_sayaci) : 0.0f;
    printf("Frene Basma Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n", yavaslama_sayaci, ort_yavaslama);

    // Rejeneratif Ortalaması
    float toplam_rejen = 0;
    for (int i = 0; i < rejen_sayaci; i++) { toplam_rejen += rejen_kayitlari[i]; }
    float ort_rejen = (rejen_sayaci > 0) ? (toplam_rejen / rejen_sayaci) : 0.0f;
    printf("Rejeneratif Frenleme Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n", rejen_sayaci, ort_rejen);
}

void sistemi_kapat() { // son bilgiler basılır ve sistem kapatılır
    printf("\n[BİLGİ] Sistem kapatılıyor... Son Sürüş İstatistikleri:\n");
    telemetri_ve_istatistik_yazdir();
    printf("\n[BİLGİ] Motor güvenli bir şekilde kapatıldı. İyi günler!\n");
    exit(0);
}