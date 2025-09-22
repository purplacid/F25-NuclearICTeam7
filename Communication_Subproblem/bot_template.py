# Google Sheets accessor
import gspread
import json
import time
# Atproto protocol for accessing Bluesky
from atproto import Client
# Geopy for geocoding 
from geopy.geocoders import Nominatim 
# Google API authorization 
from google.oauth2.service_account import Credentials
# Module for printing data structures nicely
from pprint import pprint 

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
with open("credentials.json", "r") as f: 
    creds = json.load(f)

username = creds["BLUESKY_USERNAME"]
password = creds["BLUESKY_PASSWORD"]

# Authorize Bluesky
client = Client()
client.login(username, password)

# TODO: fetch data using gspread and generate message 
alert_message = "alert" # replace with appropriate alert 
status = "status" # replace with appropriate status (e.g. "Response underway")

# TODO: Translate coordinates to general location using geopy
location = "location"

# Publish post
post = client.send_post('Major alert for ' + location + ":" + alert_message + '.' + 
status)
