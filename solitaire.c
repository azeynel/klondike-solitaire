/*
 * Klondike Solitaire - Terminal Oyunu
 * Derleme: gcc -o solitaire solitaire.c
 * Çalıştırma: ./solitaire
 *
 * Komutlar:
 *   d          - Desteden 3 kart çek
 *   t <n>      - Açık deste kartını sütun n'e taşı (1-7)
 *   m <s> <d>  - Sütun s'den sütun d'ye taşı
 *   f <n>      - Sütun n'den foundation'a taşı
 *   ft         - Açık desteden foundation'a taşı
 *   q          - Çıkış
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Renkler ── */
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BG_DARK "\033[48;5;22m"

/* ── Sabitler ── */
#define SUITS    4
#define RANKS    13
#define DECK_SIZE 52
#define TABLEAU  7
#define DRAW_COUNT 3

/* ── Kart ── */
typedef struct {
    int rank;   /* 1=A, 2-10, 11=J, 12=Q, 13=K */
    int suit;   /* 0=♠ 1=♥ 2=♦ 3=♣ */
    int face_up;
} Card;

/* ── Semboller ── */
const char *SUIT_SYM[] = {"♠", "♥", "♦", "♣"};
const char *RANK_SYM[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

/* kırmızı suit mi? */
int is_red(int suit) { return suit == 1 || suit == 2; }

/* ── Oyun durumu ── */
Card deck[DECK_SIZE];

Card stock[DECK_SIZE];   int stock_top;
Card waste[DECK_SIZE];   int waste_top;   /* tüm açılmış kartlar */
int  waste_show;          /* şu an gösterilen son 3'ün sonu (indeks) */

Card tableau[TABLEAU][DECK_SIZE]; int tab_size[TABLEAU];
Card foundation[SUITS][RANKS];    int found_size[SUITS];

/* ── Deste oluştur & karıştır ── */
void init_deck() {
    int i = 0;
    for (int s = 0; s < SUITS; s++)
        for (int r = 1; r <= RANKS; r++) {
            deck[i].rank = r; deck[i].suit = s; deck[i].face_up = 0;
            i++;
        }
    /* Fisher-Yates */
    srand((unsigned)time(NULL));
    for (int j = DECK_SIZE - 1; j > 0; j--) {
        int k = rand() % (j + 1);
        Card tmp = deck[j]; deck[j] = deck[k]; deck[k] = tmp;
    }
}

/* ── Oyunu kur ── */
void setup_game() {
    init_deck();
    int idx = 0;
    for (int col = 0; col < TABLEAU; col++) {
        tab_size[col] = 0;
        for (int row = 0; row <= col; row++) {
            tableau[col][row] = deck[idx++];
            tableau[col][row].face_up = (row == col) ? 1 : 0;
            tab_size[col]++;
        }
    }
    stock_top = 0;
    while (idx < DECK_SIZE) {
        stock[stock_top++] = deck[idx++];
    }
    waste_top = 0;
    waste_show = 0;
    memset(found_size, 0, sizeof(found_size));
}

/* ── Kart yazdır ──
 * Suit sembollerini (♠♥♦♣) KULLNMIYORUZ çünkü UTF-8 byte genişliği
 * ile terminal sütun genişliği uyuşmuyor → hizalama bozuluyor.
 * Bunun yerine sade ASCII: S H D C
 * Her kart tam 6 görsel karakter: "%-2s%-1s   " → rank(1-2)+suit(1)+3boşluk
 */
static const char *SUIT_ASCII[] = {"M", "K", "R", "S"}; /* Maça Kupa Karo Sinek */

void print_card(Card c) {
    if (!c.face_up) {
        printf(DIM "[##]  " RESET);   /* 6 char: 4+2 */
        return;
    }
    if (is_red(c.suit))
        printf(RED BOLD "%-2s%-1s   " RESET, RANK_SYM[c.rank], SUIT_ASCII[c.suit]);
    else
        printf(BOLD "%-2s%-1s   " RESET, RANK_SYM[c.rank], SUIT_ASCII[c.suit]);
}

void print_card_inline(Card c) {
    if (!c.face_up) { printf(DIM "[##]" RESET); return; }
    if (is_red(c.suit))
        printf(RED BOLD "[%-2s%s]" RESET, RANK_SYM[c.rank], SUIT_ASCII[c.suit]);
    else
        printf(BOLD "[%-2s%s]" RESET, RANK_SYM[c.rank], SUIT_ASCII[c.suit]);
}

/* ── Tahta yazdır ── */
void print_board() {
    printf("\n");

    /* ── Stock & Waste & Foundation ── */
    printf(CYAN "  Deste   Açık (son 3)              Foundation\n" RESET);
    printf("  ");

    /* stock */
    if (stock_top > 0)
        printf(DIM BOLD "[%2d]" RESET, stock_top);
    else
        printf(DIM "[  ]" RESET);

    printf("  ");

    /* waste: son 3 kartı göster */
    int show_start = waste_top - 3;
    if (show_start < 0) show_start = 0;
    if (waste_top == 0) {
        printf("[ ][ ][ ]");
    } else {
        for (int i = show_start; i < waste_top; i++) {
            print_card_inline(waste[i]);
        }
        for (int i = waste_top - show_start; i < 3; i++) printf("    ");
    }

    printf("    ");

    /* foundation */
    for (int s = 0; s < SUITS; s++) {
        if (found_size[s] == 0) {
            if (is_red(s))
                printf(RED "[ %s]" RESET, SUIT_SYM[s]);
            else
                printf("[ %s]", SUIT_SYM[s]);
        } else {
            Card top = foundation[s][found_size[s]-1];
            print_card_inline(top);
        }
        printf(" ");
    }
    printf("\n\n");

    /* ── Sütun başlıkları ── */
    printf(CYAN "   1     2     3     4     5     6     7\n" RESET);

    /* max derinlik */
    int max_rows = 0;
    for (int c = 0; c < TABLEAU; c++)
        if (tab_size[c] > max_rows) max_rows = tab_size[c];

    for (int row = 0; row < max_rows; row++) {
        printf("  ");
        for (int col = 0; col < TABLEAU; col++) {
            if (row < tab_size[col]) {
                print_card(tableau[col][row]);
            } else {
                printf("      "); /* 6 boşluk = bir kartın genişliği */
            }
        }
        printf("\n");
    }

    if (max_rows == 0) {
        printf("  ");
        for (int col = 0; col < TABLEAU; col++)
            printf(DIM "[   ]   " RESET);
        printf("\n");
    }

    printf("\n");
}

/* ── Geçerlilik kontrolleri ── */
int can_place_on_tableau(Card moving, Card base) {
    /* Farklı renk + bir küçük rank */
    return (is_red(moving.suit) != is_red(base.suit)) &&
           (moving.rank == base.rank - 1);
}

int can_place_on_foundation(Card c, int suit) {
    if (c.suit != suit) return 0;
    if (found_size[suit] == 0) return c.rank == 1;
    return c.rank == foundation[suit][found_size[suit]-1].rank + 1;
}

/* ── Komutlar ── */

/* Desteden 3 çek */
void draw_cards() {
    if (stock_top == 0) {
        if (waste_top == 0) { printf(YELLOW "Deste boş!\n" RESET); return; }
        /* Waste'i geri al */
        for (int i = waste_top - 1; i >= 0; i--)
            stock[stock_top++] = waste[i];
        waste_top = 0;
        printf(GREEN "Deste yenilendi.\n" RESET);
        return;
    }
    int drawn = 0;
    while (stock_top > 0 && drawn < DRAW_COUNT) {
        Card c = stock[--stock_top];
        c.face_up = 1;
        waste[waste_top++] = c;
        drawn++;
    }
}

/* Waste'in en üstündeki kartı al */
Card *waste_top_card() {
    if (waste_top == 0) return NULL;
    return &waste[waste_top - 1];
}

/* Waste'den tableau'ya */
void move_waste_to_tab(int col) {
    if (col < 0 || col >= TABLEAU) { printf(YELLOW "Geçersiz sütun.\n" RESET); return; }
    Card *wc = waste_top_card();
    if (!wc) { printf(YELLOW "Açık kart yok.\n" RESET); return; }

    if (tab_size[col] == 0) {
        if (wc->rank != 13) { printf(YELLOW "Boş sütuna sadece K konabilir.\n" RESET); return; }
    } else {
        Card base = tableau[col][tab_size[col]-1];
        if (!base.face_up || !can_place_on_tableau(*wc, base)) {
            printf(YELLOW "Bu kart buraya konulamaz.\n" RESET); return;
        }
    }
    tableau[col][tab_size[col]++] = *wc;
    waste_top--;
}

/* Tableau sütundan sütuna taşı */
void move_tab_to_tab(int src, int dst) {
    if (src < 0 || src >= TABLEAU || dst < 0 || dst >= TABLEAU) {
        printf(YELLOW "Geçersiz sütun.\n" RESET); return;
    }
    if (tab_size[src] == 0) { printf(YELLOW "Kaynak sütun boş.\n" RESET); return; }

    /* En üstteki yüzü açık bloğu bul */
    int move_start = tab_size[src] - 1;
    while (move_start > 0 && tableau[src][move_start-1].face_up)
        move_start--;

    /* Önce tek kartla dene, sonra blok */
    int placed = 0;
    for (int from = move_start; from < tab_size[src] && !placed; from++) {
        Card moving = tableau[src][from];
        if (!moving.face_up) continue;

        int ok = 0;
        if (tab_size[dst] == 0)
            ok = (moving.rank == 13);
        else {
            Card base = tableau[dst][tab_size[dst]-1];
            ok = base.face_up && can_place_on_tableau(moving, base);
        }

        if (ok) {
            int count = tab_size[src] - from;
            for (int i = 0; i < count; i++)
                tableau[dst][tab_size[dst]++] = tableau[src][from + i];
            tab_size[src] = from;
            if (tab_size[src] > 0 && !tableau[src][tab_size[src]-1].face_up)
                tableau[src][tab_size[src]-1].face_up = 1;
            placed = 1;
        }
    }
    if (!placed) printf(YELLOW "Taşıma mümkün değil.\n" RESET);
}

/* Tableau'dan foundation'a */
void move_tab_to_found(int col) {
    if (col < 0 || col >= TABLEAU || tab_size[col] == 0) {
        printf(YELLOW "Geçersiz sütun.\n" RESET); return;
    }
    Card c = tableau[col][tab_size[col]-1];
    if (!c.face_up) { printf(YELLOW "Kart kapalı.\n" RESET); return; }
    int s = c.suit;
    if (!can_place_on_foundation(c, s)) {
        printf(YELLOW "Foundation'a taşınamaz.\n" RESET); return;
    }
    foundation[s][found_size[s]++] = c;
    tab_size[col]--;
    if (tab_size[col] > 0 && !tableau[col][tab_size[col]-1].face_up)
        tableau[col][tab_size[col]-1].face_up = 1;
    printf(GREEN "Foundation'a eklendi: %s%s\n" RESET, RANK_SYM[c.rank], SUIT_SYM[c.suit]);
}

/* Waste'den foundation'a */
void move_waste_to_found() {
    Card *wc = waste_top_card();
    if (!wc) { printf(YELLOW "Açık kart yok.\n" RESET); return; }
    int s = wc->suit;
    if (!can_place_on_foundation(*wc, s)) {
        printf(YELLOW "Foundation'a taşınamaz.\n" RESET); return;
    }
    foundation[s][found_size[s]++] = *wc;
    waste_top--;
    printf(GREEN "Foundation'a eklendi: %s%s\n" RESET, RANK_SYM[wc->rank], SUIT_SYM[wc->suit]);
}

/* ── Kazandı mı? ── */
int check_win() {
    for (int s = 0; s < SUITS; s++)
        if (found_size[s] < RANKS) return 0;
    return 1;
}

/* ── Yardım ── */
void print_help() {
    printf(CYAN
        "\n Komutlar:\n"
        "  d          → Desteden 3 kart çek\n"
        "  t <n>      → Açık kartı sütun n'e taşı  (örn: t 3)\n"
        "  m <s> <d>  → Sütun s'den d'ye taşı      (örn: m 2 5)\n"
        "  f <n>      → Sütun n'den foundation'a    (örn: f 4)\n"
        "  ft         → Açık kartı foundation'a\n"
        "  h          → Bu yardımı göster\n"
        "  q          → Çıkış\n\n"
        RESET);
}

/* ── Ana döngü ── */
int main() {
    setup_game();

    printf(BOLD GREEN "\n  ♠ ♥  KLONDİKE SOLİTAİRE  ♦ ♣\n" RESET);
    printf("  3 kart çekme — Terminal versiyonu\n");
    print_help();

    char line[128];
    while (1) {
        print_board();

        if (check_win()) {
            printf(BOLD GREEN "\n  🎉 TEBRİKLER! Kazandınız! 🎉\n\n" RESET);
            break;
        }

        printf(BOLD "> " RESET);
        if (!fgets(line, sizeof(line), stdin)) break;

        char cmd[16] = {0};
        int a = 0, b = 0;
        int n = sscanf(line, "%15s %d %d", cmd, &a, &b);

        if (strcmp(cmd, "q") == 0) {
            printf("Görüşürüz!\n");
            break;
        } else if (strcmp(cmd, "d") == 0) {
            draw_cards();
        } else if (strcmp(cmd, "t") == 0 && n >= 2) {
            move_waste_to_tab(a - 1);
        } else if (strcmp(cmd, "m") == 0 && n >= 3) {
            move_tab_to_tab(a - 1, b - 1);
        } else if (strcmp(cmd, "f") == 0 && n >= 2) {
            move_tab_to_found(a - 1);
        } else if (strcmp(cmd, "ft") == 0) {
            move_waste_to_found();
        } else if (strcmp(cmd, "h") == 0) {
            print_help();
        } else {
            printf(YELLOW "Bilinmeyen komut. 'h' yazarak yardım alabilirsiniz.\n" RESET);
        }
    }
    return 0;
}
