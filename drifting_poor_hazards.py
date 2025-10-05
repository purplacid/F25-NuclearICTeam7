import requests
import polyline
import geopandas as gpd
from shapely.geometry import LineString, Point

def fetch_drifting_poor_segments():
    url = "https://511on.ca/api/v2/get/roadconditions"
    resp = requests.get(url)
    data = resp.json()
    hazards = []
    for seg in data:
        drifting = seg.get("Drifting", "No")
        condition = seg.get("Condition", [])
        if drifting == "Yes" and any("Poor" in c for c in condition):
            # Use EncodedPolyline if available
            poly = seg.get("EncodedPolyline")
            if poly:
                coords = polyline.decode(poly)
                geom = LineString(coords)
                hazards.append(geom)
    # Return as GeoDataFrame for easy buffering
    gdf = gpd.GeoDataFrame(geometry=hazards, crs="EPSG:4326")
    return gdf

if __name__ == "__main__":
    gdf = fetch_drifting_poor_segments()
    print(gdf)
