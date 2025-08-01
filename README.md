## MPI Fonksiyonları ve Parametreleri Açıklaması

Bu örneklerde paralel programlama için MPI (Message Passing Interface) kullanılmıştır. Aşağıda kullanılan temel MPI fonksiyonları ve parametreleri açıklanmıştır.

---

### `#include <mpi.h>`

MPI fonksiyonlarını kullanabilmek için gerekli olan kütüphane dosyasıdır.

---

### `MPI_Init(&argc, &argv);`

- **İşlev:** MPI ortamını başlatır.
- **Parametreler:**
  - `&argc, &argv`: Komut satırı argümanları. MPI'nin başlatılması için gereklidir.
- **Not:** Tüm MPI programları bu fonksiyonla başlamalıdır.

---

### `MPI_Comm_rank(MPI_COMM_WORLD, &rank);`

- **İşlev:** Mevcut işlemcinin (process) sırasını (rank) öğrenir.
- **Parametreler:**
  - `MPI_COMM_WORLD`: Tüm işlemleri kapsayan iletişim grubu.
  - `&rank`: İşlemcinin rank bilgisinin saklanacağı değişken.
- **Not:** Rank, işlemcinin benzersiz ID'sidir ve 0'dan başlar.

---

### `MPI_Comm_size(MPI_COMM_WORLD, &size);`

- **İşlev:** Toplam işlemci (process) sayısını öğrenir.
- **Parametreler:**
  - `MPI_COMM_WORLD`: İletişim grubu.
  - `&size`: Toplam işlemci sayısını tutan değişken.

---

### `MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);`

- **İşlev:** Bir işlemciden (genellikle 0. rank) tüm işlemcilere veri yayar (broadcast).
- **Parametreler:**
  - `&number`: Yayılacak veri adresi.
  - `1`: Gönderilecek eleman sayısı.
  - `MPI_INT`: Veri tipi (int).
  - `0`: Veriyi gönderen işlemcinin rank'ı.
  - `MPI_COMM_WORLD`: İletişim grubu.

---

### `MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, sub_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);`

- **İşlev:** Ana işlemcideki büyük diziyi (A) parçalara bölerek her işlemciye parça gönderir.
- **Parametreler:**
  - `A`: Gönderilecek veri dizisi (ana işlemcide).
  - `rows_per_proc * N`: Her işlemciye gönderilecek eleman sayısı.
  - `MPI_DOUBLE`: Veri tipi (double).
  - `sub_A`: Alıcı işlemcinin alacağı veri dizisi.
  - `rows_per_proc * N`: Alınacak eleman sayısı.
  - `MPI_DOUBLE`: Veri tipi (double).
  - `0`: Veriyi gönderen işlemcinin rank'ı.
  - `MPI_COMM_WORLD`: İletişim grubu.

---

### `MPI_Barrier(MPI_COMM_WORLD);`

- **İşlev:** Tüm işlemcilerin bu noktada senkronize olmasını sağlar.
- **Parametreler:**
  - `MPI_COMM_WORLD`: İletişim grubu.
- **Not:** Bu fonksiyon çağrılana kadar hiçbir işlemci ilerlemez.

---

### `MPI_Gather(C, rows_per_proc * N, MPI_DOUBLE, result, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);`

- **İşlev:** Her işlemciden alınan parçaları ana işlemcide (0. rank) birleştirir.
- **Parametreler:**
  - `C`: Gönderilecek veri (her işlemcide).
  - `rows_per_proc * N`: Gönderilecek eleman sayısı.
  - `MPI_DOUBLE`: Veri tipi (double).
  - `result`: Ana işlemcide toplanacak veri dizisi.
  - `rows_per_proc * N`: Alınacak eleman sayısı.
  - `MPI_DOUBLE`: Veri tipi (double).
  - `0`: Verileri toplayan işlemcinin rank'ı.
  - `MPI_COMM_WORLD`: İletişim grubu.
  - İşlemci 0: 3.14
  - İşlemci 1: 2.71
  - İşlemci 2: 1.41
  - MPI_Gather sonrası root işlemcide şöyle bir dizi olur: [3.14, 2.71, 1.41]

---

### `MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);`

- **İşlev:** Tüm işlemcilerdeki `mypi` değerlerini toplar (`MPI_SUM`) ve sonucu ana işlemciye (rank 0) gönderir.
- **Parametreler:**
  - `&mypi`: Gönderilecek yerel değer.
  - `&pi`: Toplanan sonucun depolanacağı değişken (sadece rank 0 için anlamlı).
  - `1`: Eleman sayısı.
  - `MPI_DOUBLE`: Veri tipi.
  - `MPI_SUM`: Toplama işlemi.
  - `0`: Sonucun gönderileceği işlemci rank'ı.
  - `MPI_COMM_WORLD`: İletişim grubu.
  - Gather örneğindeki değerler için : MPI_Reduce sonrası root işlemcide sadece toplamları yani 7.26 olur.

---

### `MPI_Wtime();`

- **İşlev:** MPI ortamındaki gerçek zamanı (wall clock time) saniye cinsinden döndürür.
- **Kullanım:** Performans ölçümü için başlangıç ve bitiş zamanı alarak hesaplama süresi bulunabilir.

---

### `MPI_Finalize();`

- **İşlev:** MPI ortamını sonlandırır.
- **Not:** Programın sonunda çağrılmalıdır, bu fonksiyon sonrası MPI fonksiyonları kullanılamaz.
