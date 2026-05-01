##DISCLAIMER - I do not know Python, so almost all of this code is generated with AI with MY ideas and input. 
#This is a part of webpage development, however, so this is allowed.

from flask import Flask, request, render_template, jsonify, send_file, Response
from openai import OpenAI
from datetime import datetime
import pathlib
import json
import os
import requests

app = Flask(__name__) #starts Flask, which runs the server and the AI in it
client = OpenAI(api_key = "YOUR_OPENAI_API_KEY") #gets the OpenAI API

#Enter Billy Personality
with open("billy_prompt.txt") as f: #gives the AI a custom personality through this prompt
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
    return render_template("index.html") #imports the html webpage

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

    if "weather" in question.lower():

        answer = get_weather() #if user's question has the word "weather" in it, it responds by getting the weather

    elif "time" in question.lower():

        answer = get_time() #if user's question has the word "time" in it, it responds by getting the current time

    else:
        response = client.responses.create(
            model = "gpt-4.1-nano", #AI model used in Billy
            input = conversation
        )

        answer = response.output_text

    #add response to memory
    conversation.append({
        "role": "assistant",
        "content": answer
    })

    with open("conversation.json", "w") as f:
        json.dump(conversation, f)

    #text to speech as mp3
    speech_mp3 = client.audio.speech.create(
        model = "gpt-4o-mini-tts",
        voice = "ash", #onyx also good choice 
        input = answer
    )

    with open("static/response.mp3", "wb") as f: #creates response.mp3 file for Billy to output
        f.write(speech_mp3.content)
        
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
OPENWEATHER_API_KEY = "YOUR_OPENWEATHER_API_KEY" #can add openWeather if you want to access the weather

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

