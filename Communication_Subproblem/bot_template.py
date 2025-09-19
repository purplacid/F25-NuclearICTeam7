import gspread
import atproto
import json
from google.oauth2.service_account import Credentials
from atproto import Client

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

# Authorize
client = Client()
client.login(username, password)

# TODO: while loop: fetch the most recent status based on time
second_row = sheet.row_values(2)
print(f"2nd row of data: {second_row}")

alert_message = "alert"
status = "status"

# TODO: Detect crisis mode if unresolved, unscheduled stop

# TODO: If message changes post right away; else, post every 15 min

# TODO: Translate coordinates to general location 
location = "location"

# Public post
post = client.send_post('Major alert for ' + location + ' residents: ' + alert_message + '.' + 
status)
