import gspread
import atproto
from google.oauth2.service_account import Credentials
from atproto import Client

# Define the scope
scopes = ["https://www.googleapis.com/auth/spreadsheets"]

# Authenticate Google Sheets with credentials

creds = Credentials.from_service_account_file("service_account.json", scopes=scopes)
gc = gspread.authorize(creds)

# Open the Google Sheet
sheet = client.open('Nuclear_challenge_data').sheet1

# bluesky connection
client = Client()
client.login('marbledmonsoon.bsky.social','aF9E5BQgx0rRfS')

# TODO: while loop: fetch the most recent status based on time
second_row = sheet.row_values(2)
print(f"2nd row of data: {second_row}")

alert_message = null
status = null

# TODO: Detect crisis mode if unresolved, unscheduled stop

# TODO: If message changes post right away; else, post every 15 min

# TODO: Translate coordinates to general location 
location = null

# Public post
post = client.send_post('Major alert for ' + location + ' residents: ' + alert_message + '.' + 
status)
