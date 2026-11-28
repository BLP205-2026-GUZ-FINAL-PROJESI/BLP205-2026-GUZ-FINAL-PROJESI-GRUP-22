#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <climits>
using namespace std;

string binaIsimleri[] = {
    "Giris", "Ogrenci Isleri", "Yemekhane", "Kafe",
    "Amfi", "Spor Salonu", "Kutuphane"
};
int binaAdedi = 7;

// graf yapisi - komsuluk listesi (komsu, mesafe)
vector< vector< pair<int,int> > > graf;

void kenarEkle(int b1, int b2, int m) {
    graf[b1].push_back(make_pair(b2, m));
    graf[b2].push_back(make_pair(b1, m));
}

void grafiYazdir() {
    cout << "Kampus Graf Yapisi:" << endl << endl;
    for (int i = 0; i < binaAdedi; i++) {
        cout << binaIsimleri[i] << " --> ";
        for (int j = 0; j < graf[i].size(); j++) {
            cout << binaIsimleri[graf[i][j].first] << "(" << graf[i][j].second << "m)";
            if (j < graf[i].size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

// bfs - kuyruk ile katman katman gezme
void BFS(int bas) {
    cout << endl << "BFS (" << binaIsimleri[bas] << " noktasindan):" << endl;

    vector<bool> gidildi(binaAdedi, false);
    queue<int> q;
    q.push(bas);
    gidildi[bas] = true;
    int sira = 1;

    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        cout << sira << ". " << binaIsimleri[cur] << endl;
        sira++;

        for (int i = 0; i < graf[cur].size(); i++) {
            int komsu = graf[cur][i].first;
            if (!gidildi[komsu]) {
                gidildi[komsu] = true;
                q.push(komsu);
            }
        }
    }
}

// dfs - rekursif, bir yolu sonuna kadar gidip geri donuyor
int dfsSira;

void dfsGez(int bina, vector<bool> &gidildi) {
    gidildi[bina] = true;
    cout << dfsSira << ". " << binaIsimleri[bina] << endl;
    dfsSira++;

    for (int i = 0; i < graf[bina].size(); i++) {
        int komsu = graf[bina][i].first;
        if (!gidildi[komsu])
            dfsGez(komsu, gidildi);
    }
}

void DFS(int bas) {
    cout << endl << "DFS (" << binaIsimleri[bas] << " noktasindan):" << endl;
    vector<bool> gidildi(binaAdedi, false);
    dfsSira = 1;
    dfsGez(bas, gidildi);
}

// dijkstra - en kisa yol algoritmasi
void dijkstra(int bas) {
    cout << endl << "Dijkstra (" << binaIsimleri[bas] << " noktasindan en kisa yollar):" << endl << endl;

    vector<int> mesafe(binaAdedi, INT_MAX);
    vector<int> onceki(binaAdedi, -1);
    mesafe[bas] = 0;

    // min-heap: (mesafe, bina)
    priority_queue< pair<int,int>, vector< pair<int,int> >, greater< pair<int,int> > > pq;
    pq.push(make_pair(0, bas));

    while (!pq.empty()) {
        int dist = pq.top().first;
        int cur = pq.top().second;
        pq.pop();

        if (dist > mesafe[cur]) continue; // eski kayit, atla

        for (int i = 0; i < graf[cur].size(); i++) {
            int komsu = graf[cur][i].first;
            int w = graf[cur][i].second;

            if (mesafe[cur] + w < mesafe[komsu]) {
                mesafe[komsu] = mesafe[cur] + w;
                onceki[komsu] = cur;
                pq.push(make_pair(mesafe[komsu], komsu));
            }
        }
    }

    // sonuclari yazdir
    for (int i = 0; i < binaAdedi; i++) {
        if (i == bas) continue;

        cout << binaIsimleri[bas] << " -> " << binaIsimleri[i];
        cout << " | " << mesafe[i] << "m | Rota: ";

        // rotayi bul (geriye dogru takip et)
        vector<int> rota;
        for (int x = i; x != -1; x = onceki[x])
            rota.push_back(x);

        for (int j = rota.size() - 1; j >= 0; j--) {
            cout << binaIsimleri[rota[j]];
            if (j > 0) cout << " -> ";
        }
        cout << endl;
    }
}

int main() {
    graf.resize(binaAdedi);

    // kampus baglantilari
    kenarEkle(0, 1, 50);   // giris - ogrenci isleri
    kenarEkle(0, 2, 100);  // giris - yemekhane
    kenarEkle(0, 3, 150);  // giris - kafe
    kenarEkle(0, 4, 20);   // giris - amfi
    kenarEkle(2, 5, 10);   // yemekhane - spor salonu
    kenarEkle(2, 6, 10);   // yemekhane - kutuphane

    grafiYazdir();
    BFS(0);
    DFS(0);
    dijkstra(0);

    return 0;
}
