const { OpenAI } = require('openai');

// Inicjalizacja oficjalnego klienta OpenAI z przekierowaniem na nasz lokalny NPU Server
const openai = new OpenAI({
  apiKey: 'brak',
  baseURL: 'http://127.0.0.1:8085/v1',
});

async function main() {
  console.log("Połączono z serwerem NPU. Czekam na pierwsze tokeny (streaming)...\n");

  const stream = await openai.chat.completions.create({
    model: 'npu-model',
    messages: [{ role: 'user', content: 'Wymień 3 popularne języki programowania i opisz każdy w 1 zdaniu.' }],
    stream: true,
  });

  // Wypisujemy tokeny na ekran w czasie rzeczywistym
  for await (const chunk of stream) {
    process.stdout.write(chunk.choices[0]?.delta?.content || '');
  }
  
  console.log("\n\n[Zakończono streamowanie]");
}

main();
