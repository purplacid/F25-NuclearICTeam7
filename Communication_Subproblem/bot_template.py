# Google Sheets accessor
import gspread
import json
# Atproto protocol for accessing Bluesky
from atproto import Client
# Google API authorization 
from google.oauth2.service_account import Credentials

# Define the scope
scopes = [
    "https://www.googleapis.com/auth/spreadsheets",
    "https://www.googleapis.com/auth/drive"
]

# Authenticate Google Sheets with credentials
creds = Credentials.from_service_account_file("service_accounts.json", scopes=scopes)
gc = gspread.authorize(creds)

# Open the Google Sheet
sheet = gc.open('Nuclear_challenge_data').sheet1

# Bluesky Connection
# Load in credentials from credentials.json
with open("/secrets/credentials.json", "r") as f: 
    creds = json.load(f)

username = creds["BLUESKY_USERNAME"]
password = creds["BLUESKY_PASSWORD"]

# Authorize Bluesky connection
client = Client()
client.login(username, password)

# TODO: fetch data using gspread and generate message 
alert_message = "Radiation leakage along Provincial Rd 211" # replace with appropriate alert 
status = "Response underway." # replace with appropriate status (e.g. "Response underway")

# TODO: Translate coordinates to general location
location = "Pinawa"

# Publish post
post = client.send_post('Major alert for ' + location + ": " + alert_message + '. ' + 
status)
