#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "telemetry.h"

#define MENU_BASLIK "\n--- TUFAN ELEKTROMOBİL SÜRÜŞ SİMÜLASYONU ---\n"
#define MENU_SECENEK_1 "1. Gaza Bas\n"
#define MENU_SECENEK_2 "2. Frene Bas\n"
#define MENU_SECENEK_3 "3. Rejeneratif Frenleme Yap\n"
#define MENU_SECENEK_4 "4. Anlık Telemetri ve İstatistikleri Oku\n"
#define MENU_SECENEK_5 "5. Sistemi Kapat\n"
#define MENU_SECIM_METNI "Seçiminiz: "
#define HATA_GECERSIZ_GIRIS "HATA: Lütfen geçerli bir menü numarası giriniz (Harf girmeyiniz)!\n"
#define HATA_GECERSIZ_SECIM "HATA: Lütfen 1 ile 5 arasında bir seçim yapınız!\n"

int main() {

    srand(time(NULL));

    float batarya_sicakligi;
    int sarj_yuzdesi;
    char kapi_durumu;
    int fren_pedali;

    printf("--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---\n");

    printf("Batarya Sıcaklığı (°C): ");
    scanf("%f", &batarya_sicakligi);

    if (batarya_sicakligi < -100 || batarya_sicakligi > 200) {
        printf("HATA: Geçersiz sıcaklık değeri.\n");
        return 1;
    }

    printf("Şarj Yüzdesi (%%): ");
    scanf("%d", &sarj_yuzdesi);

    if (sarj_yuzdesi < 0 || sarj_yuzdesi > 100) {
        printf("HATA: Hatalı sensör verisi. İmkansız şarj seviyesi.\n");
        return 1;
    }

    printf("Kapı Durumu (A/K): ");
    scanf(" %c", &kapi_durumu);

    if (kapi_durumu != 'A' && kapi_durumu != 'K') {
        printf("HATA: Geçersiz kapı durumu.\n");
        return 1;
    }

    printf("Fren Pedalı (1:Basılı, 0:Değil): ");
    scanf("%d", &fren_pedali);

    if (fren_pedali != 0 && fren_pedali != 1) {
        printf("HATA: Geçersiz fren pedalı verisi.\n");
        return 1;
    }

    if (batarya_sicakligi > 60) {
        printf("KRİTİK HATA: Motor Aşırı Isındı! Sürüş Engellendi.\n");
        return 1;
    }

    if (sarj_yuzdesi < 10) {
        printf("UYARI: Batarya Kritik Seviyede! Sürüş Başlatılamaz.\n");
        return 1;
    }

    if (kapi_durumu == 'A') {
        printf("HATA: Kapılar Açık! Lütfen Kapatınız.\n");
        return 1;
    }

    if (fren_pedali == 0) {
        printf("BİLGİ: Güvenlik için frene basarak tekrar deneyin.\n");
        return 1;
    }

    printf("BAŞARILI: SİSTEM HAZIR. MOTOR BAŞLATILIYOR...\n");

    sistemi_kur(batarya_sicakligi, sarj_yuzdesi);

    int secim;
    int scanf_kontrol;

    while (1) {
        printf("%s", MENU_BASLIK);
        printf("%s", MENU_SECENEK_1);
        printf("%s", MENU_SECENEK_2);
        printf("%s", MENU_SECENEK_3);
        printf("%s", MENU_SECENEK_4);
        printf("%s", MENU_SECENEK_5);
        printf("%s", MENU_SECIM_METNI);

        scanf_kontrol = scanf("%d", &secim);

        if (scanf_kontrol != 1) { // Kullanıcı hatalı giriş girdiyse, yani scanf 1 döndürmezse
            printf("%s", HATA_GECERSIZ_GIRIS);
            while (getchar() != '\n');
            continue;
        }

        switch (secim) {
            case 1:
                gaza_bas();
                break;
            case 2:
                frene_bas();
                break;
            case 3:
                rejen_fren();
                break;
            case 4:
                telemetri_ve_istatistik_yazdir();
                break;
            case 5:
                sistemi_kapat();
                break;
            default:
                printf("%s", HATA_GECERSIZ_SECIM);
                break;
        }
    }
    return 0;
}