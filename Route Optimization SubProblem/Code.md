```python
# Not for Students, just reference for coops
import osmnx as ox
import networkx as nx
import geopandas as gpd
import folium
from shapely.geometry import Point
from math import radians, cos, sin, sqrt, atan2

# Load relevant sites from GeoJSON
sites = gpd.read_file("nuclear_sites.geojson").to_crs(epsg=4326)
print("Loaded site names:", list(sites["Name"]))

# Load Map from saved file
G = ox.load_graphml("canada_mapping.graphml")

# Fire Location and size
fire_location = Point(-82.7569, 47.5939)
fire_range_deg = 0.3 

# Increase weighting to penalize travel through fire zone 
for u, v, key, data in G.edges(keys=True, data=True):
    x = (G.nodes[u]['x'] + G.nodes[v]['x']) / 2
    y = (G.nodes[u]['y'] + G.nodes[v]['y']) / 2
    midpoint = Point(x, y)
    if fire_location.buffer(fire_range_deg).contains(midpoint):
        data["length"] *= 10000

# Haversine function
### Calcs dist between two nodes on sphere
def haversine(n1, n2, G):
    lon1, lat1 = G.nodes[n1]['x'], G.nodes[n1]['y']
    lon2, lat2 = G.nodes[n2]['x'], G.nodes[n2]['y']
    R = 6371  # Earth radius in km
    dlon, dlat = radians(lon2 - lon1), radians(lat2 - lat1)
    a = sin(dlat/2)**2 + cos(radians(lat1)) * cos(radians(lat2)) * sin(dlon/2)**2
    return 2 * R * atan2(sqrt(a), sqrt(1 - a))

# Nearest node to Bruce
bruce = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Bruce Power"].geometry.iloc[0].x,
                                     sites[sites["Name"] == "Bruce Power"].geometry.iloc[0].y)

# Nearest node to Ignace
ignace = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Ignace"].geometry.iloc[0].x,
                                      sites[sites["Name"] == "Ignace"].geometry.iloc[0].y)

# Nearest node to Darlington
Darlington = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Darlington"].geometry.iloc[0].x,
                                      sites[sites["Name"] == "Darlington"].geometry.iloc[0].y)

# Nearest node to Chalk River
Chalk_River = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Chalk River"].geometry.iloc[0].x,
                                      sites[sites["Name"] == "Chalk River"].geometry.iloc[0].y)

CNL_Pinawa = ox.distance.nearest_nodes(G, sites[sites["Name"] == "CNL Pinawa"].geometry.iloc[0].x,
                                      sites[sites["Name"] == "CNL Pinawa"].geometry.iloc[0].y)

Point_Lepreau = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Point Lepreau"].geometry.iloc[0].x,
                                      sites[sites["Name"] == "Point Lepreau"].geometry.iloc[0].y)
# Map
m = folium.Map(location=[sites.geometry.y.mean(), sites.geometry.x.mean()], zoom_start=5)

# Calculate routes
bruce_route = nx.astar_path(G, bruce, ignace, heuristic=lambda a, b: haversine(a,b,G), weight="length")
bruce_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in bruce_route]
folium.PolyLine(bruce_route_coords, color="green", weight=5, tooltip="Bruce to Ignace").add_to(m)

Darlington_route = nx.astar_path(G, Darlington, ignace, heuristic=lambda a, b: haversine(a, b, G), weight="length")
Darlington_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in Darlington_route]
folium.PolyLine(Darlington_route_coords, color="red", weight=5, tooltip="Darlington to Ignace").add_to(m)

Chalk_route = nx.astar_path(G, Chalk_River, ignace, heuristic=lambda a, b: haversine(a, b, G), weight="length")
Chalk_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in Chalk_route]
folium.PolyLine(Chalk_route_coords, color="blue", weight=5, tooltip="Chalk River to Ignace").add_to(m)

Point_Lepreau_route = nx.astar_path(G, Point_Lepreau, ignace, heuristic=lambda a, b: haversine(a, b, G), weight="length")
Point_Lepreau_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in Point_Lepreau_route]
folium.PolyLine(Point_Lepreau_route_coords, color="orange", weight=5, tooltip="Point Lepreau to Ignace").add_to(m)

CNL_Pinawa_route = nx.astar_path(G, CNL_Pinawa, ignace, heuristic=lambda a, b: haversine(a, b, G), weight="length")
CNL_Pinawa_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in CNL_Pinawa_route]
folium.PolyLine(CNL_Pinawa_route_coords, color="yellow", weight=5, tooltip="CNL_Pinawa to Ignace").add_to(m)
# Add markers manually
for name in sites["Name"]:
    row = sites[sites["Name"] == name]
    folium.Marker(
        location=[row.geometry.y.values[0], row.geometry.x.values[0]],
        popup=name,
        icon=folium.Icon(color="green" if name == "Ignace" else "blue")
    ).add_to(m)

# Add Fire Markers
folium.Marker(
    location=[fire_location.y, fire_location.x],
    popup="🔥 Fire (Hazard Zone)",
    icon=folium.Icon(color="red", icon="fire", prefix="fa")
).add_to(m)

folium.Circle(
    location=[fire_location.y, fire_location.x],
    radius=30000,  # 30 km
    color="red",
    fill=True,
    fill_opacity=0.2,
    popup="🔥 Fire Hazard Zone"
).add_to(m)

# Save map
m.save("Optimized_Route_with_fire.html")
print("✅ Map saved as 'Optimized_Route_with_fire.html'")
```
                         
                         
