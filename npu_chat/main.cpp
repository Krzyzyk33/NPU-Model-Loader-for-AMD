#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <cstdio>
#include <mutex>
#include <ctime>

#include "httplib.h"
#include "json.hpp"
#include "ort_genai.h"

using json = nlohmann::json;

std::mutex model_mutex;

// Global pointers for the generator scope to avoid reloading
std::unique_ptr<OgaModel> model;
std::unique_ptr<OgaTokenizer> tokenizer;
std::unique_ptr<OgaTokenizerStream> tokenizer_stream;

static void print_banner() {
    printf("\n");
    printf("  ========================================\n");
    printf("  || NPU Model Loader for AMD API Server||\n");
    printf("  ||  True NPU inference (OpenAI API)    ||\n");
    printf("  ========================================\n");
    printf("\n");
}

int main(int argc, char* argv[]) {
    std::string model_path = "..\\models\\Qwen2.5-Coder-7B-4K";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-m" || arg == "--model") && i + 1 < argc) {
            model_path = argv[++i];
        }
    }

    print_banner();
    printf("[NPU] Initializing ONNX Runtime GenAI...\n");

    OgaHandle handle;

    printf("[NPU] Loading model from: %s\n", model_path.c_str());
    auto t_load_start = std::chrono::high_resolution_clock::now();

    try {
        model = OgaModel::Create(model_path.c_str());
    } catch (const std::exception& e) {
        fprintf(stderr, "[NPU ERR] Failed to load model: %s\n", e.what());
        return 1;
    }

    auto t_load_end = std::chrono::high_resolution_clock::now();
    double load_time = std::chrono::duration<double>(t_load_end - t_load_start).count();
    printf("[NPU] Model loaded in %.1f seconds.\n", load_time);

    tokenizer = OgaTokenizer::Create(*model);
    tokenizer_stream = OgaTokenizerStream::Create(*tokenizer);

    printf("[NPU] Tokenizer ready.\n\n");

    httplib::Server svr;

    svr.Post("/v1/chat/completions", [&](const httplib::Request &req, httplib::Response &res) {
        try {
            auto j = json::parse(req.body);
            bool stream = j.value("stream", false);

            if (stream) {
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_chunked_content_provider("text/event-stream",
                    [j](size_t offset, httplib::DataSink &sink) {
                        std::lock_guard<std::mutex> lock(model_mutex);
                        
                        std::string prompt = "<|im_start|>system\nYou are a helpful assistant.<|im_end|>\n";
                        if (j.contains("messages")) {
                            for (auto& msg : j["messages"]) {
                                std::string role = msg["role"];
                                std::string content = msg["content"];
                                prompt += "<|im_start|>" + role + "\n" + content + "<|im_end|>\n";
                            }
                        } else if (j.contains("prompt")) {
                            prompt += "<|im_start|>user\n" + j["prompt"].get<std::string>() + "<|im_end|>\n";
                        }
                        prompt += "<|im_start|>assistant\n";

                        auto sequences = OgaSequences::Create();
                        tokenizer->Encode(prompt.c_str(), *sequences);
                        int prompt_tokens = static_cast<int>(sequences->SequenceCount(0));

                        auto params = OgaGeneratorParams::Create(*model);
                        params->SetSearchOption("max_length", j.value("max_tokens", 4096));
                        params->SetSearchOptionBool("do_sample", true);
                        params->SetSearchOption("temperature", j.value("temperature", 0.7));
                        params->SetSearchOption("top_p", j.value("top_p", 0.9));

                        auto generator = OgaGenerator::Create(*model, *params);
                        generator->AppendTokenSequences(*sequences);

                        auto t_start = std::chrono::high_resolution_clock::now();
                        int generated_tokens = 0;

                        while (!generator->IsDone()) {
                            generator->GenerateNextToken();
                            generated_tokens++;
                            auto new_token = generator->GetSequenceData(0)[generator->GetSequenceCount(0) - 1];
                            const char* decoded = tokenizer_stream->Decode(new_token);
                            std::string token_str(decoded);
                            
                            if (token_str.find("<|im_end|>") != std::string::npos || token_str.find("<|endoftext|>") != std::string::npos) {
                                break;
                            }
                            
                            json chunk_json = {
                                {"id", "chatcmpl-npu"},
                                {"object", "chat.completion.chunk"},
                                {"created", std::time(nullptr)},
                                {"choices", {{
                                    {"index", 0},
                                    {"delta", {
                                        {"content", token_str}
                                    }}
                                }}}
                            };
                            std::string sse_msg = "data: " + chunk_json.dump() + "\n\n";
                            sink.write(sse_msg.c_str(), sse_msg.size());
                        }
                        
                        sink.write("data: [DONE]\n\n", 15);
                        sink.done();
                        
                        auto t_end = std::chrono::high_resolution_clock::now();
                        double total_time = std::chrono::duration<double>(t_end - t_start).count();
                        double decode_tps = (total_time > 0) ? generated_tokens / total_time : 0;
                        printf("[API STREAM] Request processed. Generated: %d tokens (Speed: %.1f t/s)\n", generated_tokens, decode_tps);
                        
                        return false; 
                    });
            } else {
                std::lock_guard<std::mutex> lock(model_mutex);
                
                std::string prompt = "<|im_start|>system\nYou are a helpful assistant.<|im_end|>\n";
                if (j.contains("messages")) {
                    for (auto& msg : j["messages"]) {
                        std::string role = msg["role"];
                        std::string content = msg["content"];
                        prompt += "<|im_start|>" + role + "\n" + content + "<|im_end|>\n";
                    }
                } else if (j.contains("prompt")) {
                    prompt += "<|im_start|>user\n" + j["prompt"].get<std::string>() + "<|im_end|>\n";
                }
                prompt += "<|im_start|>assistant\n";

                auto sequences = OgaSequences::Create();
                tokenizer->Encode(prompt.c_str(), *sequences);
                int prompt_tokens = static_cast<int>(sequences->SequenceCount(0));

                auto params = OgaGeneratorParams::Create(*model);
                params->SetSearchOption("max_length", j.value("max_tokens", 4096));
                params->SetSearchOptionBool("do_sample", true);
                params->SetSearchOption("temperature", j.value("temperature", 0.7));
                params->SetSearchOption("top_p", j.value("top_p", 0.9));

                auto generator = OgaGenerator::Create(*model, *params);
                generator->AppendTokenSequences(*sequences);

                auto t_start = std::chrono::high_resolution_clock::now();
                std::string response_text;
                int generated_tokens = 0;

                while (!generator->IsDone()) {
                    generator->GenerateNextToken();
                    generated_tokens++;
                    auto new_token = generator->GetSequenceData(0)[generator->GetSequenceCount(0) - 1];
                    const char* decoded = tokenizer_stream->Decode(new_token);
                    std::string token_str(decoded);
                    
                    if (token_str.find("<|im_end|>") != std::string::npos || token_str.find("<|endoftext|>") != std::string::npos) {
                        break;
                    }
                    
                    response_text += token_str;
                }
                
                auto t_end = std::chrono::high_resolution_clock::now();
                double total_time = std::chrono::duration<double>(t_end - t_start).count();
                double decode_tps = (total_time > 0) ? generated_tokens / total_time : 0;

                json res_j = {
                    {"id", "chatcmpl-npu"},
                    {"object", "chat.completion"},
                    {"created", std::time(nullptr)},
                    {"choices", {{
                        {"index", 0},
                        {"message", {
                            {"role", "assistant"},
                            {"content", response_text}
                        }},
                        {"finish_reason", "stop"}
                    }}},
                    {"usage", {
                        {"prompt_tokens", prompt_tokens},
                        {"completion_tokens", generated_tokens},
                        {"total_tokens", prompt_tokens + generated_tokens}
                    }}
                };

                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_content(res_j.dump(), "application/json");

                printf("[API] Request processed. Generated: %d tokens (Speed: %.1f t/s)\n", generated_tokens, decode_tps);
            }
        } catch (const std::exception& e) {
            printf("[API ERR] %s\n", e.what());
            res.status = 500;
            json err = {{"error", {{"message", e.what()}, {"type", "server_error"}}}};
            res.set_content(err.dump(), "application/json");
        }
    });

    svr.Options("/v1/chat/completions", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    printf("[API] NPU Server listening on http://127.0.0.1:8085 ...\n");
    svr.listen("0.0.0.0", 8085);

    return 0;
}
