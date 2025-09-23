# ⚠️ Alerts and Communication Subproblem 



## For me: todos

* Update main front page README

* Update subproblem README
   * run through instructions and implement suggested changes

* user testing

## Problem Description 
 Though CNL has developed the security systems to ensure safe and effective transport of the nuclear waste, they can never be truly failsafe. In the case of an incident, alerts must be rapidly disseminated to contain the hazards and minimize casualties. 

Your task is to design such a **communication system**, with information separately tailored for the **public** and the **authorities**.

This subproblem highlights the importance of responsible communication in critical infrastructure. The public should be informed, but not exposed to sensitive operational details. At the same time, internal teams must have access to detailed, real-time data for decision-making and emergency response.

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

#### Connect Google Sheets to Looker Studio
1. Visit lookerstudio.google.com

2. Click **“Blank Report”** or **“+ Create”** > **"Report"**

3. Choose “Google Sheets” as your data source

4. Select your spreadsheet and the correct tab (sheet)

5. Click “Add to Report”

#### Add basic widgets
Looker Studio gives you building blocks called widgets to create dashboards. You can use these however you like! 


#### 💡 Suggested improvements 
* 
### 📣 Part 2: Bluesky Alert Bot 

---

#### What is an API?

#### What is Bluesky? 

#### Setting up Google Sheet API 
1.  Set up Google Cloud  
    i. Log into [Google Cloud](https://cloud.google.com/). Do this by following the link, scrolling down, and clicking **"Go to my console."**  

    ii. In the search bar, search and enable **Google Sheets API (Marketplace)**.  

    iii. In the left sidebar, navigate to **"APIs and Services"** > **"Credentials."**  

2. Create a service account  
    i. On the **"Credentials"** page, click **"Create Credentials"** > **"Service Account."**  

    ii. Name the service account and click **"Create and Continue."**  

    iii. Download the JSON key file for the service account. Rename it to `service_account.json`. Move it into the same folder as your Python script for now. 

3. Connect to Google Sheets  
    i. Open your Google Sheet containing all your data. 

    ii. Click the **"Share"** button and add the service account email address (`"client_email":`) found in your `service_account.json`.

    iii. Grant Editor permissions to the email. 

4. Connect your Python script  
    i. In your terminal, install the necessary libraries (replace atproto if you are not using Bluesky):
    ``` bash
     pip install gspread google-auth geopy atproto
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
    Now you can refer to specific columns, tables, rows, and more from your Google Sheets. Read more about how to use gspread [here](https://docs.gspread.org/en/latest/). 

#### Setting up social media connection
1. Start by reading and understanding the developer's documentation for your social media platofrm of choice. The documentation for Bluesky is available [here](https://docs.bsky.app/).

2. Set up a new account and save the credentials in a ``credentials.json`` file. Move that file into a ``secrets`` subfolder.
    ``` json
    {
    "BLUESKY_USERNAME": "username",
    "BLUESKY_PASSWORD": "password"
    }
    ```

3. Import the json decoder module in the header of your Python script. 
    ``` python 
    import json
    ```

4. Paste this snippet into your script. 
    ``` python
    # Load in credentials from credentials.json
    with open("secrets/credentials.json", "r") as f: 
        creds = json.load(f)

    username = creds["BLUESKY_USERNAME"]
    password = creds["BLUESKY_PASSWORD"]
    ```

5. Now you can use your credentials to access your platform's API. For accessing Bluesky with atproto:
    ``` python
    # Authorize Bluesky connection
    client = Client()
    client.login(username, password)
    ```
#### Write your code 


#### Automate via Github Action
We are going to use Github Action for a no-cost way of deploying the "bot". Github Action is a powerful tool that can automate creating new packages, deploying apps, listening for events and conditionally performing actions based on them, and more. [Here's](https://www.youtube.com/watch?v=mFFXuXjVgkU) a great introduction video, but today we are simply using it to run the bot's script every 15 minute. 

1. Go to the Github repository of your project. Go to **"Settings"** > **"Security"** > **"Secrets and variables"** > **Actions**.  

2. In the **"Actions secrets and variables,"** tab, click **"New repository secret"** and paste the content of your ``service_account.json`` inside. Rename the secret to GOOGLE_CREDENTIALS and save.

3. Go back to your repository and create a ``.github`` folder. 

4. Create another ``workflows`` subfolder inside. 

5. Create a new YAML (.yml) file that will contain the content of your automation script. We have provided a starting script that will simply run the bot script every 15 minute. 
    ``` yaml
    # This workflow is used for the Communication Subproblem. 
    # It automates the bot so that its script is run every 15 minute. 
    name: Bot Feed Updates

    # Controls when workflow will run 
    on: 
    # Allows running this workflow manually from the Actions tab of repo
    workflow_dispatch:
    # Run is scheduled every 15 min
    schedule:
        - cron: "*/15 * * * *"

    # A workflow run is made up of one or more JOBS
    jobs: 
    # This workflow contains a single JOB called "feed-update"
    feed-update: 
    # The runner that the job will run on
        runs-on: ubuntu-latest 

        # Steps represent a sequence of tasks that will be executed 
        # as part of the job
        steps:
        # Checks-out your repository under $GITHUB_WORKSPACE, 
        # so your job can access it
        - name: Checkout Repository 
            uses: actions/checkout@v4

        # Set up Python 
        - name: Set up Python
            uses: actions/setup-python@v5
            with: 
                python-version: '3.11'

        # Install Python dependencies 
        # Here, it first installs and updates pip
        # Then finds the requirements.txt file and installs the dependencies listed
        - name: Install Dependencies 
            run: |
            python -m pip install --upgrade pip 
            if [ -f requirements.txt ]; then pip install -r requirements.txt; fi

        # Create the service_accounts.json file that stores Google API credentials 
        # It's in the repository's secrets and not in the public repo, so it will be written to the repo now
        - name: Write service account JSON
            run: |
                cat <<EOF > service_accounts.json
                ${{ secrets.GOOGLE_CREDENTIALS }}
                EOF
                
        # Run the bot's script 
        - name: Run Bot Script 
            run: |
            python "Communication Subproblem/app_template.py"
            echo "Bot feed refreshed"
    ```
    YAML is a very easy language to learn, so you are encouraged to play around with the script and add additional logic.  

6. Test that the script work by going to the **Actions** tab of your repository and manually running the Workflow. 

Now your bot should be automated to run every 15 minute. If you want to stop it from continuously posting, you can disable the workflow until you need to present it. 

You are free to deploy the bot in other ways, such as by using any of the popular cloud infrastructure providers. Options that include generous free trials/always-free plans are [Google Cloud](https://cloud.google.com/free) and [Oracle Cloud Infrastructure](https://www.oracle.com/cloud/free/).
#### 2.5 Test the bot 

#### 💡 Suggested improvements 
* More specific alert types (may have to create new types of data in your dataset.)
* Instead of fixed scheduling at every 15 minute, have the bot only post in response to important updates
* Alerts in French for incidents in Quebec. 
* Better, more secure handling of API credentials.  
* Interactivity (e.g. reply to the bot to get more detailed information.)  
* Better organization of posts (e.g. instead of sending out individual posts, consolidate some into threads)

## Resources 
* [**Link to Google Sheets dataset**](https://docs.google.com/spreadsheets/d/1OjUgnxIO0DFm4mhyJQ-2K-jQ2L1gPBy0DVwrID45a6Y/edit?usp=sharing): Make a copy to adjust and add new data to your liking. 

## FAQs


## Citations

https://medium.com/@thibautdonis1998/automating-your-workflows-on-a-schedule-github-actions-cron-fd7e662083c6  

https://docs.bsky.app/docs/get-started  

https://spreadsheetpoint.com/connect-python-and-google-sheets-15-minute-guide/

