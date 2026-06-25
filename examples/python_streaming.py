import os
from openai import OpenAI

# Inicjalizacja oficjalnego klienta OpenAI, ale z przekierowaniem na nasz NPU Server
client = OpenAI(
    api_key="brak",
    base_url="http://127.0.0.1:8085/v1"
)

print("Wysyłam zapytanie do serwera NPU...\n")

# Wysyłamy zapytanie ze STREAMINGIEM (stream=True)
response = client.chat.completions.create(
    model="npu-qwen-coder",
    messages=[
        {"role": "system", "content": "Jesteś asystentem działającym na NPU AMD."},
        {"role": "user", "content": "Napisz krótką funkcję w Pythonie odwracającą stringa."}
    ],
    stream=True # Używamy nowo dodanego streamingu SSE!
)

# Odbieramy chunk po chunku w czasie rzeczywistym
for chunk in response:
    content = chunk.choices[0].delta.content
    if content:
        print(content, end="", flush=True)

print("\n\n[Zakończono streamowanie]")
