import os
from openai import OpenAI

# Initialize the official OpenAI client, but redirect it to our local NPU Server
client = OpenAI(
    api_key="none",
    base_url="http://127.0.0.1:8085/v1"
)

print("Sending request to the NPU Server...\n")

# Send the request with STREAMING enabled (stream=True)
response = client.chat.completions.create(
    model="npu-qwen-coder",
    messages=[
        {"role": "system", "content": "You are a helpful AI assistant running on AMD NPU."},
        {"role": "user", "content": "Write a short Python function to reverse a string."}
    ],
    stream=True # Use the newly added SSE streaming!
)

# Receive chunk by chunk in real-time
for chunk in response:
    content = chunk.choices[0].delta.content
    if content:
        print(content, end="", flush=True)

print("\n\n[Streaming finished]")
