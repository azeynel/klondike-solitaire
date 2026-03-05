# ♠ Klondike Solitaire

Terminal tabanlı klasik Klondike Solitaire oyunu. Saf C ile yazılmıştır, tek dosya, sıfır bağımlılık.

![C](https://img.shields.io/badge/language-C-blue) ![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)

---

## Kurulum

### Gereksinimler
- `gcc` derleyicisi (macOS'ta Xcode Command Line Tools, Linux'ta genellikle hazır gelir)

### Derleme

```bash
gcc -o solitaire solitaire.c
```

### Çalıştırma

```bash
./solitaire
```

---

## Oyun Hakkında

Klasik Klondike Solitaire kuralları geçerlidir:

- 7 sütun, soldan sağa 1'den 7'ye kadar kart
- Her sütunun en alttaki kartı açık, üstündekiler kapalıdır
- Kalan 24 kart destede bekler, her seferinde 3'er kart açılır
- Kartlar **farklı renk** ve **azalan sıra** ile sütunlara dizilir
- Tüm kartlar foundation'a taşınınca oyun kazanılır

### Foundation (Hedef)

4 suit için ayrı yığın: **M**aça · **K**upa · **R**aro · **S**inek  
Her yığın A'dan K'ya sıralanmalıdır.

---

## Komutlar

| Komut | Açıklama | Örnek |
|-------|----------|-------|
| `d` | Desteden 3 kart çek | `d` |
| `t <n>` | Açık kartı sütun n'e taşı | `t 3` |
| `m <s> <d>` | Sütun s'den sütun d'ye taşı | `m 2 5` |
| `f <n>` | Sütun n'in üst kartını foundation'a gönder | `f 4` |
| `ft` | Açık destekartını foundation'a gönder | `ft` |
| `h` | Yardım menüsünü göster | `h` |
| `q` | Oyundan çık | `q` |

---

## Ekran Görünümü

```
  Deste   Açık (son 3)              Foundation
  [21]  [9S][6S][4R]    [AM] [AK] [ R] [ S]

   1     2     3     4     5     6     7
  8M    [##]  [##]  [##]  [##]  [##]  [##]
        7S    [##]  [##]  [##]  [##]  [##]
              5S    [##]  [##]  [##]  [##]
                    QS    [##]  [##]  [##]
                          6R    [##]  [##]
                                9M    [##]
                                      10S
```

- `[##]` → kapalı kart
- Kırmızı renkli kartlar: Kupa (K) ve Karo (R)
- Siyah renkli kartlar: Maça (M) ve Sinek (S)

---

## Lisans

MIT
