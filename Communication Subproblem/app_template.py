import gspread
import atproto
from oauth2client.service_account import ServiceAccountCredentials
from atproto import Client

# Define the scope
scope = [
    'https://www.googleapis.com/auth/spreadsheets',
    'https://www.googleapis.com/auth/drive'
]

# Authenticate with credentials
credentials = ServiceAccountCredentials.from_json_keyfile_name('credentials.json', scope)
client = gspread.authorize(credentials)

# Open the Google Sheet
sheet = client.open('Nuclear_challenge_data').sheet1

# Fetch the first row of data
first_row = sheet.row_values(1)
print(f"First row of data: {first_row}")

# bluesky connection
client = Client()
client.login('marbledmonsoon.bsky.social','aF9E5BQgx0rRfS')

post = client.send_post('Hello world! I posted this via the Python SDK.')
