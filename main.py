
from pysolar.solar import get_altitude, get_azimuth
from datetime import datetime, timezone

import warnings
warnings.filterwarnings("ignore", message="Leap seconds for year.*")

lat, lon = 37.35, -121.95
when = datetime.now(timezone.utc)

elevation = get_altitude(lat, lon, when)
azimuth = get_azimuth(lat, lon, when)

print(f"Elevation: {elevation:.2f} degrees")
print(f"Azimuth: {azimuth:.2f} degrees")