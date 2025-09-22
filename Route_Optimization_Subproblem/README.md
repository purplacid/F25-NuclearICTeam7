# Nuclear Transport Route Optimization Challenge

**Welcome!**

Congratulations! You have been chosen to join a special team of engineers and scientists at Canadian Nuclear Labs (CNL). Your mission is to design a system to optimize truck routing, considering environmental challenges and on-route resources, such as gas stations.

Transport trucks could face a vast number of environmental conditions that present challenges for safe transportation—forest fires causing smoke and debris, floods affecting the Trans-Canada highway, as well as snow and Icy conditions. Your solution should ultimately get the truck to its destination, avoid fires and hazards, and stop at designated refueling areas. A list of locations and coordinates has been be provided.

## Setup

Before starting, make sure you have Python installed:
-    You can download Python here: https://www.python.org/downloads/
-    You can download VSCode here: https://code.visualstudio.com/download
-    And here is a link to a tutorial on how to use VSCode if needed: https://code.visualstudio.com/docs/getstarted/getting-started

### Creating a Virtual Environment
A Python virtual environment (venv) is a self-contained directory that allows you to install packages and dependencies for a specific project without affecting the global Python installation or other projects.

1. Download the starter code contained in this folder (it is recommended you use [git](https://www.w3schools.com/git/git_getstarted.asp?remote=github) to download and share these files)
2. Open a terminal in vscode and use `cd [insert absolute path to your project locally]` to navigate to the directory where you stored the code you downloaded in step 1
3. Create a venv:
    - Enter `python -m venv [name-of-your-venv]` in the same terminal. 
    This should create a new folder with the name of your venv under your directory
4. Activate the venv:
    - for Mac/Linux users: `source ./name-of-your-venv/bin/activate `
    - for Windows users: `.\name-of-your-venv\Scripts\activate`
    - When prompted, type R and then enter to run once
    - you should now see `(name-of-your-venv)` being appended to your input prompt, indicating that you have activated your venv

### Install Python Libraries

-   **OSMnx**: To download, model, analyze, and visualize street networks and other geospatial features from OpenStreetMap
-   **NetworkX**: For the creation, manipulation, and study of the structure, dynamics, and functions of complex networks
-   **GeoPandas**: For working with geospatial data
-   **Folium**: For creating interactive web maps
-   **Scikit-Learn**: For statistical modeling

You can download the above Python libraries in your virtual environment by entering this one-line command in your terminal (Command Prompt on Mac or PowerShell on Windows):
```
pip install osmnx networkx geopandas folium scikit-learn
```

## Walkthrough

We have supplied the map data on Learn to be used to create your maps and routes, it is called "ontario_drive_network.graphml". Download it. 
Make sure to keep all the files used in this project in the same folder. 

First, we will need a .geojson file to hold site names and coordinates which can be made as follows:  

<img src="/Route_Optimization_Subproblem/images/geojson.png" alt="GeoJSON file" height="250">
A list of nuclear sites and their coordinates has been provided.
Ensure that this new file is also in your project folder.

From here, we can open our folder in VS code and create a new file for our main code. We need to import some libraries.

<img src="/Route_Optimization_Subproblem/images/import_libraries.png" alt="Import code" height="150">
<!-- ![Import code](/Route_Optimization_Subproblem/images/import_libraries.png) -->

| import code         | Purpose                                                  |
| ------------- | -------------------------------------------------------- |
| **import osmnx as ox**     | Downloads Ontario's real road network from OpenStreetMap |
| **import networkx as nx**  | Calculates the shortest route using A\* search algorithm |
| **import geopandas as gpd** | Reads the nuclear site locations from a GeoJSON file     |
| **import folium**    | Draws an interactive map you can view in your browser    |
| **from shapely.geometry import Point** |Used to create our fire source and discourage routing through it|
| **from math import radians, cos, sin, sqrt, atan2**|For dist calculations|

### Load Nuclear Location File

<img src="/Route_Optimization_Subproblem/images/load_nuclear_location.png" alt="Load relavent sites from GeoJSON" height="25">
<!-- ![Load relavent sites from GeoJSON](/Route_Optimization_Subproblem/images/load_nuclear_location.png) -->
This line reads the geojson file we created and confirms for us that coordinates are in the correct format.

For debugging, you can include a line printing out the loaded site names, though this is not necessary for the function of the code as a whole. 

<img src="/Route_Optimization_Subproblem/images/print_loaded_sites.png" alt="Print loaded sites" height="25">
<!-- ![Print loaded sites](/Route_Optimization_Subproblem/images/print_loaded_sites.png) -->

### Defining our Heuristic
We can create a function to calculate the direct distance from a node to a site, which will be used when finding
the fastest route. It will take three inputs, the first node, second node, and the road network.

<img src="/Route_Optimization_Subproblem/images/def_haversine.png" alt="def_haversine" height="25">
<!-- ![def_haversine](/Route_Optimization_Subproblem/images/def_haversine.png)-->

Next we can find the longitude and latitude of the nodes from graph G.

<img src="/Route_Optimization_Subproblem/images/haversine1.png" alt="Find longitude and latitude" height="50">
<!-- ![Find longitude and latitude](/Route_Optimization_Subproblem/images/haversine1.png) -->

Now we need to set the radius of the sphere we are measuring on. In this case, we use earth, which has radius 6371 km.

<img src="/Route_Optimization_Subproblem/images/haversine2.png" alt="Radius" height="25">
<!-- ![Radius](/Route_Optimization_Subproblem/images/haversine2.png) -->

We can calculate the difference in longitude and latitude, and want to convert this to radians so that we can estimate the distance on the curve of the earth, as this uses trig functions.

<img src="/Route_Optimization_Subproblem/images/haversine3.png" alt="Distance calculation" height="50">
<!-- ![Distance calculation](/Route_Optimization_Subproblem/images/haversine3.png) -->

Finally, we can calculate the distance and convert it to kilometers.

<img src="/Route_Optimization_Subproblem/images/haversine4.png" alt="Return line" height="25">
<!-- ![Return line](/Route_Optimization_Subproblem/images/haversine4.png) -->

### Find Nearest Nodes to Sites
These steps specifically locate the nodes for Bruce Power and Ignace:  

<img src="/Route_Optimization_Subproblem/images/bruce_nearestnode.png" alt="Nearest node to Bruce" height="50">
<!-- ![Nearest node to Bruce](/Route_Optimization_Subproblem/images/bruce_nearestnode.png) -->
Repeat this for the other sites of interest (Ignace, Darlington, Chalk River Labs, etc.)

### Map Creation
Now, we have to make some way of actually showing the route we've found, and for that we have folium. This will create an empty map centered around Ontario.

<img src="/Route_Optimization_Subproblem/images/map_code.png" alt="Map creation" height="25">
<!-- ![Map creation](/Route_Optimization_Subproblem/images/map_code.png) -->

### Finding our Routes
we can use a built in a* algorithm with our haversine heuristic to calculate the most optimal route between sites.

<img src="/Route_Optimization_Subproblem/images/bruce_route.png" alt="Bruce route calculation" height="75">
<!-- ![Bruce route calculation](/Route_Optimization_Subproblem/images/bruce_route.png) -->
Again, repeat this for the other sites routing to ignace.

Now we need to populate it with route data! We can turn each node into a pair of coordinates, then use folium to create a line between them and add it to the map.
<!-- 'route_coords = [(G.nodes[n]['y'], G.nodes[n]['x']) for n in route]'
'folium.PolyLine(route_coords, color="green", weight=5).add_to(m)' -->
<img src="/Route_Optimization_Subproblem/images/route_coords_ex.png" alt="Route Data" height="50">
<!-- ![Route data](/Route_Optimization_Subproblem/images/route_coords_ex.png) -->

We want to highlight our sites with special markers for easy reading, which can be done easily by looping through each site with the following code.

<img src="/Route_Optimization_Subproblem/images/add_markers.png" alt="Add markers manually" height="175">
<!-- ![Add markers manually](/Route_Optimization_Subproblem/images/add_marker.png) -->

Finally, we should save the map so that we can open it in our browser, and print out a message confirming we have saved successfully.

<img src="/Route_Optimization_Subproblem/images/save_map.png" alt="Save map" height="50">
<!-- ![Save map](/Route_Optimization_Subproblem/images/save_map.png) -->

### Identify Fire Location and Radius
Now that we have functioning code, consider the limitless possibile additions to make this solution more realistic. For example, We can add hazards, such as fires. 

We can choose a point to be the midpoint of our fire, then set the size of the area around it.

<img src="/Route_Optimization_Subproblem/images/fire_location.png" alt="Fire location" height="50">
<!-- ![Fire location](/Route_Optimization_Subproblem/images/fire_location.png) -->
For the purpose of this challenge, a list of fire locations to avoid has been provided.

Now that we have set an area for our fire, we have to let the algorithm know that routing through the fire would be less optimal. To do this, we increase the weighting of the nodes within the specificed area.

<img src="/Route_Optimization_Subproblem/images/fire_penalty.png" alt="Fire zone penalty" height="150">
<!-- ![Fire zone penalty](/Route_Optimization_Subproblem/images/fire_penalty.png) -->
