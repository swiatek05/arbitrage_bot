# MEXC Bot Arbitrażowy Trójkątny

Wielowątkowy bot napisany w C++20, który wykrywa i wykonuje okazje do arbitrażu trójkątnego na giełdzie MEXC.

System traktuje rynek kryptowalut jako graf skierowany, gdzie węzłami są aktywa (np. USDT, BTC), a krawędziami są kursy wymiany z arkusza zleceń (order book). Wykorzystuje algorytm Bellmana-Forda do znalezienia ujemnych cykli, które matematycznie gwarantują zysk wolny od ryzyka.

## Architektura i Funkcje

* **Algorytm Grafowy:** Identyfikuje opłacalne pętle handlowe za pomocą algorytmu Bellmana-Forda. Aby obsłużyć kursy wymiany, wagi krawędzi są obliczane jako -log(kurs * (1 - opłata)), co zamienia mnożenie kursów na dodawanie.
* **Wielowątkowość:** 
    * Wątek 1 (We/Wy): Asynchronicznie odpytuje REST API MEXC o najnowsze dane arkusza zleceń (bid/ask).
    * Wątek 2 (Obliczenia): Aktualizuje wagi grafu i uruchamia wykrywanie cykli. Jeśli cykl zostanie znaleziony, natychmiast wysyła zlecenia rynkowe (market orders).
* **Wielkość Transakcji i Bezpieczeństwo:** Automatycznie oblicza maksymalny bezpieczny wolumen transakcji, sprawdzając głębokość arkusza zleceń (bid_qty/ask_qty), przy ścisłym przestrzeganiu reguł MEXC `min_notional` i `step_size`, aby uniknąć odrzuceń przez API.
* **Uwierzytelnianie HMAC-SHA256:** Podpisywanie żądań API zaimplementowane bezpiecznie za pomocą OpenSSL.
* **Logowanie:** Zrzuca metryki wykonania (oczekiwany zysk vs. rzeczywisty, szczegóły ścieżki, salda portfela) do logów CSV w celu analizy po transakcjach.

## Wymagania

* Kompilator zgodny z C++20 (GCC, Clang, MSVC)
* CMake (3.25+)
* vcpkg (Menedżer pakietów)

**Zależności** (zarządzane automatycznie przez vcpkg):
* cpr (Zapytania HTTP)
* nlohmann/json (Parsowanie JSON)
* OpenSSL (Podpisy API)
* GTest (Testy jednostkowe)

## Instrukcja Budowania

    git clone https://github.com/swiatek05/arbitrage_bot
    cd arbitrage_bot

    # Skonfiguruj projekt (podmień ścieżkę na swój toolchain vcpkg)
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[sciezka_do_vcpkg]/scripts/buildsystems/vcpkg.cmake

    # Zbuduj plik wykonywalny
    cmake --build build --config Release

## Konfiguracja

Bot wymaga pliku `config.json` w tym samym katalogu co plik wykonywalny.

    {
      "api_key": "TWÓJ_KLUCZ_API",
      "api_secret": "TWÓJ_SEKRET_API",
      "fee": 0.001 
    }

## Użycie

Uruchom bota handlowego:

    ./build/arbitrage_bot

Uruchom testy jednostkowe (walidacja logiki grafowej i matematyki):

    ./build/run_tests
