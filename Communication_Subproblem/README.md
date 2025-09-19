# ⚠️ Alerts and Communication Subproblem 



## For me: todos

* Update main front page README

* Update subproblem README
   * run through instructions and implement suggested changes

* Bluesky bot template finish

* Generate more fake data 
    * History 
    * Local authorities in Location sheet 
    * Export

* user testing

## Problem Description 
 Though CNL has developed the security systems to ensure safe and effective transport of the nuclear waste, they can never be truly failsafe. In the case of an incident, alerts must be rapidly disseminated to contain the hazards and minimize casualties. 

Your task is to design such a **communication system**, with information separately tailored for the **public** and the **authorities**.

### Potential Solutions 
* *Two communication dashboards, one for the public and one for the authorities, that contain real-time updates on the truck's general location, status, and other relevant information.*
* *A social media bot that posts alerts in the event of transport incidents*  
* *A mobile app that sends alert notifications in the event of transport incidents, which will only be displayed to people whose location is close to the incident* 

<br>
🛠️ For this challenge, we will provide the blueprints for a two-part solution: Looker Studio dashboards (which can can split between public and internal communications) and a Bluesky alert bot.

## Solution Template

### 📊 Part 1: Public and Internal Dashboard

---

#### What is a Dashboard?

#### What is Google Looker Studio? 

Google Looker Studio is a free web-based data visualization tool by Google. It allows you to turn raw data — like from Google Sheets — into interactive dashboards and real-time charts, without needing to code. 

It’s perfect for building simulated alert dashboards, interactive maps, or public information panels as part of this challenge. 

#### 💡 Suggested improvements 

### 📣 Part 2: Bluesky Alert Bot 

---

#### What is an API?

#### What is Bluesky? 

#### 2.1 Setting up Google Sheet API 

1. Set up Google Cloud  
    i. Log into [Google Cloud](https://cloud.google.com/). Do this by following the link, scrolling down, and clicking "Go to my console."  

    ii. In the search bar, search and enable Google Sheets API (Marketplace).  

    iii. In the left sidebar, navigate to "APIs and Services" > "Credentials."  

2. Create a service account  
    i. On the "Credentials" page, click "Create Credentials" > "Service Account."  

    ii. Name the service account and click "Create and Continue."  

    iii. Download the JSON key file for the service account. Rename it to `service_account.json`.  

3. Connect to Google Sheets  
    i. Open your Google Sheet containing all your data. 

    ii. Click the "Share" button and add the service account email address (`"client_email"`) found in your `service_account.json`.

    iii. Grant Editor permissions to the email. 

4. Connect your Python script  
    i. In your terminal, install the necessary libraries:  
    ``` bash
     pip install gspread google-auth
    ```  
    ii. Paste the following code into your bot script:  
    ``` python
    import gspread
    from google.oauth2.service_account import Credentials

    # Define the scope
    scopes = [
        "https://www.googleapis.com/auth/spreadsheets",
        "https://www.googleapis.com/auth/drive"
    ]

    # Authenticate Google Sheets with credentials

    creds = Credentials.from_service_account_file("service_account.json", scopes=scopes)
    gc = gspread.authorize(creds)

    # Open the Google Sheet
    sheet = gc.open('Your_Google_Sheet').sheet1
    ```
    Now you can refer to specific columns, tables, rows, etc. Read more about how to use gspread [here](https://docs.gspread.org/en/latest/). 

#### 2.2 Setting up social media connection
1. Start by reading the documentations of 
connect to your platform's api and send the alerts there  

#### 2.3 Write your code 
Find a way to parse the data and generate alerts based on it 

#### 2.4 Deploy via Github Action
Starter guide: https://www.youtube.com/watch?v=mFFXuXjVgkU

iv. Go to the Github repository of your project. Go to "Settings" > "Security" > "Secrets and variables" > Actions.  

v. In "Actions secrets and variables," click "New repository secret" and paste the content of your service_account.json inside. Rename the secret to GOOGLE_CREDENTIALS and save.

#### 2.5 Test the bot 

#### 💡 Suggested improvements 
* More specific alert types (may have to create new types of data in your dataset.)
* Instead of fixed scheduling, have the bot only post in response to important updates
* Better handling of API credentials and security measures 
* Interactivity (e.g. reply to the bot to get more detailed information, etc)  
* Better post organization (e.g. instead of sending out individual posts, consolidate some into threads)

## Resources 

## FAQs


## Citations

https://medium.com/@thibautdonis1998/automating-your-workflows-on-a-schedule-github-actions-cron-fd7e662083c6  

https://docs.bsky.app/docs/get-started  

https://spreadsheetpoint.com/connect-python-and-google-sheets-15-minute-guide/

