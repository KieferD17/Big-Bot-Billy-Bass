from flask import Flask, request, render_template, jsonify, send_file, Response
from openai import OpenAI
from datetime import datetime
import json
import requests
import queue
import threading
import whisper
import os

#Making so FFmpeg path is available for Whisper
os.environ["PATH"] += os.pathsep + r"C:/ffmpeg/bin"

##API Keys
OPENAI_API_KEY = "OPENAI_API_KEY"
OPENWEATHER_API_KEY = "OPENWEATHER_API_KEY"

app = Flask(__name__)
client = OpenAI(api_key = OPENAI_API_KEY) 

#Enter Billy Personality
with open("billy_prompt.txt") as f:
    system_prompt = f.read()

#Enable conversation history
if os.path.exists("conversation.json"):
    with open("conversation.json") as f:
        conversation = json.load(f)

else:
    conversation = [{"role": "system", "content": system_prompt}]

#load whisper globally
print("Loading model...")
model = whisper.load_model("base")

## Making a singular brain for both the website and the Whisper model
def billy_brain(message):
    global conversation

    conversation.append({
        "role": "user",
        "content": message
    })

    #Built-in tools
    if "weather" in message.lower():
        answer = get_weather() #weather tool

    elif "time" in message.lower():
        answer = get_time() #time tool

    elif "take me to the river" in message.lower():
        answer = "With pleasure"
        billy_events.put("RIVER") #sends signal to index.html to play Take me to the River

    elif "don't worry be happy" in message.lower() or "don't worry, be happy" in message.lower():
        answer = "Watch this"
        billy_events.put("HAPPY") #sends signal to index.html to play Don't Worry Be Happy

    elif "shut up" in message.lower() or "billy stop" in message.lower(): #if user enters stop prompt...
        answer = "Fine. Just say you hate me"
        billy_events.put("STOP") #sends signal index.html to stop everything

    else:
        #create the response file using the AI TTS model
        response = client.responses.create(
            model = "gpt-4.1-nano",
            input = conversation
        )
        answer = response.output_text

    #Update the conversation memory with the answer
    conversation.append({
        "role": "assistant",
        "content": answer
    })

    #save memory
    with open("conversation.json", "w") as f:
        json.dump(conversation, f)

    return answer

##Homepage
@app.route("/")
def home():
    return render_template("index.html") #has server running on the HTML file

#Process audio in background to speed up upload speeds
def process_audio():
    global billy_events #globale variable

    print("Transcribing...")

    result = model.transcribe("recording.wav") #uses Whisper to transcrube the sent recording audio file
    transcript = result["text"]

    #prints the conversation
    print("User said: ", transcript)
    print("Thinking...")
    answer = billy_brain(transcript) #sends transcript to Billy AI to answer as input
    print("Billy says: ", answer)

    #generate the response audio file
    print("Generating speech...")
    speech = client.audio.speech.create(
        model = "gpt-4o-mini-tts",
        voice = "ash",
        input = answer
    )

    with open("static/response.mp3", "wb") as f:
        f.write(speech.content)

    billy_events.put("new_audio") #sends signal to index.html that there is a new audio file to play

    print("Done processing.")

#For voice input from ESP32
@app.route("/upload", methods=["POST"])
def upload():
    audio = request.data

    with open("recording.wav", "wb") as f: #opens recording file
        f.write(audio)

    print("Received WAV!")

    #runs process_audio concurrently for speedier upload
    threading.Thread(target = process_audio, daemon = True).start()

    return jsonify({
        "status": "received"
    })

billy_events = queue.Queue()

#stream signals from server to index.html
@app.route("/stream")
def stream():
    def event_stream():
        while True:
            msg = billy_events.get() # waits for the message to arrive
            yield f"data: {msg}\n\n"

    return Response(event_stream(), mimetype = "text/event-stream")

#Ask Billy (web UI)
@app.route("/ask", methods = ["POST"])
def ask():
    data = request.get_json()
    question = data["question"]

    answer = billy_brain(question) #inputs question asked on website to Billy AI

    #text to speech as mp3
    speech_mp3 = client.audio.speech.create(
        model = "gpt-4o-mini-tts",
        voice = "ash", #onyx also good choice 
        input = answer
    )

    with open("static/response.mp3", "wb") as f:
        f.write(speech_mp3.content)
        
    return jsonify({"answer": answer})

##Add reset button for Billy's memory
@app.route("/reset")
def reset():
    global conversation
    conversation = [{"role": "system", "content": system_prompt}]

    #clears conversation history
    with open("conversation.json", "w") as f:
        json.dump(conversation, f)

    return "Billy forgot everything."

##Add weather feature using OpenWeather
@app.route("/weather") 
def get_weather():
    city = "Minneapolis"

    url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={OPENWEATHER_API_KEY}&units=imperial"

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
