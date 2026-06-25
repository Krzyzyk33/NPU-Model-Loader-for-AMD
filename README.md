# NPU Model Loader for AMD

Ten projekt to ultra-szybki, lokalny serwer zgodny z API OpenAI, zaprojektowany specjalnie po to, by całkowicie przenieść inferencję modeli językowych (LLM) na procesor neuronowy **AMD NPU (XDNA)**. 

Dzięki bezpośredniemu wykorzystaniu NPU, zwalniasz zasoby CPU oraz zintegrowanej/dedykowanej karty graficznej. Całość generowania (prefill oraz decode) wykonywana jest bezpośrednio na koprocesorze, co pozwala oszczędzać energię i pracować w kompletnej ciszy.

## Wymagania Sprzętowe ("Czy to zadziała na moim procesorze?")

**TAK**, jeśli posiadasz procesor AMD Ryzen wyposażony w układ Ryzen AI NPU, z zainstalowanymi sterownikami NPU (oraz bibliotekami `Ryzen AI 1.7.1`).

Działa bezbłędnie z:
- **AMD Ryzen™ AI 300 Series (Strix Point)** - 50 TOPS, tutaj rozwija maksymalną prędkość (np. ponad 10 tokenów/s przy dekodowaniu i dziesiątki tysięcy t/s przy analizie).
- **AMD Ryzen™ 8040 Series (Hawk Point)** - 16 TOPS.
- **AMD Ryzen™ 7040 Series (Phoenix)** - 10 TOPS.

*Uwaga dla posiadaczy starszych serii (7040/8040)*: Może zaistnieć potrzeba użycia mniejszego modelu lub odpowiedniego dopasowania wersji ONNX ze względu na mniejszą ilość pamięci współdzielonej.

## Instalacja i Uruchomienie

Projekt to czysty kod źródłowy aplikacji (C++), aby użytkownik miał pełną kontrolę nad swoim środowiskiem bez zbędnych skryptów. 

1. Skompiluj projekt używając kompilatora obsługującego C++17 (np. MSVC) oraz CMake:
   ```cmd
   cd npu_chat
   mkdir build && cd build
   cmake .. -A x64
   cmake --build . --config Release
   ```
2. Pobierz pożądany model NPU (np. `Qwen2.5-Coder-7B-4K`) w formacie ONNX z biblioteki HuggingFace do katalogu `models`.
3. Uruchom serwer przekazując ścieżkę do modelu:
   ```cmd
   npu_chat.exe -m "..\..\..\models\Qwen2.5-Coder-7B-4K"
   ```

W folderze `examples` znajdziesz proste skrypty integrujące nasz serwer z językami Python oraz Node.js z wykorzystaniem domyślnego pakietu `openai` i funkcji **streamingu na żywo**.

## Jak podłączyć do aplikacji GUI?

Serwer zachowuje się identycznie jak chmura OpenAI, więc jest kompatybilny z każdą nowoczesną aplikacją do obsługi lokalnych LLM (np. **AnythingLLM**, **SillyTavern**, **Chatbox**, **LM Studio UI**, czy różnego rodzaju wtyczki do programowania jak **Continue.dev** w VSCode).

### Instrukcja dla dowolnego UI:
1. Upewnij się, że okno z `start_server.bat` jest uruchomione i serwer nasłuchuje.
2. W ustawieniach swojej aplikacji odszukaj sekcję **AI Providers** lub **Connections**.
3. Wybierz typ: `OpenAI API` (lub `Custom OpenAI`, `Local OpenAI`).
4. Jako **Base URL** (lub API Endpoint) wpisz:
   ```text
   http://127.0.0.1:8085/v1
   ```
5. Pole na API Key (hasło) możesz zostawić puste lub wpisać cokolwiek (np. `brak`), ponieważ nasz serwer go nie wymaga.
6. Ciesz się inferencją 100% zasilaną Twoim NPU!

---
Stworzone w ramach walki o perfekcyjne oprogramowanie na NPU. Licencja MIT.
