const { OpenAI } = require('openai');

// Initialize the official OpenAI client, redirecting it to our local NPU Server
const openai = new OpenAI({
  apiKey: 'none',
  baseURL: 'http://127.0.0.1:8085/v1',
});

async function main() {
  console.log("Connected to the NPU Server. Waiting for first tokens (streaming)...\n");

  const stream = await openai.chat.completions.create({
    model: 'npu-model',
    messages: [{ role: 'user', content: 'List 3 popular programming languages and describe each in 1 sentence.' }],
    stream: true,
  });

  // Print tokens to the console in real-time
  for await (const chunk of stream) {
    process.stdout.write(chunk.choices[0]?.delta?.content || '');
  }
  
  console.log("\n\n[Streaming finished]");
}

main();
