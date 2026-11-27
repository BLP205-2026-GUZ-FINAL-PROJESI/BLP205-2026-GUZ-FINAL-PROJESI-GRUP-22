#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;

struct GirdiBitti {};

string satir(const string& istem) {
    cout << istem;
    string deger;
    if (!getline(cin, deger)) throw GirdiBitti{};
    return deger;
}

int sayi(const string& istem, int enAz = 0) {
    while (true) {
        istringstream girdi(satir(istem));
        int deger;
        char fazla;
        if (girdi >> deger && !(girdi >> fazla) && deger >= enAz)
            return deger;
        cout << "En az " << enAz << " olan bir tam sayi girin.\n";
    }
}

string metin(const string& istem) {
    while (true) {
        string deger = satir(istem);
        if (deger.find_first_not_of(" \t\r") != string::npos &&
            deger.find('|') == string::npos)
            return deger;
        cout << "Bos veya | iceren metin kullanmayin.\n";
    }
}

struct Kitap {
    int id;
    string ad;
    string yazar;
    int sayfa;
};

void kitapYaz(const Kitap& kitap) {
    cout << kitap.id << " | " << kitap.ad << " | " << kitap.yazar
         << " | " << kitap.sayfa << " sayfa\n";
}

class Katalog {
    struct Dugum {
        Kitap kitap;
        Dugum* prev;
        Dugum* next;
    };
    Dugum* bas = nullptr;
    Dugum* son = nullptr;

    Dugum* bul(int id) const {
        for (Dugum* gezen = bas; gezen; gezen = gezen->next)
            if (gezen->kitap.id == id) return gezen;
        return nullptr;
    }

public:
    ~Katalog() {
        while (bas) {
            Dugum* silinecek = bas;
            bas = bas->next;
            delete silinecek;
        }
    }

    // Konum 1: bas, 2: son, 3: hedef ID'nin oncesi.
    bool ekle(const Kitap& kitap, int konum, int hedef = 0) {
        if (bul(kitap.id)) {
            cout << "Bu kitap ID zaten var.\n";
            return false;
        }
        Dugum* sonraki = nullptr;
        if (konum == 1) sonraki = bas;
        if (konum == 3) {
            sonraki = bul(hedef);
            if (!sonraki) {
                cout << "Hedef kitap ID bulunamadi.\n";
                return false;
            }
        }
        Dugum* onceki = sonraki ? sonraki->prev : son;
        Dugum* yeni = new Dugum{kitap, onceki, sonraki};
        // Her komsuyu yeni dugume bagla ve gerekiyorsa liste ucunu guncelle.
        if (onceki) onceki->next = yeni;
        else bas = yeni;
        if (sonraki) sonraki->prev = yeni;
        else son = yeni;
        return true;
    }

    bool sil(int id) {
        Dugum* hedef = bul(id);
        if (!hedef) {
            cout << "Kitap ID bulunamadi.\n";
            return false;
        }
        if (hedef->prev) hedef->prev->next = hedef->next;
        else bas = hedef->next;
        if (hedef->next) hedef->next->prev = hedef->prev;
        else son = hedef->prev;
        delete hedef;
        return true;
    }

    void listele(bool ters) const {
        if (!bas) cout << "Katalog bos.\n";
        for (Dugum* gezen = ters ? son : bas; gezen;
             gezen = ters ? gezen->prev : gezen->next)
            kitapYaz(gezen->kitap);
    }

    void analiz() const {
        if (!bas) {
            cout << "Katalog bos.\n";
            return;
        }
        int esik = sayi("Sayfa esigi: ");
        int adet = 0;
        long long toplam = 0;
        const Kitap* az = &bas->kitap;
        const Kitap* cok = &bas->kitap;
        for (Dugum* gezen = bas; gezen; gezen = gezen->next) {
            const Kitap& kitap = gezen->kitap;
            ++adet;
            toplam += kitap.sayfa;
            if (kitap.sayfa < az->sayfa) az = &kitap;
            if (kitap.sayfa > cok->sayfa) cok = &kitap;
        }
        cout << "Kitap sayisi: " << adet << "\nToplam sayfa: " << toplam
             << "\nOrtalama: " << fixed << setprecision(2)
             << static_cast<double>(toplam) / adet << "\nEn az: ";
        kitapYaz(*az);
        cout << "En cok: ";
        kitapYaz(*cok);
        cout << "Esigi asan kitaplar:\n";
        bool bulundu = false;
        for (Dugum* gezen = bas; gezen; gezen = gezen->next) {
            if (gezen->kitap.sayfa > esik) {
                kitapYaz(gezen->kitap);
                bulundu = true;
            }
        }
        if (!bulundu) cout << "Esigi asan kitap yok.\n";
    }

    void dosyayaYaz() const {
        ofstream dosya("kitaplar.txt");
        for (Dugum* gezen = bas; gezen && dosya; gezen = gezen->next) {
            const Kitap& k = gezen->kitap;
            dosya << k.id << '|' << k.ad << '|' << k.yazar << '|' << k.sayfa << '\n';
        }
        dosya.close();
        cout << (dosya ? "Katalog kitaplar.txt dosyasina yazildi.\n"
                       : "Yazma hatasi: kitaplar.txt yazilamadi.\n");
    }

    void dosyadanGoster() const {
        ifstream dosya("kitaplar.txt");
        if (!dosya) {
            cout << "kitaplar.txt acilamadi. Once katalogu kaydedin.\n";
            return;
        }
        string kayit;
        bool bos = true;
        while (getline(dosya, kayit)) {
            cout << kayit << '\n';
            bos = false;
        }
        if (dosya.bad()) cout << "Dosya okunurken hata olustu.\n";
        else if (bos) cout << "Dosyada kayit yok.\n";
    }
};

class DuyuruListesi {
    struct Dugum {
        string duyuru;
        Dugum* next;
    };
    Dugum* son = nullptr;

public:
    ~DuyuruListesi() {
        if (!son) return;
        Dugum* gezen = son->next;
        son->next = nullptr; // Halkayi ac, sonra dogrusal liste gibi temizle.
        while (gezen) {
            Dugum* sonraki = gezen->next;
            delete gezen;
            gezen = sonraki;
        }
    }

    void ekle(const string& duyuru) {
        Dugum* yeni = new Dugum{duyuru, nullptr};
        yeni->next = son ? son->next : yeni;
        if (son) son->next = yeni;
        son = yeni;
    }

    void listele() const {
        if (!son) {
            cout << "Duyuru listesi bos.\n";
            return;
        }
        Dugum* bas = son->next;
        Dugum* gezen = bas;
        do {
            cout << gezen->duyuru << '\n';
            gezen = gezen->next;
        } while (gezen != bas);
    }
};

class IslemYigini {
    struct Dugum {
        string islem;
        Dugum* next;
    };
    Dugum* ust = nullptr;

    void ustuSil() {
        Dugum* eski = ust;
        ust = ust->next;
        delete eski;
    }

public:
    ~IslemYigini() {
        while (ust) ustuSil();
    }

    void push(const string& islem) {
        ust = new Dugum{islem, ust};
    }

    void peek() const {
        if (ust) cout << "En ust: " << ust->islem << '\n';
        else cout << "Yigin bos.\n";
    }

    void pop() {
        if (!ust) {
            cout << "Yigin bos.\n";
            return;
        }
        cout << "Cikarilan islem: " << ust->islem << '\n';
        ustuSil();
    }

    void listele() const {
        if (!ust) cout << "Yigin bos.\n";
        for (Dugum* gezen = ust; gezen; gezen = gezen->next)
            cout << gezen->islem << '\n';
    }
};

struct Ogrenci {
    string numara;
    string adSoyad;
    string kitap;
};

void ogrenciYaz(const Ogrenci& ogrenci) {
    cout << ogrenci.numara << " | " << ogrenci.adSoyad << " | "
         << ogrenci.kitap << '\n';
}

class BeklemeKuyrugu {
    struct Dugum {
        Ogrenci ogrenci;
        Dugum* next;
    };
    Dugum* on = nullptr;
    Dugum* arka = nullptr;

    void onuSil() {
        Dugum* eski = on;
        on = on->next;
        if (!on) arka = nullptr;
        delete eski;
    }

public:
    ~BeklemeKuyrugu() {
        while (on) onuSil();
    }

    void enqueue(const Ogrenci& ogrenci) {
        Dugum* yeni = new Dugum{ogrenci, nullptr};
        if (arka) arka->next = yeni;
        else on = yeni;
        arka = yeni;
    }

    void dequeue() {
        if (!on) {
            cout << "Kuyruk bos.\n";
            return;
        }
        cout << "Hizmet verilen: ";
        ogrenciYaz(on->ogrenci);
        onuSil();
    }

    void listele() const {
        if (!on) cout << "Kuyruk bos.\n";
        for (Dugum* gezen = on; gezen; gezen = gezen->next)
            ogrenciYaz(gezen->ogrenci);
    }
};

void modul1(Katalog& katalog, DuyuruListesi& duyurular) {
    while (true) {
        cout << "\nMODUL 1 - Katalog ve duyurular\n"
             << "1 Basa ekle\n2 Sona ekle\n3 ID oncesine ekle\n"
             << "4 ID ile sil\n5 Ileri listele\n6 Geri listele\n7 Analiz\n"
             << "8 Duyuru ekle\n9 Duyurulari listele\n"
             << "10 Dosyaya yaz\n11 Dosyadan goster\n0 Ana menu\n";
        int secim = sayi("Secim: ");
        switch (secim) {
            case 0: return;
            case 1:
            case 2:
            case 3: {
                int hedef = secim == 3 ? sayi("Hedef kitap ID: ", 1) : 0;
                Kitap kitap{sayi("Yeni kitap ID: ", 1), metin("Kitap adi: "),
                            metin("Yazar: "), sayi("Sayfa sayisi: ", 1)};
                if (katalog.ekle(kitap, secim, hedef)) cout << "Kitap eklendi.\n";
                break;
            }
            case 4:
                if (katalog.sil(sayi("Silinecek ID: ", 1))) cout << "Kitap silindi.\n";
                break;
            case 5: katalog.listele(false); break;
            case 6: katalog.listele(true); break;
            case 7: katalog.analiz(); break;
            case 8: duyurular.ekle(metin("Duyuru: ")); break;
            case 9: duyurular.listele(); break;
            case 10: katalog.dosyayaYaz(); break;
            case 11: katalog.dosyadanGoster(); break;
            default: cout << "Gecersiz secim.\n";
        }
    }
}

void modul2(IslemYigini& yigin, BeklemeKuyrugu& kuyruk) {
    while (true) {
        cout << "\nMODUL 2 - Yigin ve kuyruk\n"
             << "1 Push\n2 Pop\n3 Peek\n4 Yigini listele\n"
             << "5 Enqueue\n6 Dequeue\n7 Kuyrugu listele\n0 Ana menu\n";
        switch (sayi("Secim: ")) {
            case 0: return;
            case 1: yigin.push(metin("Islem aciklamasi: ")); break;
            case 2: yigin.pop(); break;
            case 3: yigin.peek(); break;
            case 4: yigin.listele(); break;
            case 5: {
                Ogrenci ogrenci{metin("Ogrenci numarasi: "), metin("Ad soyad: "),
                                metin("Talep edilen kitap: ")};
                kuyruk.enqueue(ogrenci);
                break;
            }
            case 6: kuyruk.dequeue(); break;
            case 7: kuyruk.listele(); break;
            default: cout << "Gecersiz secim.\n";
        }
    }
}

int main() {
    Katalog katalog;
    DuyuruListesi duyurular;
    IslemYigini yigin;
    BeklemeKuyrugu kuyruk;
    try {
        while (true) {
            cout << "\nKAMPUS KUTUPHANESI\n[1] MODUL 1 - Bagli Listeler ve Dosya\n"
                 << "[2] MODUL 2 - Yigin ve Kuyruk\n0 Cikis\n";
            switch (sayi("Secim: ")) {
                case 0: return 0;
                case 1: modul1(katalog, duyurular); break;
                case 2: modul2(yigin, kuyruk); break;
                default: cout << "Gecersiz secim.\n";
            }
        }
    } catch (const GirdiBitti&) {
        cout << "\nGirdi sonlandi. Program kapatiliyor.\n";
    }
}
