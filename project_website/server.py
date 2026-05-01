from flask import Flask, request, render_template, jsonify, send_file, Response
from openai import OpenAI
from datetime import datetime
import pathlib
import json
import os
import requests

app = Flask(__name__)
client = OpenAI(api_key = "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") 

#Enter Billy Personality
with open("billy_prompt.txt") as f:
    system_prompt = f.read()

#Enable conversation history
if os.path.exists("sonversation.json"):
    with open("conversation.json") as f:
        conversation = json.load(f);

else:
    conversation = [{"role": "system", "content": system_prompt}]

##Homepage
@app.route("/")
def home():
    return render_template("index.html")

#Ask Billy
@app.route("/ask", methods = ["POST"])
def ask():

    global conversation

    data = request.get_json()
    question = data["question"]

    #add user message to memory
    conversation.append({
        "role": "user",
        "content": question
    })

    ##memory limit - UNNECESSARY, but could be useful
    # if len(conversation) > 20:
    #     conversation.pop(1)
    
    if "weather" in question.lower():

        answer = get_weather()

    elif "time" in question.lower():

        answer = get_time()

    else:
        response = client.responses.create(
            model = "gpt-4.1-nano",
            input = conversation
            # input = [{"role": "system", "content": system_prompt}, 
            #          {"role": "user", "content": question}]
        )

        answer = response.output_text

    #add response to memory
    conversation.append({
        "role": "assistant",
        "content": answer
    })

    with open("conversation.json", "w") as f:
        json.dump(conversation, f)

    #save text response to txt file - UNNECESSARY
    with open("static/response.txt", "w") as file:
        file.write(answer)

    #text to speech as mp3
    speech_mp3 = client.audio.speech.create(
        model = "gpt-4o-mini-tts",
        voice = "ash", #onyx also good choice 
        input = answer
    )

    with open("static/response.mp3", "wb") as f:
        f.write(speech_mp3.content)

    #text to speech as wav - UNNECESSARY
    speech_wav = client.audio.speech.create(
        model = "gpt-4o-mini-tts",
        voice = "ash",  
        input = answer
    )

    with open("static/response.wav", "wb") as f: # "wb" means write binary
        f.write(speech_wav.content)
        

    return jsonify({"answer": answer})

##Add reset button for Billy's memory
@app.route("/reset")
def reset():

    global conversation
    conversation = [{"role": "system", "content": system_prompt}]

    with open("conversation.json") as f:
        json.dump(conversation, f)

    return "Billy forgot everything."

##Add weather feature using OpenWeather
OPENWEATHER_API_KEY = "c4992bc89c4c807a4fcbfdc0e1549849"

@app.route("/weather") 
def get_weather():
    api_key = OPENWEATHER_API_KEY
    city = "Minneapolis"

    url = f"http://api.openweathermap.org/data/2.5/weather?q={"Minneapolis"}&appid={OPENWEATHER_API_KEY}&units=imperial"

    data = requests.get(url).json()
    print(data)  # debug

    if "main" not in data:
        return f"Billy can't find the weather... ({data.get('message', 'error')})"

    temp = data["main"]["temp"]
    desc = data["weather"][0]["description"]

    return f"Let me come up to the surface real quick... It's {temp} degrees Fahrenheit with {desc}"

@app.route("/time")
def get_time():

    now = datetime.now()

    current_time = now.strftime("%I:%M %p") #Time format (hours:mins PM/AM)

    return f"Now, I can't see the sun that well from here, but I think it's {current_time}"

if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 5000, debug = True, use_reloader = False)

