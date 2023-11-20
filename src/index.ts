import { FeatureCollection, LineString } from "./geoJsonTypes";

const { Mapper } = require("./build/Release/Mapper");
import geojson from "./highway-line.json";

const mapper = new Mapper(geojson);

// test
mapper.findRoutes([[38.4447323, 59.8532187], [38.006403, 59.130858]]);
