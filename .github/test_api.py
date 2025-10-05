import requests
import json

def test_traffic_api():
    url = "https://511on.ca/api/v2/get/traffic"
    print("\nTesting Traffic API...")
    try:
        resp = requests.get(url, timeout=10)
        print(f"Status Code: {resp.status_code}")
        print(f"Content Type: {resp.headers.get('content-type', 'not specified')}")
        print("\nFirst 500 chars of response:")
        print(resp.text[:500])
    except Exception as e:
        print(f"Error: {e}")

def test_road_conditions_api():
    url = "https://511on.ca/api/v2/get/roadconditions"
    print("\nTesting Road Conditions API...")
    try:
        resp = requests.get(url, timeout=10)
        print(f"Status Code: {resp.status_code}")
        print(f"Content Type: {resp.headers.get('content-type', 'not specified')}")
        print("\nFirst 500 chars of response:")
        print(resp.text[:500])
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    test_traffic_api()
    test_road_conditions_api()