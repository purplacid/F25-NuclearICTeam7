# Setup

Before starting, make sure you have Python installed.
You can download Python here: https://www.python.org/downloads/
***Probably need VSCode setup instructions too

We have supplied the map data to be used to create your maps and routes, it is called "ontario_drive_network.graphml"  
Create a folder to contain files used in this project. 

First, we will need a .geojson file to hold site names and coordinates which can be made as follows:  
*****Instead of giving can describe the formatting or link to an example file

**{  
  "type": "FeatureCollection",  
  "features": [  
    { "type": "Feature", "properties": { "Name": "Bruce Power" }, "geometry": { "type": "Point", "coordinates": [-81.4944, 44.3275] } },  
    { "type": "Feature", "properties": { "Name": "Darlington" }, "geometry": { "type": "Point", "coordinates": [-78.7167, 43.8667] } },  
    { "type": "Feature", "properties": { "Name": "Chalk River" }, "geometry": { "type": "Point", "coordinates": [-77.4175397, 46.0357699] } },  
    { "type": "Feature", "properties": { "Name": "Ignace" }, "geometry": { "type": "Point", "coordinates": [-91.6423, 49.4192] } }  
  ]  
}**  

Ensure that this new file is also in your project folder

For the main code you will also need some python packages  
Install them by opening your terminal (Command Prompt or PowerShell on Windows) and running: 

pip install osmnx networkx geopandas folium

From here, we can open our folder in VS code and create a new file for our main code. 
| import code         | Purpose                                                  |
| ------------- | -------------------------------------------------------- |
| **import osmnx as ox**     | Downloads Ontario's real road network from OpenStreetMap |
| **import networkx as nx**  | Calculates the shortest route using A\* search algorithm |
| **import geopandas as gpd** | Reads the nuclear site locations from a GeoJSON file     |
| **import folium**    | Draws an interactive map you can view in your browser    |
| **from shapely.geometry import Point** |Used to create our fire source and discourage routing through it|
| **from math import radians, cos, sin, sqrt, atan2**|For dist calculations|

### Load Nuclear location file
**sites = gpd.read_file("nuclear_sites.geojson").to_crs(epsg=4326)**  
This line reads the geojson file we created and confirms for us that coordinates are in the correct format

For debugging, you can include a line printing out the loaded site names,
though this is not necessary for the function of the code as a whole  
**print("Loaded site names:", list(sites["Name"]))**  

### Defining our Heuristic
We can create a function to calculate the direct distance from a node to a site, which will be used when finding
the fastest route. It will take three inputs, the first node, second node, and the road network.

**def haversine(n1, n2, G):**

Next we can find the longitude and latitude of the nodes from graph G

**lon1, lat1 = G.nodes[n1]['x'], G.nodes[n1]['y']  
lon2, lat2 = G.nodes[n2]['x'], G.nodes[n2]['y']**

Now we need to set the radius of the sphere we are measuring on. In this case, we use earth,
which has radius 6371 km

**R = 6371**

We can calculate the difference in longitude and latitude, and want to convert this to radians
so that we can estimate the distance on the curve of the earth, as this uses trig functions

**dlon, dlat = radians(lon2 - lon1), radians(lat2 - lat1)**

**a = sin(dlat/2)^2 + cos(radians(lat1)) * cos(radians(lat2)) * sin(dlon/2)^2**


Finally, we can calculate the distance and convert it to kilometers

**return 2 * R * atan2(sqrt(a), sqrt(1 - a))**


### Find Nearest nodes to sites specifically
These steps specifically locate the nodes for Bruce Power and Ignace:  

**bruce = ox.distance.nearest_nodes(G, sites[sites["Name"] == "Bruce Power"].geometry.iloc[0].x,  
                                     sites[sites["Name"] == "Bruce Power"].geometry.iloc[0].y)**  
Repeat this for the other sites of interest (Ignace, Darlington, Chalk River Labs, etc)

### Creating a map
Now, we have to make some way of actually showing the route we've found, and for that we have folium
This will create an empty map centered around Ontario

**m = folium.Map(location=[sites.geometry.y.mean(), sites.geometry.x.mean()], zoom_start=5)**

### Finding our routes
we can use a built in a* algorithm with our haversine heuristic to calculate
the most optimal route between sites

**bruce_route = nx.astar_path(G, bruce, ignace, heuristic=lambda a, b: haversine(a,b,G), weight="length")  
bruce_route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in bruce_route]  
folium.PolyLine(bruce_route_coords, color="green", weight=5, tooltip="Bruce to Ignace").add_to(m)**  
Again, repeat this for the other sites routing to ignace

Now we need to populate it with route data! We can turn each node into a pair of coordinates, then
use folium to create a line between them and add it to the map

**route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in route]
folium.PolyLine(route_coords, color="green", weight=5).add_to(m)**

We want to highlight our sites with special markers for easy reading, which can be done easily by looping through each site with the following code

**for name in sites["Name"]:
    row = sites[sites["Name"] == name]
    folium.Marker(
        location=[row.geometry.y.values[0], row.geometry.x.values[0]],
        popup=name,
        icon=folium.Icon(color="green" if name == "Ignace" else "blue")
    ).add_to(m)

Finally, we should save the map so that we can open it in our browser, and print out a message
confirming we have saved successfully.

**m.save("Optimized_Nuclear_Route.html")  
print("Map saved as Optimized_Nuclear_Route.html")**

Now that we have functioning code, consider the limitless possibile additions to make this solution more realistic. For example, We can add  
    ### Identify Fire location and radius
We can choose a point to be the midpoint of our fire, then set the size of the area around it.

**fire_location = Point(-82.7569, 47.5939)   
fire_range_deg = 0.1**

Now that we have set an area for our fire, we have to let the algorithm know that routing through the fire would be less optimal. To do this, we increase the weighting of the nodes within the specificed area.
__for u, v, key, data in G.edges(keys=True, data=True):  
    x = (G.nodes[u]['x'] + G.nodes[v]['x']) / 2  
    y = (G.nodes[u]['y'] + G.nodes[v]['y']) / 2  
    midpoint = Point(x, y)  
    if fire_location.buffer(fire_range_deg).contains(midpoint):  
        data["length"] *= 10000__
