import requests
import json
import geopandas as gpd
from shapely.geometry import Point, LineString
import pandas as pd

def fetch_construction_sites():
    """
    Fetches construction site data from Ontario 511 API
    Returns a GeoDataFrame with significant construction impacts
    Only includes sites with:
    - Delays of 10+ minutes
    - Lane closures affecting truck routes
    - Major road work on highways
    """
    try:
        response = requests.get('https://511on.ca/api/v2/get/constructionprojects')
        if response.status_code == 200:
            data = response.json()
            
            # Process construction sites
            construction_sites = []
            for site in data:
                desc = site['Description'].lower()
                
                # Skip sites with no significant impact
                if 'no impact' in desc or 'no restriction' in desc:
                    continue
                    
                # Calculate impact level
                delay = 0
                if 'up to' in desc and 'minutes' in desc:
                    try:
                        delay = int(''.join(filter(str.isdigit, desc.split('up to')[1].split('minutes')[0])))
                    except:
                        delay = 0

                # Only include significant impacts
                is_significant = any([
                    delay >= 10,  # Significant delays
                    'lane closure' in desc and ('truck' in desc or any(road in site['RoadwayName'] for road in ['401', '400', '417', '17'])),  # Major highways
                    'full closure' in desc,
                    'reduced to single lane' in desc and ('truck' in desc or 'width' in desc),  # Lane restrictions affecting trucks
                    'width' in desc and any(str(w) in desc for w in range(3, 5)),  # Width restrictions under 5m
                    site['IsFullClosure'] == True
                ])
                    
                if is_significant and site['Latitude'] and site['Longitude']:
                    point = Point(site['Longitude'], site['Latitude'])
                    
                    # Calculate impact severity (1-10 scale)
                    severity = min(10, max(1, delay // 5))  # 5 min delay = 1, 50+ min delay = 10
                    if site['IsFullClosure']:
                        severity = 10
                    if 'width' in desc and any(str(w) in desc for w in range(3, 5)):
                        severity = max(severity, 8)
                    
                    construction_sites.append({
                        'geometry': point,
                        'id': site['ID'],
                        'source_id': site['SourceId'],
                        'description': site['Description'],
                        'road': site['RoadwayName'],
                        'impact_delay': delay,
                        'severity': severity,
                        'start_date': site['StartDate'],
                        'end_date': site['PlannedEndDate'],
                        'is_full_closure': site['IsFullClosure']
                    })
            
            # Create GeoDataFrame
            if construction_sites:
                gdf = gpd.GeoDataFrame(construction_sites)
                # Add buffer based on severity (larger buffer for more severe impacts)
                gdf['buffer_distance'] = gdf['severity'].apply(lambda x: x * 200)  # 200m per severity point, max 2km
                return gdf
            
    except Exception as e:
        print(f"Error fetching construction data: {e}")
    
    return None

def create_construction_hazards(construction_gdf, route_graph):
    """
    Creates hazard areas from construction sites
    Returns a list of tuples: (hazard_geometry, properties)
    where properties contains id, source_id, description, and impact_level
    """
    if construction_gdf is None or construction_gdf.empty:
        return []
        
    hazards = []
    for _, site in construction_gdf.iterrows():
        # Create buffer around construction site
        buffer_size = site['buffer_distance'] / 100000  # Convert to degrees (approximate)
        hazard = site['geometry'].buffer(buffer_size)
        
        # Include properties with the hazard
        properties = {
            'id': site['id'],
            'source_id': site['source_id'],
            'description': site['description'],
            'road': site['road'],
            'impact_delay': site['impact_delay'],
            'type': 'construction'
        }
        
        hazards.append((hazard, properties))
    
    return hazards
