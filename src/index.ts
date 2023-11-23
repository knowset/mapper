import { FeatureCollection, LineString } from "./geoJsonTypes";

const { Mapper } = require("./build/Release/Mapper");
import geojson from "./highway-line.json";

const mapper = new Mapper(geojson);

// test

try {
  mapper.findRoutes([[36.4644238,61.0245816], [36.4626106,61.0175432]]);
} catch (e) {
  console.error("Error message: ", e);
}
